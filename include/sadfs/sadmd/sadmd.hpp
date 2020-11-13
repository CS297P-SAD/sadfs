#ifndef SADFS_SADMD_SADMD_HPP
#define SADFS_SADMD_SADMD_HPP
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/socket.hpp>

#include <ctime>
#include <string>
#include <sqlite3.h>
#include <unordered_map>
#include <vector>

namespace sadfs {

// all the information needed about a chunk server
struct chunk_server_info{
	inet::ip_addr ip;
	inet::port_no port;
	int total_space;
	int used_space;
	time_t ttl;
};

// all the information needed about a file
struct file_info{
	bool locked;
	time_t ttl;
	std::vector<int> chunkids;
};

class sadmd
{
public:
	sadmd(char const* ip, int port);

	// starts server
	void start();
	// creates (the metadata for) a new file
	void create_file(std::string);
private:
	// reads the message from a socket that just received some data
	std::string process_message(sadfs::socket const&);
	// loads file metadata from disk
	void load_files();
	// runs an sql statement on system_files_
	void db_command(std::string);

	inet::service const service_;
	// in memory representation of each file
	std::unordered_map<std::string, file_info> files_;
	// list of active chunk servers
	std::vector<chunk_server_info> active_chunk_servers_;
	// map from chunkid to list of chunk servers
	std::unordered_map<int, std::vector<chunk_server_info*> > chunkids_;
	// persistent/on disk copy of files_
	sqlite3* files_db_;

};

} // sadfs namespace

#endif // SADFS_SADMD_SADMD_HPP
