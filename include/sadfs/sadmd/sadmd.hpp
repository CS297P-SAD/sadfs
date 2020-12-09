#ifndef SADFS_SADMD_SADMD_HPP
#define SADFS_SADMD_SADMD_HPP

// sadfs specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/socket.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/msgs/master/message_processor.hpp>
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

using time_point = std::chrono::steady_clock::rep;
using version = unsigned int;

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

// all the information needed about a chunk
struct chunk_info
{
	version latest_version;
	// all the places this chunk is stored and what version is there
	std::vector<std::pair<chunk_server_info*, version> > locations;
};

class sadmd
{
public:
	sadmd(char const* ip, int port);

	// starts server
	void start();

	// handles a chunk_location_request and responds to channel it came in on
	bool handle(msgs::master::chunk_location_request const&, 
                msgs::channel const&);

	// handles a chunk_server_hearbeat
	//bool handle(msgs::master::chunk_server_heartbeat const&, 
    //            msgs::channel const&);

	// handles a join_network_request and responds to channel it came in on
	bool handle(msgs::master::join_network_request const&, 
                msgs::channel const&);
private:
	// takes ownership of a channel and serves the request on it
	void serve_requests(msgs::channel);

	//friend class msgs::master::processor;

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

	void add_chunk_to_server(chunkid, version, serverid);

	void reintroduce_chunks_to_network(util::file_chunks);

	// returns true on success
	bool add_server_to_network(serverid, comm::service, uint64_t, uint64_t);
	void remove_server_from_network(serverid) noexcept;
	void register_server_heartbeat(serverid) noexcept;
	bool is_active(serverid) const noexcept;

	comm::service const service_;
	// in memory representation of each file
	std::unordered_map<std::string, file_info> files_;
	// metadata for each chunk server
	std::unordered_map<serverid, chunk_server_info> chunk_server_metadata_;
	// map from chunkid to chunk info
	std::unordered_map<chunkid, chunk_info> chunk_metadata_;
	// persistent/on disk copy of files_
	sqlite3* const files_db_;
};

} // sadfs namespace

#endif // SADFS_SADMD_SADMD_HPP
