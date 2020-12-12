/* Code for the sadmd class,  */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/client/serializer.hpp>
#include <sadfs/msgs/master/message_processor.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/proto/internal.pb.h>
#include <sadfs/sadmd/sadmd.hpp>

// standard includes
#include <array>
#include <cstring> // std::strerror
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>  // std::osstream
#include <unistd.h> // read/write

namespace sadfs
{

namespace
{
namespace constants
{

// columns in our SQLite database
constexpr auto filename_col    = 0;
constexpr auto chunkid_str_col = 1;
constexpr auto bytes_per_chunk = 64 * 1024 * 1024; // 64 MB

} // namespace constants

namespace time
{

using namespace std::literals;
constexpr auto server_ttl = 1min;
constexpr auto file_ttl   = 1min;

time_point
from_now(std::chrono::minutes delta) noexcept
{
    return (std::chrono::steady_clock::now() + delta)
        .time_since_epoch()
        .count();
}

time_point
now() noexcept
{
    // the time point zero minutes from now is, you guessed it, now
    return from_now(0min);
}
} // namespace time

sqlite3*
open_db()
{
    sqlite3* db;

    // open the sadmd database
    if (sqlite3_open("sadmd.db", &db) != 0)
    {
        std::cerr << "Error: Couldn't open database: ";
        std::cerr << sqlite3_errmsg(db) << '\n';
        std::exit(1);
    }
    return db;
}
} // unnamed namespace

std::vector<comm::service>
choose_servers(int                                              num_servers,
               std::unordered_map<serverid, chunk_server_info>& server_map)
{
    auto services = std::vector<comm::service>{};
    // TODO : this decision process should be randomized
    for (auto server : server_map)
    {
        if (server.second.valid_until > time::now())
        {
            services.push_back(server.second.service);
        }
        if (services.size() >= num_servers)
            break;
    }

    return services;
}

std::vector<comm::service>
sadmd::valid_servers(chunk_info& info, bool latest_only = true)
{
    auto is_active_latest_version = [&info](auto version, auto valid_until) {
        return valid_until > time::now() && version == info.latest_version;
    };
    auto filter = [&](auto version, auto valid_until) {
        if (latest_only)
        {
            return is_active_latest_version(version, valid_until);
        }
        return valid_until > time::now();
    };

    auto services = std::vector<comm::service>{};
    services.reserve(info.locations.size());
    for (auto location : info.locations)
    {
        auto server = chunk_server_metadata_.at(location.first);
        if (filter(location.second /*=version*/, server.valid_until))
        {
            services.push_back(server.service);
        }
    }
    return services;
}

sadmd::sadmd(char const* ip, int port)
    : service_(ip, port), files_db_(open_db())
{
    // make sure the files table exists
    db_command(
        "CREATE TABLE IF NOT EXISTS files (filename TEXT, chunkids TEXT);");
    // load files from the database to in-memory representation
    load_files();
}

void
sadmd::start()
{
    auto listener = comm::listener{service_};
    while (true)
    {
        auto sock = listener.accept();
        serve_requests(std::move(sock));
    }
}

void
sadmd::serve_requests(msgs::channel ch)
{
    auto processor = msgs::master::processor{};
    bool result{false}, eof{false};
    while (true)
    {
        auto [result, eof] = processor.process_next(ch, *this);
        if (result)
        {
            // std::cout << "request served successfully\n";
        }
        else if (eof)
        {
            // std::cout << "EOF\n";
            break;
        }
        else
        {
            std::cerr << "Error: request service failed\n";
        }
    }
}

bool
sadmd::handle(msgs::master::file_metadata_request const& fmr,
              msgs::message_header const&, msgs::channel const& ch)
{
    auto it       = files_.find(fmr.filename());
    auto response = msgs::client::file_metadata_response{
        it != files_.end(),                        /* ok */
        it == files_.end() ? 0 : it->second.size}; /* size */
    auto result = msgs::client::serializer{}.serialize(response, ch);
    ch.flush();
    return result;
}

bool
sadmd::handle(msgs::master::create_file_request const& cfr,
              msgs::message_header const&, msgs::channel const& ch)
{
    return create_file(cfr.filename());
    // TODO: send ack
}

bool
sadmd::handle(msgs::master::chunk_location_request const& clr,
              msgs::message_header const&, msgs::channel const& ch)
{
    auto        id          = chunkid{};
    auto        servers     = std::vector<comm::service>{};
    auto const& filename    = clr.filename();
    auto        version_num = version{0};

    // lambda to check if a chunk number is valid for filename
    auto validate = [](auto it, auto chunk_number) {
        // safe since we don't validate if iterator is invalid
        if (chunk_number >= it->second.chunkids.size())
        {
            std::cerr << "Error: request for too large chunk number: chunk "
                      << chunk_number << " of " << it->first // filename
                      << '\n';
            return false;
        }
        return true;
    };

    // validate file name
    auto it = files_.find(filename);
    if (it == files_.end())
    {
        std::cerr << "Error: request for chunk location in nonexistant file "
                  << filename << '\n';
    }
    else if (clr.io_type() == sadfs::msgs::io_type::write)
    {
        if (it->second.locked_until > time::now())
        {
            std::cerr << "File locked\n";
        }
        else if (clr.chunk_number() == it->second.chunkids.size())
        {
            // request to write just after last chunk - generate new info
            id      = chunkid::generate();
            servers = choose_servers(3, chunk_server_metadata_);
            // (version num is already 0)
            it->second.locked_until = time::from_now(time::file_ttl);
        }
        else if (validate(it, clr.chunk_number()))
        {
            // request to write to last chunk - lookup info
            id          = it->second.chunkids[clr.chunk_number()];
            auto& chunk = chunk_metadata_[id];
            servers     = valid_servers(chunk);
            version_num = chunk.latest_version;
            if (servers.size() <= 0)
            {
                std::cerr << "Error: list of server locations empty\n";
            }
            it->second.locked_until = time::from_now(time::file_ttl);
        }
        else
        {
            // All other chunk numbers are invalid
            std::cerr << "Error: attempt to write to chunk "
                      << clr.chunk_number() << " of " << filename
                      << " which is not the last chunk\n";
        }
    }
    else
    {
        if (validate(it, clr.chunk_number()))
        {
            id          = it->second.chunkids[clr.chunk_number()];
            auto& chunk = chunk_metadata_[id];
            servers     = valid_servers(chunk);
            version_num = chunk.latest_version;
            if (servers.size() <= 0)
            {
                std::cerr << "Error: list of server locations empty\n";
            }
        }
    }

    auto response = msgs::client::chunk_location_response{
        servers.size() > 0, servers, id, version_num};

    // send protobuf back over channel
    auto result = msgs::client::serializer{}.serialize(response, ch);
    ch.flush();
    return result;
}

bool
sadmd::handle(msgs::master::join_network_request const& jnr,
              msgs::message_header const& header, msgs::channel const& ch)
{
    add_server_to_network(header.host_id, jnr.service(), jnr.max_chunks(),
                          jnr.chunk_count());
    return true;
}

bool
sadmd::handle(msgs::master::chunk_write_notification const& cwn,
              msgs::message_header const& header, msgs::channel const& ch)
{
    auto server = header.host_id;
    // validate request
    if (!is_active(server))
    {
        std::cerr
            << "Error: write notification from server not on the network\n";
        return false;
    }

    if (!files_.count(cwn.filename()))
    {
        std::cerr << "Error: write notification for nonexistant file\n";
        return false;
    }

    auto chunk = cwn.chunk_id();
    // should update the size IF this is the latest version of the last chunk
    // in the file
    auto update_size = false;

    auto it = chunk_metadata_.end();
    if ((it = chunk_metadata_.find(chunk)) != chunk_metadata_.end())
    {
        // chunk is already registerd - update version info
        auto& info = it->second;
        info.latest_version =
            std::max<version>(info.latest_version, cwn.version());
        auto location = info.locations.end();
        if ((location = info.locations.find(server)) != info.locations.end())
        {
            // chunk was already stored on this server - update version info
            info.locations[server] = cwn.version();
        }
        else
        {
            // chunk was not previously on this server
            add_chunk_to_server(chunk, cwn.version(), server);
        }
        update_size = info.latest_version == cwn.version();
    }
    else
    {
        // new chunk - add to file and record its location
        if (add_chunk_to_server(chunk, cwn.version(), server))
        {
            append_chunk_to_file(cwn.filename(), chunk);
            update_size = true;
        }
    }
    if (update_size)
    {
        auto& file_info_ = files_[cwn.filename()];
        auto  num_chunks = file_info_.chunkids.size();
        // check that this is the last chunk in the file
        if (chunk == file_info_.chunkids[num_chunks - 1])
        {
            file_info_.size = ((num_chunks - 1) * constants::bytes_per_chunk) +
                              cwn.new_size();
            logger::debug(std::string_view{"Updated size of " +
                                           cwn.filename() + " to " +
                                           std::to_string(file_info_.size)});
        }
    }

    return true;
}

bool
sadmd::handle(msgs::master::release_lock const& rl,
              msgs::message_header const&, msgs::channel const& ch)
{
    auto it = files_.find(rl.filename());
    if (it != files_.end())
    {
        logger::debug("unlocked file"sv);
        it->second.locked_until = time::now();
    }
    // TODO: send ack
    return true;
}

bool
sadmd::create_file(std::string const& filename)
{
    return load_file(filename, {});
}

bool
sadmd::load_file(std::string const& filename,
                 std::string const& existing_chunks)
{
    if (files_.count(filename))
    {
        std::cerr << "Error: attempt to load " << filename
                  << ": file already exists\n";
        return false;
    }
    files_.emplace(filename, file_info{});
    auto& file_chunkids = files_[filename].chunkids;
    file_chunkids.deserialize(existing_chunks);
    reintroduce_chunks_to_network(file_chunkids);
    return true;
}

void
sadmd::db_command(std::string const& stmt) const
{
    if (sqlite3_exec(files_db_, stmt.c_str(), nullptr, nullptr, nullptr) != 0)
    {
        std::cerr << "Error running {" << stmt << "}: ";
        std::cerr << sqlite3_errmsg(files_db_) << '\n';
        std::exit(1);
    }
}

bool
sadmd::db_contains(std::string const& filename) const
{
    auto          contains = false;
    sqlite3_stmt* stmt;
    auto          cmd =
        std::string{"SELECT * FROM files WHERE filename='" + filename + "';"};

    if (sqlite3_prepare_v2(files_db_, cmd.c_str(), -1, &stmt, nullptr) != 0)
    {
        std::cerr << sqlite3_errmsg(files_db_) << '\n';
        std::exit(1);
    }
    contains = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);

