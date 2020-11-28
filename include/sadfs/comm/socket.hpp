#ifndef SADFS_SOCKET_HPP
#define SADFS_SOCKET_HPP

namespace sadfs { namespace comm {

class socket
{
public:
	enum class domain { local, inet };
	enum class type { stream, datagram };

	// creates a new socket
	socket(domain const, type const);

	// creates a representation of an existing socket
	// does not create a new socket
	socket(domain const, type const, int const) noexcept;

	// support move operations
	socket(socket&&) noexcept;
	socket& operator=(socket&&) noexcept;

	// sockets must not be copied
	socket(socket const&) = delete;
	socket& operator=(socket const&) = delete;

	~socket() noexcept;

	// accessors
	domain comm_domain() const noexcept;
	type   socket_type() const noexcept;
	int    descriptor()  const noexcept;

private:
	domain domain_;
	type   type_;
	int    descriptor_;
};

} // comm namespace
} // sadfs namespace

#endif // SADFS_SOCKET_HPP
