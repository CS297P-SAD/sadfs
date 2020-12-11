#ifndef SADFS_MSGS_CHUNK_MESSAGES_HPP
#define SADFS_MSGS_CHUNK_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/chunk.pb.h>
#include <sadfs/types.hpp>

// standard includes
#include <cstddef> // std::size_t
#include <string>

namespace sadfs
{
namespace msgs
{
namespace chunk
{
using message_container = proto::chunk::message_container;

enum class msg_type
{
    unknown,
    acknowledgement,
    stream_request,
};

// acknowledgement chunk::acknowledgement
using acknowledgement = msgs::acknowledgement<message_container, msg_type,
                                              msg_type::acknowledgement>;

class stream_request
{
public:
    // constructors
    stream_request() = default;
    stream_request(msgs::io_type, chunkid chunk_id, uint64_t offset,
	               uint64_t length, std::string&& data);

    msgs::io_type      io_type()  const;
    chunkid            chunk_id() const;
	uint64_t           offset()   const;
	uint64_t           length()   const;
	std::string const& data()     const;

    inline static msg_type type{msg_type::stream_request};

private:
    proto::chunk::stream_request protobuf_{};

    // provide embed/extract functions access to private members
    friend bool embed(stream_request const&, message_container&);
    friend bool extract(stream_request&, message_container&);
};

// declarations
bool embed(stream_request const&, message_container&);
bool extract(stream_request&, message_container&);

// ==================================================================
//                      inline function definitions
// ==================================================================
inline chunkid
stream_request::chunk_id() const
{
	auto id = chunkid{};
	id.deserialize(protobuf_.chunk_id().data());
	return id;
}

inline uint64_t
stream_request::offset() const
{
	return protobuf_.offset();
}

inline uint64_t           
stream_request::length()   const
{
	return protobuf_.length();
}

inline std::string const&
stream_request::data()     const
{
	return protobuf_.data();
}

} // namespace chunk
} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_CHUNK_MESSAGES_HPP
