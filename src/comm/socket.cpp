/* implementation of sadfs::comm::socket */

// sadfs-specific includes
#include <sadfs/comm/socket.hpp>
#include <sadfs/logger.hpp>

// standard includes
#include <cerrno>       // errno
#include <cstring>      // std::strerror
#include <stdexcept>    // std::invalid_argument
#include <sys/socket.h> // ::socket
#include <unistd.h>     // ::close

namespace sadfs { namespace comm {
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
socket(domain const domain, type const type) noexcept
	: domain_{domain}, type_{type}
{
	// let the OS choose protocol by passing 0
	descriptor_ = ::socket(lookup(domain), lookup(type), /* protocol */ 0);
	if (descriptor_ == -1)
	{
		logger::error(std::strerror(errno));
	}
}

socket::
socket(domain const domain, type const type, int const descriptor) noexcept
	: domain_{domain}, type_{type}, descriptor_{descriptor}
{
	// TODO: verify parameters
}

socket::
socket(socket&& other) noexcept
	: domain_{other.domain_}, type_{other.type_},
	  descriptor_{other.descriptor_}
{
	// prevent other's destructor from closing the descriptor
	other.descriptor_ = -1;
}

socket& socket::
operator=(socket&& other) noexcept
{
	// move this into temporary that will be destroyed
	auto tmp = socket{std::move(*this)};
	domain_     = other.domain_;
	type_       = other.type_;
	descriptor_ = other.descriptor_;

	// prevent other's destructor from closing the descriptor
	other.descriptor_ = -1;

	return *this;
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

} // comm namespace
} // sadfs namespace
