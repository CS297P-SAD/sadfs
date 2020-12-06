#ifndef SADFS_MSGS_CHUNK_MESSAGES_HPP
#define SADFS_MSGS_CHUNK_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/chunk.pb.h>

// standard includes
#include <cstddef> // std::size_t
#include <string>

namespace sadfs { namespace msgs { namespace chunk {
using message_container = proto::chunk::message_container;

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
	friend bool embed(chunk_request const&, message_container&);
	friend bool extract(chunk_request&, message_container const&);
};

// declarations
bool embed(chunk_request const&, message_container&);
bool extract(chunk_request&, message_container const&);

// ==================================================================
//                      inline function definitions
// ==================================================================
inline std::size_t chunk_request::
chunk_id() const
{
	return protobuf_.chunk_id();
}

} // chunk namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_CHUNK_MESSAGES_HPP
