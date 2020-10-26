#ifndef SADFS_SOCKET_HPP
#define SADFS_SOCKET_HPP

namespace sadfs {

class socket
{
public:
	enum class domain_type { local, inet };
	enum class socket_type { conn_based, conn_less };

	// creates a new socket
	socket(domain_type, socket_type);

	// creates a representation of an existing socket
	// does not create a new socket
	socket(domain_type, socket_type, int);

	socket(socket&&);
	socket(socket const&) = delete;
	~socket() noexcept;

	// accessors
	domain_type domain()     const noexcept;
	socket_type type()       const noexcept;
	int         descriptor() const noexcept;

private:
	domain_type domain_;
	socket_type type_;
	int         descriptor_;
};

} // sadfs

#endif // SADFS_SOCKET_HPP
