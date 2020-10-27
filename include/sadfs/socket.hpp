#ifndef SADFS_SOCKET_HPP
#define SADFS_SOCKET_HPP

namespace sadfs {

class socket
{
public:
	enum class domain { local, inet };
	enum class type { stream, datagram };

	// creates a new socket
	socket(domain, type);

	// creates a representation of an existing socket
	// does not create a new socket
	socket(domain, type, int);

	socket(socket&&);
	socket(socket const&) = delete;
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

} // sadfs

#endif // SADFS_SOCKET_HPP
