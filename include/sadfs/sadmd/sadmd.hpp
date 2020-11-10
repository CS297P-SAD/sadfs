#ifndef SADFS_SADMD_SADMD_HPP
#define SADFS_SADMD_SADMD_HPP
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/socket.hpp>

#include <string>

namespace sadfs {

class sadmd
{
public:
	sadmd(char const*, int);

	// starts server
	void start();
private:
	// reads the message from a socket that just received some data
	std::string process_message(sadfs::socket const&);

	inet::service const service_;
};

} // sadfs namespace

#endif // SADFS_SADMD_SADMD_HPP
