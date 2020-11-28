/* Code for the sadmd class,  */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/sadmd/sadmd.hpp>
#include <sadfs/proto/internal.pb.h>

// standard includes
#include <array>
#include <cstring>  // std::strerror
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

std::string
serialize(std::vector<uint64_t>& chunkids) noexcept
{
	auto chunkid_pb = sadfs::proto::internal::chunkid_container{};
	auto chunkid_str = std::string{};

	for (auto chunkid : chunkids)
	{
		chunkid_pb.add_chunkids(chunkid);
	}
	chunkid_pb.SerializeToString(&chunkid_str);

	return chunkid_str;
}

void
deserialize(std::vector<uint64_t>& chunkids, 
	std::string const& existing_chunks)
{
	if (existing_chunks.size() == 0)
	{
		// do nothing if string is empty
		return;
	}
	// parse string into protobuf object
	auto chunkid_pb = sadfs::proto::internal::chunkid_container{};
	chunkid_pb.ParseFromString(existing_chunks);
	
	// clear chunkids, and allocate enough space for contents
	chunkids.clear();
	chunkids.reserve(chunkid_pb.chunkids_size());
	
	// copy items from protobuf object into vector
	for (auto id : chunkid_pb.chunkids())
	{
		chunkids.push_back(id);
	}
}

std::string
process_message(comm::socket const& sock)
{
	auto buf = std::array<char, 512>{};
	auto len = 0;
	auto result = std::string{};
	while ((len = ::read(sock.descriptor(), buf.data(), buf.size())))
	{
		if (len == -1)
		{
			std::cerr << "read error\n";
			std::cerr << std::strerror(errno) << std::endl;
			std::exit(1);
		}
		if (::write(sock.descriptor(), buf.data(), len) == -1)
		{
			std::cerr << "write error\n";
			std::exit(1);
		}
		for (auto i = 0; i < len; i++)
		{
			result += buf[i];
		}
		buf.fill({});
	}

	return result;
}

} // unnamed namespace

namespace time{

using namespace std::literals;
constexpr auto server_ttl = 1min;
constexpr auto file_ttl = 1min;

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
		auto result = process_message(sock);

		std::cout << result << "\n";
		// perform some action based on result
	}
}

void sadmd::
create_file(std::string const& filename, std::string const& existing_chunks)
{
	if (!files_.count(filename))
	{
		auto info = file_info{};
		deserialize(info.chunkids, existing_chunks);
		files_.emplace(filename, info);
	}
	else
	{
		// TODO: give a more meaningful error message to the user
		std::cerr << "Error: " << filename << ": file already exists\n";
		std::exit(1);
	}
	
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
		create_file(filename, cids);
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
		auto chunkids = serialize(file.second.chunkids);

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
add_server_to_network(serverid uuid, char const* ip, int port, 
					  uint64_t max_chunks, uint64_t chunk_count)
{
	if (chunk_server_metadata_.count(uuid))
	{
		std::cerr << "Error: attempt to add server "
			<< uuid
			<< " which is already on the network\n";
		return false;
	}
	chunk_server_metadata_.emplace(
		uuid,
		chunk_server_info{
			comm::service(ip, port),
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
			<< id
			<< " which is not on the network\n";
		return;
	}
	// use the default value
	chunk_server_metadata_.at(id).expiration_point = time::from_now(time::server_ttl);
}

bool sadmd::
is_active(serverid id) const noexcept
{
	if (!chunk_server_metadata_.count(id))
	{
		std::cerr << "Error: query for status of server "
			<< id
			<< " which is not on the network\n";
		return false;
	}
	return (chunk_server_metadata_.at(id).expiration_point) > time::now();
}
} // sadfs namespace
