/* implementation of inet communication components */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/defaults.hpp>

// standard includes
#include <arpa/inet.h>  // inet_aton, htons
#include <netinet/in.h> // in_addr
#include <sys/socket.h> // bind, listen, accept

namespace sadfs { namespace inet {

ip_addr::
ip_addr(char const* ip)
{
	auto tmp = in_addr{};
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
	auto addr = sockaddr_in
	{
		.sin_len    = 0,
		.sin_family = AF_INET,
		.sin_port   = htons(port),
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

socket listener::
accept() const
{
	auto addr = sockaddr{};
	auto len = socklen_t{sizeof(addr)};
	auto fd = ::accept(sock_.descriptor(),
	                   reinterpret_cast<sockaddr*>(&addr),
	                   &len);
	if (fd == -1)
	{
		// TODO: throw exception
	}

	return {sock_.comm_domain(), sock_.socket_type(), fd};
}

} // namespace inet
} // namespace sadfs
