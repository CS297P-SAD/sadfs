#ifndef SADFS_MSGS_CHUNK_MESSAGES_HPP
#define SADFS_MSGS_CHUNK_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/chunk.pb.h>
#include <sadfs/types.hpp>

// standard includes
#include <cstddef> // uint32_t
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
    read_request,
    read_response,
};

// acknowledgement chunk::acknowledgement
using acknowledgement = msgs::acknowledgement<message_container, msg_type,
                                              msg_type::acknowledgement>;

class read_request
{
public:
    // constructors
    read_request() = default;
    read_request(chunkid chunk_id, uint32_t offset, uint32_t length);

    chunkid  chunk_id() const;
    uint32_t offset() const;
    uint32_t length() const;

    inline static msg_type type{msg_type::read_request};

private:
    proto::chunk::read_request protobuf_{};

    // provide embed/extract functions access to private members
    friend bool embed(read_request const&, message_container&);
    friend bool extract(read_request&, message_container const&);
};

class read_response
{
public:
    // constructors
    read_response() = default;
    read_response(bool ok, std::string&& data);

    bool               ok() const;
    std::string const& data() const;

    inline static msg_type type{msg_type::read_response};

private:
    proto::chunk::read_response protobuf_{};

    // provide embed/extract functions access to private members
    friend bool embed(read_response const&, message_container&);
    friend bool extract(read_response&, message_container&);
};

// ==================================================================
//                      inline function definitions
// ==================================================================
inline chunkid
read_request::chunk_id() const
{
    auto id = chunkid{};
    id.deserialize(protobuf_.chunk_id().data());
    return id;
}

inline uint32_t
read_request::offset() const
{
    return protobuf_.offset();
}

inline uint32_t
read_request::length() const
{
    return protobuf_.length();
}

inline bool
read_response::ok() const
{
    return protobuf_.ok();
}

inline std::string const&
read_response::data() const
{
    return protobuf_.data();
}

} // namespace chunk
} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_CHUNK_MESSAGES_HPP