    return contains;
}

void
sadmd::load_files()
{
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(files_db_, "SELECT * FROM files;", -1, &stmt,
                           nullptr) != 0)
    {
        std::cerr << sqlite3_errmsg(files_db_) << '\n';
        std::exit(1);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        auto filename = std::string{reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, constants::filename_col))};
        auto cids     = std::string{reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, constants::chunkid_str_col))};
        load_file(filename, cids);
    }
    sqlite3_finalize(stmt);
}

void
sadmd::save_files() const noexcept
{
    for (auto file : files_)
    {
        auto sql_command = std::string{};
        auto filename    = std::string{file.first};
        auto chunkids    = file.second.chunkids.serialize();

        if (db_contains(file.first))
        {
            sql_command += "UPDATE files SET chunkids='";
            sql_command += chunkids + "' WHERE filename='" + filename + "';";
        }
        else
        {
            sql_command += "INSERT INTO files VALUES('";
            sql_command += filename + "', '" + chunkids + "');";
        }
        db_command(sql_command);
    }
}

bool
sadmd::add_server_to_network(serverid uuid, comm::service service,
                             uint64_t max_chunks, uint64_t chunk_count)
{
    if (chunk_server_metadata_.count(uuid))
    {
        std::cerr << "Error: attempt to add server " << to_string(uuid)
                  << " which is already on the network\n";
        return false;
    }
    chunk_server_metadata_.emplace(
        uuid, chunk_server_info{service, max_chunks, chunk_count,
                                time::from_now(time::server_ttl)});
    return true;
}

