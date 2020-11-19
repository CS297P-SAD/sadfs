#ifndef SADFS_COMM_MESSAGES_HPP
#define SADFS_COMM_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/comm/socket.hpp>
#include <sadfs/proto/messages.pb.h>

// standard includes
#include <cstddef> // std::size_t
#include <string>

namespace sadfs { namespace comm {

struct file_section
{
	std::string const filename;
	std::size_t const offset;
	std::size_t const length;
};

enum class request_type {
	read,
	write
};

class file_request
{
public:
	// constructor
	file_request() = default;
	file_request(std::size_t sender, request_type req_type,
	             file_section section);

	bool send(socket const&) const noexcept;
	bool recv(socket const&) noexcept;

	std::size_t  sender()  const noexcept;
	file_section section() const noexcept;
	request_type type()    const noexcept;
private:
	proto::file_request protobuf_;
};

class chunk_request
{
public:
	// constructors
	chunk_request() = default;
	chunk_request(std::size_t sender, request_type req_type,
	              std::size_t chunk_id);

	bool send(socket const&) const noexcept;
	bool recv(socket const&) noexcept;

	std::size_t  sender()   const noexcept;
	std::size_t  chunk_id() const noexcept;
	request_type type()     const noexcept;
private:
	proto::chunk_request protobuf_;
};

} // comm namespace
} // sadfs namespace

#endif // SADFS_COMM_MESSAGES_HPP
