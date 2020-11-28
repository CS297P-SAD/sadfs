#ifndef SADFS_MSGS_MESSAGES_HPP
#define SADFS_MSGS_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/proto/chunk.pb.h>
#include <sadfs/proto/master.pb.h>

// standard includes
#include <cstddef> // std::size_t
#include <string>

namespace sadfs { namespace msgs {

enum class io_type
{
	unknown,
	read,
	write,
};

// uniquely identifies a host
struct host_id
{
	std::size_t uuid;
};

namespace master {
using control_message = proto::master::control_message;

// enumerates types of raw messages
enum class msg_type
{
	unknown,
	file_request,
};

// specifies a section of a file
struct file_section
{
	std::string const filename;
	std::size_t const offset;
	std::size_t const length;
};

class file_request
{
public:
	file_request() = default;
	file_request(io_type, file_section const&);

	io_type      type()    const;
	file_section section() const;
	
	inline static msg_type msg_type{msg_type::file_request};
private:
	proto::master::file_request protobuf_{};

	// provide embed/extract functions access to private members
	friend bool embed(file_request const&, control_message&);
	friend bool extract(file_request&, control_message const&);
};

// declarations
bool embed(file_request const&, control_message&);
bool extract(file_request&, control_message const&);

} // master namespace

namespace chunk {
using control_message = proto::chunk::control_message;

enum class msg_type
{
	chunk_request,
	unknown,
};

class chunk_request
{
public:
	// constructors
	chunk_request() = default;
	chunk_request(io_type, std::size_t);

	io_type      type()     const;
	std::size_t  chunk_id() const;
	bool         is_set()  const noexcept;

	inline static msg_type msg_type{msg_type::chunk_request};
private:
	proto::chunk::chunk_request protobuf_{};

	// provide embed/extract functions access to private members
	friend bool embed(chunk_request const&, control_message&);
	friend bool extract(chunk_request&, control_message const&);
};

// declarations
bool embed(chunk_request const&, control_message&);
bool extract(chunk_request&, control_message const&);

} // chunk namespace

} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_MESSAGES_HPP
