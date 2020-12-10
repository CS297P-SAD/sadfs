#ifndef SADFS_COMM_INET_HPP
#define SADFS_COMM_INET_HPP

// sadfs-specific includes
#include "socket.hpp"

// standard includes
#include <arpa/inet.h> // inet_ntoa, ntohs
#include <cstdint>     // std::uint32_t, std::uint16_t
#include <string>

namespace sadfs { namespace comm {

// encodes a dotted-decimal ip address as a
// 32-bit unsigned integer in network byte order
// see: inet_aton(3)
class ip_addr
{
public:
	// ip should be a string literal representing
	// an address in dotted-decimal fashion
	// ex: "127.0.0.1"
	ip_addr(char const* ip);
	std::uint32_t value() const noexcept;
private:
	std::uint32_t const value_;
};

// returns the dotted-decimal representation of ip
inline std::string
to_string(ip_addr const& ip)
{
	return inet_ntoa({ip.value()});
}

// represents a port number
class port_no
{
public:
	// port must be a number in [0, 65535]
	port_no(int port);
	std::uint16_t value() const noexcept;
private:
	std::uint16_t const value_;
};

// returns the int representation of port
inline int
to_int(port_no const& port) noexcept
{
	return ntohs(port.value());
}

// represents a service on a host,
// specified by an IP + port number
class service
{
public:
	service(char const* ip, int port);

	// sets up a connection to the service
	socket connect() const noexcept;

	// accessors
	ip_addr ip()   const noexcept;
	port_no port() const noexcept;
private:
	ip_addr const ip_;
	port_no const port_;
};

// abstracts listening for connections directed at
// a host, i.e. ip + port number
class listener
{
public:
	// creates a listener for connections made
	// to a service
	listener(service const&);

	listener(listener const&) = delete;
	listener(listener&&) = default;
	~listener() = default;

	// accepts a new connection and returns a socket
	socket accept() const;
private:
	service const service_;
	socket        socket_;
};

namespace constants { // sadfs::comm::constants

inline constexpr auto ip_localhost{"127.0.0.1"};

} // constants namespace
} // comm namespace
} // sadfs namespace

#endif // SADFS_COMM_INET_HPP
