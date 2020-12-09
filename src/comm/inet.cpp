/* implementation of inet communication components */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/defaults.hpp>

// standard includes
#include <arpa/inet.h>  // inet_aton, htons
#include <cerrno>       // errno
#include <limits>       // std::numeric_limits
#include <netinet/in.h> // in_addr
#include <string>
#include <system_error> // std::system_error, std::system_category
#include <sys/socket.h> // bind, listen, accept

namespace sadfs { namespace comm {
using namespace std::string_literals;

namespace {

// lambda that formats error messages
auto fmt_err = [](char const* msg, ip_addr const& ip, port_no const& port)
{
	return ""s // explicitly tell the compiler that we want a string
	       + msg + to_string(ip) + ":"
	       + std::to_string(to_int(port));
};

// verifies that an ip address in string format is valid,
// and converts it to network byte-order
std::uint32_t
parse_ip_addr(char const* ip)
{
	auto tmp = in_addr{};
	if (inet_aton(ip, &tmp) == 0)
	{
		throw std::invalid_argument(ip + " is malformed"s);
	}
	return tmp.s_addr;
}

// verifies that a port number is within bounds,
// and converts it to network byte-order
std::uint16_t
parse_port_no(int port)
{
	if (port > std::numeric_limits<std::uint16_t>::max())
	{
		throw std::invalid_argument("invalid port number: "
		                            + std::to_string(port));
	}
	return htons(static_cast<std::uint16_t>(port));
}

} // unnamed namespace

/* -------------------------------------------------------------
 *                           ip_addr
 * -------------------------------------------------------------
 */
ip_addr::
ip_addr(char const* ip) : value_(parse_ip_addr(ip))
{
	// do nothing
}

std::uint32_t ip_addr::
value() const noexcept
{
	return value_;
}

/* -------------------------------------------------------------
 *                           port_no
 * -------------------------------------------------------------
 */
port_no::
port_no(int port) : value_(parse_port_no(port))
{
	// do nothing
}

std::uint16_t port_no::
value() const noexcept
{
	return value_;
}

/* -------------------------------------------------------------
 *                           service
 * -------------------------------------------------------------
 */
service::
service(char const* ip, int port) : ip_(ip), port_(port)
{
	// do nothing
}

ip_addr service::
ip() const noexcept
{
	return ip_;
}

port_no service::
port() const noexcept
{
	return port_;
}

socket service::
connect() const
{
	// create a socket
	auto sock = socket{socket::domain::inet, socket::type::stream};
	auto addr = sockaddr_in{};
	addr.sin_family = AF_INET;
	addr.sin_port   = port_.value();
	addr.sin_addr   = {ip_.value()};

	// connect to service using sock
	if (::connect(sock.descriptor(),
	              reinterpret_cast<sockaddr const*>(&addr),
	              sizeof(addr)) == -1)
	{
		throw std::system_error(errno, std::system_category(),
		          fmt_err("failed to connect to: ", ip_, port_));
	}
	return sock;
}

/* -------------------------------------------------------------
 *                           listener
 * -------------------------------------------------------------
 */
listener::
listener(service const& s)
	: service_(s),
	  socket_(socket::domain::inet, socket::type::stream)
{
	// bind to ip + port
	auto const& ip = service_.ip();
	auto const& port = service_.port();
	auto addr = sockaddr_in{};
	addr.sin_family = AF_INET;
	addr.sin_port   = port.value();
	addr.sin_addr   = {ip.value()};
	if (::bind(socket_.descriptor(),
	           reinterpret_cast<sockaddr const*>(&addr),
	           sizeof(addr)) == -1)
	{
		throw std::system_error(errno, std::system_category(),
		          fmt_err("failed to bind socket to: ", ip, port));
	}

	// specify that connections will be accepted on socket
	if (::listen(socket_.descriptor(), ::sadfs::defaults::somaxconn) == -1)
	{
		throw std::system_error(errno, std::system_category(),
		          fmt_err("failed to listen on: ", ip, port));
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
		auto const& ip = service_.ip();
		auto const& port = service_.port();
		throw std::system_error(errno, std::system_category(),
		          fmt_err("failed to accept a connection on: ", ip, port));
	}

	return {socket_.comm_domain(), socket_.socket_type(), desc};
}

} // namespace comm
} // namespace sadfs
