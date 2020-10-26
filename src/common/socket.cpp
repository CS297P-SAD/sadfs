/* implementation of sadfs::socket */

// sadfs-specific includes
#include <sadfs/socket.hpp>

// standard includes
#include <sys/socket.h>
#include <unistd.h>

namespace sadfs {
namespace {

int
lookup(socket::domain_type domain)
{
	int val{-1};
	switch (domain)
	{
		case socket::domain_type::local:
			val = AF_UNIX;
			break;
		case socket::domain_type::inet:
			val = AF_INET;
			break;
		default:
			// TODO: throw exception
			break;
	}
	return val;
}

int
lookup(socket::socket_type type)
{
	int val{-1};
	switch (type)
	{
		case socket::socket_type::conn_based:
			val = SOCK_STREAM;
			break;
		case socket::socket_type::conn_less:
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
socket(domain_type domain, socket_type type)
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
socket(domain_type domain, socket_type type, int descriptor)
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

socket::domain_type
socket::domain() const noexcept
{
	return domain_;
}

socket::socket_type
socket::type() const noexcept
{
	return type_;
}

int
socket::descriptor() const noexcept
{
	return descriptor_;
}

} // sadfs
