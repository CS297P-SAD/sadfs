#ifndef SADFS_SADCD_SADCD_HPP
#define SADFS_SADCD_SADCD_HPP
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/socket.hpp>

#include <string>

namespace sadfs {

class sadcd
{
public:
	sadcd(char const* ip, int port);

	// starts server by opening a listener
	void start();
private:
	// reads the message from a socket that just received some data
	std::string process_message(comm::socket const&);

	comm::service const service_;
};

} // sadfs namespace

#endif // SADFS_SADCD_SADCD_HPP
