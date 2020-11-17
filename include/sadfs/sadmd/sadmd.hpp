#ifndef SADFS_SADMD_SADMD_HPP
#define SADFS_SADMD_SADMD_HPP
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/socket.hpp>

#include <chrono>
#include <string>
#include <sqlite3.h>
#include <unordered_map>
#include <vector>

namespace sadfs {

using chunkid = uint64_t;

// all the information needed about a chunk server
struct chunk_server_info{
	inet::service service;
	uint64_t max_chunks;
	uint64_t chunk_count;
	int ttl;
};

// all the information needed about a file
struct file_info{
	int ttl;
	std::vector<chunkid> chunkids;
};

class sadmd
{
public:
	sadmd(char const* ip, int port);

	// starts server
	void start();
	// creates (the metadata for) a new file
	void create_file(std::string const&);
private:
	// loads file metadata from disk
	void load_files();
	// runs an sql statement on system_files_
	void db_command(std::string const&) const noexcept;

	inet::service const service_;
	// in memory representation of each file
	std::unordered_map<std::string, file_info> files_;
	// metadata for each chunk server
	std::unordered_map<uint64_t, chunk_server_info> chunk_server_metadata_;
	// map from chunkid to list of chunk servers
	std::unordered_map<chunkid, std::vector<chunk_server_info*> > chunk_locations_;
	// persistent/on disk copy of files_
	sqlite3* const files_db_;
};

} // sadfs namespace

#endif // SADFS_SADMD_SADMD_HPP
