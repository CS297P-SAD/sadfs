/* implementation of sadfs::socket */

// sadfs-specific includes
#include <sadfs/socket.hpp>

// standard includes
#include <sys/socket.h>
#include <unistd.h>

namespace sadfs {
namespace {

int
lookup(socket::domain domain)
{
	int val{-1};
	switch (domain)
	{
		case socket::domain::local:
			val = AF_UNIX;
			break;
		case socket::domain::inet:
			val = AF_INET;
			break;
		default:
			// TODO: throw exception
			break;
	}
	return val;
}

int
lookup(socket::type type)
{
	int val{-1};
	switch (type)
	{
		case socket::type::stream:
			val = SOCK_STREAM;
			break;
		case socket::type::datagram:
			val = SOCK_DGRAM;
			break;
		default:
			// TODO: throw exception
			break;
	}
	return val;
}

} // unnamed namespace

socket::
socket(domain domain, type type)
{
	descriptor_ = ::socket(lookup(domain), lookup(type), 0);
	if (descriptor_ == -1)
	{
		// TODO: throw exception
	}

	domain_ = domain;
	type_ = type;
}

socket::
socket(domain domain, type type, int descriptor)
{
	// TODO: verify parameters
	domain_     = domain;
	type_       = type;
	descriptor_ = descriptor;
}

socket::
socket(socket&& other)
{
	domain_     = other.domain_;
	type_       = other.type_;
	descriptor_ = other.descriptor_;

	// prevent other's destructor from closing the descriptor
	other.descriptor_ = -1;
}

socket::
~socket() noexcept
{
	if (descriptor_ != -1)
	{
		::close(descriptor_);
	}
}

socket::domain socket::
comm_domain() const noexcept
{
	return domain_;
}

socket::type socket::
socket_type() const noexcept
{
	return type_;
}

int socket::
descriptor() const noexcept
{
	return descriptor_;
}

} // sadfs
