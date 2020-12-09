#ifndef SADFS_SADCD_SADCD_HPP
#define SADFS_SADCD_SADCD_HPP
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/socket.hpp>
#include <sadfs/uuid.hpp>

#include <string>

namespace sadfs {

class sadcd
{
public:
	sadcd(char const* ip, int port, char const* master_ip, int master_port, char const* server_id);

	// starts server by opening a listener
	void start();
private:
	// reads the message from a socket that just received some data
	std::string process_message(comm::socket const&);

	void join_network();

	comm::service const service_;
	comm::service const master_;
	serverid serverid_;
};

} // sadfs namespace

#endif // SADFS_SADCD_SADCD_HPP
