#ifndef SADFS_SADMD_SADMD_HPP
#define SADFS_SADMD_SADMD_HPP

// sadfs specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/socket.hpp>
#include <sadfs/uuid.hpp>
#include "util.hpp" // file_chunks object

// extrenal includes
#include <sqlite3.h>

// standard includes
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace sadfs {

using chunkid = sadfs::uuid;
using serverid = sadfs::uuid;
using time_point = std::chrono::steady_clock::rep;

// all the information needed about a chunk server
struct chunk_server_info
{
	comm::service service;
	uint64_t max_chunks;
	uint64_t chunk_count;
	time_point valid_until;
};

// all the information needed about a file
struct file_info
{
	int ttl;
	util::file_chunks chunkids;
};

class sadmd
{
public:
	sadmd(char const* ip, int port);

	// starts server
	void start();

private:
	// creates (the metadata for) a new file
	void create_file(std::string const&);

	// loads file metadata from disk
	void load_files();
	void load_file(std::string const&, std::string const&);

	// copies in-memory files into database
	void save_files() const noexcept;

	// runs an sql statement on system_files_
	void db_command(std::string const&) const;

	// returns true if the database contains a file with the given name
	bool db_contains(std::string const&) const;

	// functions for maintaining chunk servers 

	void append_chunk_to_file(std::string const&, chunkid);

	void reintroduce_chunks_to_network(util::file_chunks);

	// returns true on success
	bool add_server_to_network(serverid, char const*, int, uint64_t, uint64_t);
	void remove_server_from_network(serverid) noexcept;
	void register_server_heartbeat(serverid) noexcept;
	bool is_active(serverid) const noexcept;

	comm::service const service_;
	// in memory representation of each file
	std::unordered_map<std::string, file_info> files_;
	// metadata for each chunk server
	std::unordered_map<serverid, chunk_server_info> chunk_server_metadata_;
	// map from chunkid to list of chunk servers
	std::unordered_map<chunkid, std::vector<chunk_server_info*> > chunk_locations_;
	// persistent/on disk copy of files_
	sqlite3* const files_db_;
};

} // sadfs namespace

#endif // SADFS_SADMD_SADMD_HPP