void
sadmd::remove_server_from_network(serverid id) noexcept
{
    chunk_server_metadata_.erase(id);
}

void
sadmd::register_server_heartbeat(serverid id) noexcept
{
    if (!chunk_server_metadata_.count(id))
    {
        std::cerr << "Error: received heartbeat from server " << to_string(id)
                  << " which is not on the network\n";
        return;
    }
    // use the default value
    chunk_server_metadata_.at(id).valid_until =
        time::from_now(time::server_ttl);
}

bool
sadmd::is_active(serverid id) const noexcept
{
    if (!chunk_server_metadata_.count(id))
    {
        std::cerr << "Error: query for status of server " << to_string(id)
                  << " which is not on the network\n";
        return false;
    }
    return (chunk_server_metadata_.at(id).valid_until) > time::now();
}

void
sadmd::append_chunk_to_file(std::string const& filename, chunkid new_chunkid)
{
    files_[filename].chunkids.add_chunk(new_chunkid);
    chunk_metadata_.emplace(new_chunkid, chunk_info{});
}

void
sadmd::reintroduce_chunks_to_network(util::file_chunks ids)
{
    for (auto i = 0; i < ids.size(); i++)
    {
        chunk_metadata_.emplace(ids[i], chunk_info{});
    }
}

bool
sadmd::add_chunk_to_server(chunkid cid, version v, serverid sid)
{
    auto& info = chunk_metadata_[cid];
    // add server to chunk's locations
    info.locations.emplace(sid, v);
    // make sure latest version is correct
    info.latest_version = std::max<version>(info.latest_version, v);
    return true;
}

} // namespace sadfs
