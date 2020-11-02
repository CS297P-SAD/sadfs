/* implementation of sadfs::socket */

// sadfs-specific includes
#include "socket.hpp"

// standard includes
#include <stdexcept>    // std::invalid_argument
#include <system_error> // std::system_error, std::system_category
#include <sys/socket.h> // ::socket
#include <unistd.h>     // ::close

namespace sadfs {
namespace {

int
lookup(socket::domain const domain)
{
	switch (domain)
	{
		case socket::domain::local:
			return AF_UNIX;
		case socket::domain::inet:
			return AF_INET;
	}
	throw std::invalid_argument("unsupported domain");
}

int
lookup(socket::type const type)
{
	switch (type)
	{
		case socket::type::stream:
			return SOCK_STREAM;
		case socket::type::datagram:
			return SOCK_DGRAM;
	}
	throw std::invalid_argument("unsupported socket type");
}

} // unnamed namespace

socket::
socket(domain const domain, type const type)
{
	// let the OS choose protocol by passing 0
	descriptor_ = ::socket(lookup(domain), lookup(type), /* protocol */ 0);
	if (descriptor_ == -1)
	{
		throw std::system_error(errno, std::system_category(),
		                        "socket creation failed");
	}

	domain_ = domain;
	type_ = type;
}

socket::
socket(domain const domain, type const type, int const descriptor) noexcept
	: domain_(domain), type_(type), descriptor_(descriptor)
{
	// TODO: verify parameters
}

socket::
socket(socket&& other) noexcept
	: domain_(other.domain_), type_(other.type_),
	  descriptor_(other.descriptor_)
{
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
