#ifndef SADFS_COMM_INET_HPP
#define SADFS_COMM_INET_HPP

// sadfs-specific includes
#include <sadfs/comm/socket.hpp>

// standard includes
#include <cstdint>

namespace sadfs { namespace inet {

using port_no = std::uint16_t;

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
	ip_addr();
	std::uint32_t value() const noexcept;
	void set(char const* ip);
private:
	std::uint32_t addr_;
};

// abstracts listening on a port number for
// incoming connections
class listener
{
public:
	// creates a listener on socket specified by
	// ip_addr and port_no
	listener(ip_addr const&, port_no);

	listener(listener const&) = delete;
	listener(listener&&) = default;
	~listener() = default;

	// accepts a new connection and returns a socket
	socket accept() const;
private:
	ip_addr ip_;
	port_no port_;
	socket  socket_;
};

namespace constants { // sadfs::inet::constants

inline ip_addr const ip_localhost("127.0.0.1");

} // constants namespace
} // inet
} // sadfs

#endif // SADFS_COMM_INET_HPP
