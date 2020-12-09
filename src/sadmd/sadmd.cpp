/* Code for the sadmd class,  */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/client/serializer.hpp>
#include <sadfs/msgs/master/message_processor.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/proto/internal.pb.h>
#include <sadfs/sadmd/sadmd.hpp>

// standard includes
#include <array>
#include <cstring>  // std::strerror
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>  // std::osstream
#include <unistd.h> // read/write

namespace sadfs {

namespace {
namespace constants {

// columns in our SQLite database
constexpr auto filename_col = 0;
constexpr auto chunkid_str_col = 1;

} // (local) constants namespace

namespace time {

using namespace std::literals;
constexpr auto server_ttl = 1min;
constexpr auto file_ttl   = 1min;

time_point
from_now(std::chrono::minutes delta) noexcept
{
	return (std::chrono::steady_clock::now() + delta).time_since_epoch().count();
}

time_point 
now() noexcept
{
	// the time point zero minutes from now is, you guessed it, now
	return from_now(0min);
}
} // time namespace

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

std::vector<comm::service>
valid_servers(chunk_info& info, bool latest_only)
{
	auto is_active = [&info](auto location)
	{ 
		return location.first->valid_until > time::now();
	};
	auto is_active_latest_version = [&info](auto location){ 
		return location.first->valid_until > time::now() && 
		      location.second == info.latest_version;
	};
	auto filter = [&](auto location)
	{
		if (latest_only) { return is_active_latest_version(location); }
		return is_active(location);
	};

	auto services = std::vector<comm::service>{};
	services.reserve(info.locations.size());
	for (auto location : info.locations)
	{
		if (filter(location))
		{
			services.push_back(location.first->service);
		}
	}
	return services;
}
} // unnamed namespace

sadmd::
sadmd(char const* ip, int port) : service_(ip, port) , files_db_(open_db())
{
	// make sure the files table exists
	db_command("CREATE TABLE IF NOT EXISTS files (filename TEXT, chunkids TEXT);");
	// load files from the database to in-memory representation
	load_files();
}

void sadmd::
start()
{
	auto listener = comm::listener{service_};
	while (true)
	{
		auto sock = listener.accept();
		serve_requests(std::move(sock));
	}
}

void sadmd::
serve_requests(msgs::channel ch)
{
	auto processor = msgs::master::processor{};
	bool result{false}, eof{false};
	while (true)
	{
		auto [result, eof] = processor.process_next(ch, *this);
		if (result)
		{
			//std::cout << "request served successfully\n";
		}
		else if (eof)
		{
			//std::cout << "EOF\n";
			break;
		}
		else
		{
			std::cerr << "Error: request service failed\n";
		}
	}
}

bool sadmd::
handle(msgs::master::chunk_location_request const& clr, msgs::channel const& ch)
{
	auto id = chunkid{};
	auto servers = std::vector<comm::service>{};
	auto const& filename = clr.filename();

	// lambda to check if a chunk number is valid for filename
	auto validate = [&filename](auto it, auto chunk_number)
	{
		 // safe since we don't validate if iterator is invalid
		if (chunk_number >= it->second.chunkids.size())
		{
			std::cerr << "Error: request for too large chunk number: chunk "
					<< chunk_number
					<< " of "
					<< filename
					<< '\n';
			return false;
		}
		return true;
	};

	// look up relevant info
	auto it = files_.find(filename);
	if (it == files_.end())
	{
		std::cerr << "Error: request for chunk location in nonexistant file "
				  << filename
				  << '\n';
	}
	else if (validate(it, clr.chunk_number()))
	{
		id = it->second.chunkids[clr.chunk_number()];
		servers = valid_servers(chunk_locations_[id], 
					clr.io_type() == sadfs::msgs::io_type::read); 
		if (servers.size() <= 0)
		{
			std::cerr << "Error: list of server locations empty\n";
		}
	}
		
	// create the response protobuf
	auto response = msgs::client::chunk_location_response
	{
		servers.size() > 0,
		servers,
		id,
	};

	// send protobuf back over channel
	auto result = msgs::client::serializer{}.serialize(response, ch);
	ch.flush();
	return result;
}

bool sadmd::
handle(msgs::master::join_network_request const& jnr, msgs::channel const& ch)
{
	add_server_to_network(jnr.server_id(), 
						  jnr.service(),
						  jnr.max_chunks(), 
						  jnr.chunk_count());
	ch.flush();
	return true;
}

void sadmd::
create_file(std::string const& filename)
{
	load_file(filename, {});
}

void sadmd::
load_file(std::string const& filename, std::string const& existing_chunks)
{
	if (files_.count(filename))
	{
		std::cerr << "Error: attempt to load " 
				  << filename 
				  << ": file already exists\n";
		return;
	}
	files_.emplace(filename, file_info{});
	auto& file_chunkids  = files_[filename].chunkids;
	file_chunkids.deserialize(existing_chunks);
	reintroduce_chunks_to_network(file_chunkids);
}

void sadmd::
db_command(std::string const& stmt) const
{
	if (sqlite3_exec(files_db_, stmt.c_str(), nullptr, nullptr, nullptr) != 0)
	{
		std::cerr << "Error running {" << stmt << "}: ";
		std::cerr << sqlite3_errmsg(files_db_) << '\n';
		std::exit(1);
	}
}

bool sadmd::
db_contains(std::string const& filename) const
{
	auto contains = false;
	sqlite3_stmt* stmt;
	auto cmd = std::string{"SELECT * FROM files WHERE filename='" + filename +
		"';"};

	if (sqlite3_prepare_v2(files_db_, cmd.c_str(), -1, &stmt, nullptr) != 0)
	{
		std::cerr << sqlite3_errmsg(files_db_) << '\n';
		std::exit(1);
	}
	contains = (sqlite3_step(stmt) == SQLITE_ROW);
	sqlite3_finalize(stmt);

	return contains;
}

void sadmd::
load_files()
{
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(files_db_, "SELECT * FROM files;", -1, &stmt, nullptr) != 0)
	{
		std::cerr << sqlite3_errmsg(files_db_) << '\n';
		std::exit(1);
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		auto filename = std::string{reinterpret_cast<const char*>(
			sqlite3_column_text(stmt, constants::filename_col))};
		auto cids = std::string{reinterpret_cast<const char*>(
			sqlite3_column_text(stmt, constants::chunkid_str_col))};
		load_file(filename, cids);
	}
	sqlite3_finalize(stmt);
}

