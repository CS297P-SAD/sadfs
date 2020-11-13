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

using chunkid = unsigned int;

// all the information needed about a chunk server
struct chunk_server_info{
	inet::service service;
	unsigned int total_space;
	unsigned int used_space;
	int ttl;
};

// all the information needed about a file
struct file_info{
	int ttl;
	std::vector<chunkid> chunkids;
	// decides whether file is currently locked for writing
	bool locked();
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
	// reads the message from a socket that just received some data
	std::string process_message(sadfs::socket const&);
	// loads file metadata from disk
	void load_files();
	// runs an sql statement on system_files_
	void db_command(std::string const&) const noexcept;

	const inet::service service_;
	// in memory representation of each file
	std::unordered_map<std::string, file_info> files_;
	// list of active chunk servers
	std::vector<chunk_server_info> chunk_servers_;
	// map from chunkid to list of chunk servers
	std::unordered_map<chunkid, std::vector<chunk_server_info*> > chunkids_;
	// persistent/on disk copy of files_
	sqlite3* const files_db_;
	sqlite3* open_db();

};

} // sadfs namespace

#endif // SADFS_SADMD_SADMD_HPP
