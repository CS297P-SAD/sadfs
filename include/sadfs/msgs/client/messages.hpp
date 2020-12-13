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
    chunk_location_response,
    file_metadata_response,
    read_response,
};

// instantiate client::acknowledgement
using acknowledgement = msgs::acknowledgement<message_container, msg_type,
                                              msg_type::acknowledgement>;

class chunk_location_response
{
public:
    chunk_location_response() = default;
    chunk_location_response(bool                              ok,
                            std::vector<comm::service> const& services,
                            chunkid chunk_id, version version_num,
			    uint32_t file_size);

    bool          ok() const noexcept;
    comm::service service(int) const;
    chunkid       chunk_id() const noexcept;
    uint32_t      version_num() const;
    int           locations_size() const noexcept;
    uint32_t	  file_size() const noexcept;

    inline static msg_type type{msg_type::chunk_location_response};

private:
    proto::client::chunk_location_response protobuf_{};

    friend bool embed(chunk_location_response const&, message_container&);
    friend bool extract(chunk_location_response&, message_container const&);
};

class file_metadata_response
{
public:
    file_metadata_response() = default;
    file_metadata_response(bool ok, uint32_t size);

    bool     ok() const noexcept;
    uint32_t size() const noexcept;

    inline static msg_type type{msg_type::file_metadata_response};

private:
    proto::client::file_metadata_response protobuf_{};

    friend bool embed(file_metadata_response const&, message_container&);
    friend bool extract(file_metadata_response&, message_container const&);
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
    proto::client::read_response protobuf_{};

    // provide embed/extract functions access to private members
    friend bool embed(read_response const&, message_container&);
    friend bool extract(read_response&, message_container&);
};

// ==================================================================
//                     inline function definitions
// ==================================================================
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

inline uint32_t
chunk_location_response::file_size() const noexcept
{
    return protobuf_.file_size();
}

inline bool
file_metadata_response::ok() const noexcept
{
    return protobuf_.ok();
}

inline uint32_t
file_metadata_response::size() const noexcept
{
    return protobuf_.size();
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

} // namespace client
} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_CLIENT_MESSAGES_HPP
