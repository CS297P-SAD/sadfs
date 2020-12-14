#ifndef SADFS_MSGS_CHUNK_MESSAGES_HPP
#define SADFS_MSGS_CHUNK_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/chunk.pb.h>
#include <sadfs/types.hpp>

// standard includes
#include <cstddef> // uint32_t
#include <string>
#include <vector>

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
    stream,
    append_request,
    append_forward_request,
};

// chunk::acknowledgement
using acknowledgement = msgs::acknowledgement<message_container, msg_type,
                                              msg_type::acknowledgement>;

// chunk::stream
using stream = msgs::stream<message_container, msg_type, msg_type::stream>;

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

class append_request
{
public:
    // constructors
    append_request() = default;
    append_request(chunkid chunk_id, uint32_t length,
                   std::vector<comm::service> const& replicas,
                   std::string const&                filename);

    chunkid            chunk_id() const;
    uint32_t           length() const;
    comm::service      replicas(int const) const;
    int                replicas_size() const;
    std::string const& filename() const;

    inline static msg_type type{msg_type::append_request};

private:
    proto::chunk::append_request protobuf_{};

    // provide embed/extract functions access to private members
    friend bool embed(append_request const&, message_container&);
    friend bool extract(append_request&, message_container const&);
};

class append_forward_request
{
public:
    // constructors
    append_forward_request() = default;
    append_forward_request(chunkid chunk_id, uint32_t length,
                           std::vector<comm::service> const& replicas,
                           std::string const&                filename);

    chunkid            chunk_id() const;
    uint32_t           length() const;
    comm::service      replicas(int const) const;
    int                replicas_size() const;
    std::string const& filename() const;

    inline static msg_type type{msg_type::append_forward_request};

private:
    proto::chunk::append_forward_request protobuf_{};

    // provide embed/extract functions access to private members
    friend bool embed(append_forward_request const&, message_container&);
    friend bool extract(append_forward_request&, message_container const&);
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

inline chunkid
append_request::chunk_id() const
{
    auto id = chunkid{};
    id.deserialize(protobuf_.chunk_id().data());
    return id;
}

inline uint32_t
append_request::length() const
{
    return protobuf_.length();
}

inline int
append_request::replicas_size() const
{
    return protobuf_.replicas_size();
}

inline comm::service
append_request::replicas(int const i) const
{
    auto replica = protobuf_.replicas(i);
    return {replica.ip().c_str(), replica.port()};
}

inline std::string const&
append_request::filename() const
{
    return protobuf_.filename();
}

inline chunkid
append_forward_request::chunk_id() const
{
    auto id = chunkid{};
    id.deserialize(protobuf_.chunk_id().data());
    return id;
}

inline uint32_t
append_forward_request::length() const
{
    return protobuf_.length();
}

inline int
append_forward_request::replicas_size() const
{
    return protobuf_.replicas_size();
}

inline comm::service
append_forward_request::replicas(int const i) const
{
    auto replica = protobuf_.replicas(i);
    return {replica.ip().c_str(), replica.port()};
}

inline std::string const&
append_forward_request::filename() const
{
    return protobuf_.filename();
}

} // namespace chunk
} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_CHUNK_MESSAGES_HPP
