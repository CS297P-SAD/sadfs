/* implementation of inet communication components */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/defaults.hpp>

// standard includes
#include <arpa/inet.h>  // inet_aton
#include <netinet/in.h> // in_addr
#include <sys/socket.h> // bind, listen

namespace sadfs { namespace inet {

ip_addr::
ip_addr(char const* ip)
{
	in_addr tmp;
	if (inet_aton(ip, &tmp) == 0)
	{
		// ip is malformed
		// TODO: throw exception
	}
	addr_ = tmp.s_addr;
}

std::uint32_t ip_addr::
value() const noexcept
{
	return addr_;
}

listener::
listener(ip_addr ip, port_no port)
	: sock_(socket::domain::inet, socket::type::stream)
{
	// bind to ip + port
	sockaddr_in addr
	{
		.sin_len    = 0,
		.sin_family = AF_INET,
		.sin_port   = port,
		.sin_addr   {ip.value()},
		.sin_zero   {}
	};
	if (::bind(sock_.descriptor(),
	           reinterpret_cast<sockaddr const*>(&addr),
			   sizeof(addr)) == -1)
	{
		// TODO: throw exception
	}

	// specify that connections will be accepted on socket
	if (::listen(sock_.descriptor(), ::sadfs::defaults::somaxconn) == -1)
	{
		// TODO; throw exception
	}
}

} // namespace inet
} // namespace sadfs
