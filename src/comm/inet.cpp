/* implementation of inet communication components */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/defaults.hpp>

// standard includes
#include <arpa/inet.h>  // inet_aton, inet_ntoa, htons, ntohs
#include <cerrno>       // errno
#include <netinet/in.h> // in_addr
#include <sstream>      // std::stringstream
#include <system_error> // std::system_error, std::system_category
#include <sys/socket.h> // bind, listen, accept

namespace sadfs { namespace inet {

ip_addr::
ip_addr(char const* ip)
{
	auto tmp = in_addr{};
	if (inet_aton(ip, &tmp) == 0)
	{
		auto err = std::stringstream{};
		err << ip << " is malformed";
		throw std::invalid_argument(err.str());
	}
	addr_ = tmp.s_addr;
}

std::uint32_t ip_addr::
value() const noexcept
{
	return addr_;
}

namespace {

// lambda that formats error messages
auto fmt_err = [](char const* msg, ip_addr const& ip, port_no const port)
{
	auto err = std::stringstream{};
	err << msg << inet_ntoa({ip.value()})
	    << ":" << ntohs(port);
	return err.str();
};

} // unnamed namespace

listener::
listener(ip_addr const& ip, port_no port)
	: ip_(ip), port_(port),
	  socket_(socket::domain::inet, socket::type::stream)
{
	// bind to ip + port
	auto addr = sockaddr_in{};
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(port_);
	addr.sin_addr   = {ip_.value()};
	if (::bind(socket_.descriptor(),
	           reinterpret_cast<sockaddr const*>(&addr),
	           sizeof(addr)) == -1)
	{
		throw std::system_error(errno, std::system_category(),
		          fmt_err("failed to bind socket to: ", ip_, port_));
	}

	// specify that connections will be accepted on socket
	if (::listen(socket_.descriptor(), ::sadfs::defaults::somaxconn) == -1)
	{
		throw std::system_error(errno, std::system_category(),
		          fmt_err("failed to listen on: ", ip_, port_));
	}
}

socket listener::
accept() const
{
	auto addr = sockaddr{};
	auto len = socklen_t{sizeof(addr)};
	auto desc = ::accept(socket_.descriptor(),
	                     reinterpret_cast<sockaddr*>(&addr),
	                     &len);
	if (desc == -1)
	{
		throw std::system_error(errno, std::system_category(),
		          fmt_err("failed to accept a connection on: ", ip_, port_));
	}

	return {socket_.comm_domain(), socket_.socket_type(), desc};
}

} // namespace inet
} // namespace sadfs