void sadmd::
save_files() const noexcept
{
	for (auto file : files_)
	{
		auto sql_command = std::string{};
		auto filename = std::string{file.first};
		auto chunkids = file.second.chunkids.serialize();

		if(db_contains(file.first))
		{
			sql_command += "UPDATE files SET chunkids='";
			sql_command += chunkids + "' WHERE filename='" + filename + "';";
		}
		else
		{
			sql_command += "INSERT INTO files VALUES('";
			sql_command += filename + "', '" +  chunkids + "');";
		}
		db_command(sql_command);
	}
}

bool sadmd::
add_server_to_network(serverid uuid, comm::service service, 
					  uint64_t max_chunks, uint64_t chunk_count)
{
	if (chunk_server_metadata_.count(uuid))
	{
		std::cerr << "Error: attempt to add server "
			<< to_string(uuid)
			<< " which is already on the network\n";
		return false;
	}
	chunk_server_metadata_.emplace(
		uuid,
		chunk_server_info{
			service,
			max_chunks,
			chunk_count,
			time::from_now(time::server_ttl)
		}
	);
	return true;
}
	
void sadmd::
remove_server_from_network(serverid id) noexcept
{
	chunk_server_metadata_.erase(id);
}

void sadmd::
register_server_heartbeat(serverid id) noexcept
{
	if (!chunk_server_metadata_.count(id))
	{
		std::cerr << "Error: received heartbeat from server "
			<< to_string(id)
			<< " which is not on the network\n";
		return;
	}
	// use the default value
	chunk_server_metadata_.at(id).valid_until = time::from_now(time::server_ttl);
}

bool sadmd::
is_active(serverid id) const noexcept
{
	if (!chunk_server_metadata_.count(id))
	{
		std::cerr << "Error: query for status of server "
			<< to_string(id)
			<< " which is not on the network\n";
		return false;
	}
	return (chunk_server_metadata_.at(id).valid_until) > time::now();
}

void sadmd::
append_chunk_to_file(std::string const& filename, chunkid new_chunkid)
{
	if (!files_.count(filename))
	{
		std::cerr << "Error: cannot append chunk to file "
				  << filename
				  << ": file does not exist\n";
		return;
	}
	files_[filename].chunkids.add_chunk(new_chunkid);
	chunk_metadata_.emplace(new_chunkid, chunk_info{});
}

void sadmd::
reintroduce_chunks_to_network(util::file_chunks ids)
{
	for (auto i = 0; i < ids.size(); i++)
	{
		chunk_metadata_.emplace(ids[i], chunk_info{});
	}
}

void sadmd::
add_chunk_to_server(chunkid cid, version v, serverid sid)
{
	if (!is_active(sid)) return;	
	auto server_ptr = &(chunk_server_metadata_.at(sid));
	auto& info = chunk_metadata_[cid];
	// add server to chunk's locations
	info.locations.emplace_back(server_ptr, v);
	// make sure latest version is correct
	info.latest_version = std::max<version>(info.latest_version, v);
}
  
} // sadfs namespace
