#ifndef SADFS_COMM_MESSAGES_HPP
#define SADFS_COMM_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/comm/socket.hpp>
#include <sadfs/proto/messages.pb.h>

// standard includes
#include <cstddef> // std::size_t
#include <string>

#include <google/protobuf/io/zero_copy_stream.h>

namespace sadfs { namespace comm { namespace msgs {
namespace gpio = google::protobuf::io;

// specifies a section of a file
struct file_section
{
	std::string const filename;
	std::size_t const offset;
	std::size_t const length;
};

enum class io_type
{
	read,
	write,
};

enum class host_type
{
	unknown_host_type,
	client,
	chunk_server,
	master_server,
};

enum class msg_type
{
	unknown_msg_type,
	id,
	file,
	chunk,
	metadata,
};

// base class for all protobuf messages
class protobuf_base
{
public:
	virtual bool send(gpio::ZeroCopyOutputStream*) const noexcept = 0;
	virtual bool recv(gpio::ZeroCopyInputStream*) noexcept = 0;
};

class identification final : public protobuf_base
{
public:
	// constructor
	identification() = default;
	identification(host_type type, std::size_t id);

	bool send(gpio::ZeroCopyOutputStream*) const noexcept;
	bool recv(gpio::ZeroCopyInputStream*) noexcept;

	std::size_t id()   const noexcept;
	host_type   type() const noexcept;
private:
	proto::identification protobuf_;
};

class file_request final : public protobuf_base
{
public:
	// constructor
	file_request() = default;
	file_request(std::size_t sender, io_type type,
	             file_section section);

	bool send(gpio::ZeroCopyOutputStream*) const noexcept;
	bool recv(gpio::ZeroCopyInputStream*) noexcept;

	std::size_t  sender()  const noexcept;
	file_section section() const noexcept;
	io_type      type()    const noexcept;
private:
	proto::file_request protobuf_;
};

class chunk_request final : public protobuf_base
{
public:
	// constructors
	chunk_request() = default;
	chunk_request(std::size_t sender, io_type type,
	              std::size_t chunk_id);

	bool send(gpio::ZeroCopyOutputStream*) const noexcept;
	bool recv(gpio::ZeroCopyInputStream*) noexcept;

	std::size_t  sender()   const noexcept;
	std::size_t  chunk_id() const noexcept;
	io_type      type()     const noexcept;
private:
	proto::chunk_request protobuf_;
};

class msg_id final : private protobuf_base
{
public:
	// constructors
	msg_id() = default;

	bool recv(gpio::ZeroCopyInputStream*) noexcept;

	msg_type type() const noexcept;
private:
	// this function must never be called
	bool send(gpio::ZeroCopyOutputStream*) const noexcept;

	proto::msg_id protobuf_;
};

} // msgs namespace
} // comm namespace
} // sadfs namespace

#endif // SADFS_COMM_MESSAGES_HPP
