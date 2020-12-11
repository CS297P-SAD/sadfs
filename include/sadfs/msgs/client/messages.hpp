#ifndef SADFS_MSGS_CLIENT_MESSAGES_HPP
#define SADFS_MSGS_CLIENT_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/client.pb.h>
#include <sadfs/types.hpp>

namespace sadfs
{
namespace msgs
{
namespace client
{
using proto::client::message_container;
using version = unsigned int;

// enumerates types of control messages sent to clients
enum class msg_type
{
    unknown,
    acknowledgement,
    file_info_response,
    chunk_location_response,
};

class file_info_response
{
public:
    file_info_response() = default;
    file_info_response(bool exists, uint64_t size);

    bool     exists() const noexcept;
    uint64_t size() const;

    inline static msg_type type{msg_type::file_info_response};

private:
    proto::client::file_info_response protobuf_{};

    friend bool embed(file_info_response const &, message_container &);
    friend bool extract(file_info_response &, message_container const &);
};

// declarations
bool embed(file_info_response const &, message_container &);
bool extract(file_info_response &, message_container const &);

// instantiate client::acknowledgement
using acknowledgement = msgs::acknowledgement<message_container, msg_type,
                                              msg_type::acknowledgement>;

class chunk_location_response
{
public:
    chunk_location_response() = default;
    chunk_location_response(bool                              ok,
                            std::vector<comm::service> const& services,
                            chunkid chunk_id, version version_num);

    bool          ok() const noexcept;
    comm::service service(int) const;
    chunkid       chunk_id() const noexcept;
    uint32_t      version_num() const;
    int           locations_size() const noexcept;

    inline static msg_type type{msg_type::chunk_location_response};

private:
    proto::client::chunk_location_response protobuf_{};

    friend bool embed(chunk_location_response const&, message_container&);
    friend bool extract(chunk_location_response&, message_container const&);
};

// declarations
bool embed(chunk_location_response const &, message_container &);
bool extract(chunk_location_response &, message_container const &);

// ==================================================================
//                     inline function definitions
// ==================================================================
inline bool
file_info_response::exists() const noexcept
{
    return protobuf_.exists();
}

inline uint64_t
file_info_response::size() const
{
    return protobuf_.size();
}

inline bool
chunk_location_response::ok() const noexcept
{
    return protobuf_.ok();
}

inline comm::service
chunk_location_response::service(int i) const
{
    return {protobuf_.server_ips(i).c_str(), protobuf_.ports(i)};
}

inline chunkid
chunk_location_response::chunk_id() const noexcept
{
    auto id = chunkid{};
    id.deserialize(protobuf_.chunk_id().data());
    return id;
}

inline int
chunk_location_response::locations_size() const noexcept
{
    return protobuf_.server_ips_size();
}

inline uint32_t
chunk_location_response::version_num() const
{
    return protobuf_.version_num();
}

} // namespace client
} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_CLIENT_MESSAGES_HPP
