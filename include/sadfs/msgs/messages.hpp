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
	chunk_location_request,
};

class chunk_location_request
{
public:
	chunk_location_request() = default;
	chunk_location_request(msgs::io_type, std::string const& filename,
	                       std::size_t chunk_number);

	msgs::io_type      io_type()     const;
	std::string const& filename()    const;
	std::size_t        chunk_number() const;
	
	inline static msg_type type{msg_type::chunk_location_request};
private:
	proto::master::chunk_location_request protobuf_{};

	// provide embed/extract functions access to private members
	friend bool embed(chunk_location_request const&, control_message&);
	friend bool extract(chunk_location_request&, control_message const&);
};

// declarations
bool embed(chunk_location_request const&, control_message&);
bool extract(chunk_location_request&, control_message const&);

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
	chunk_request(msgs::io_type, std::size_t);

	msgs::io_type io_type()  const;
	std::size_t   chunk_id() const;

	inline static msg_type type{msg_type::chunk_request};
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

// ==================================================================
//                     small function definitions
// ==================================================================
namespace master {

inline std::string const& chunk_location_request::
filename() const
{
	return protobuf_.filename();
}

inline std::size_t chunk_location_request::
chunk_number() const
{
	return protobuf_.chunk_number();
}

} // master namespace

} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_MESSAGES_HPP
