/* implementation of control messages intended for the master server */
// sadfs-specific includes
#include <sadfs/msgs/master/messages.hpp>

// standard includes
#include <unordered_map>

namespace sadfs
{
namespace msgs
{
namespace master
{

using proto::master::message_container;
using MsgCase      = message_container::MsgCase;
using msg_type_map = std::unordered_map<MsgCase, msg_type>;
namespace
{

auto const msg_type_lookup = msg_type_map{
    {MsgCase::MSG_NOT_SET, msg_type::unknown},
    {MsgCase::kFileInfoReq, msg_type::file_info_request},
    {MsgCase::kCreateFileReq, msg_type::create_file_request},
    {MsgCase::kChunkWriteNotify, msg_type::chunk_write_notification},
    {MsgCase::kChunkLocationReq, msg_type::chunk_location_request},
    {MsgCase::kChunkServerHeartbeat, msg_type::chunk_server_heartbeat},
    {MsgCase::kJoinNetworkReq, msg_type::join_network_request},
};

} // unnamed namespace

/* ========================================================
 *                       file_info_request
 * ========================================================
 */
file_info_request::file_info_request(std::string const &filename)
{
    protobuf_.set_filename(filename);
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(file_info_request const &req, message_container &container)
{
    // should this be in a try-catch block?
    // msg.mutable_file_info_req() can throw if heap allocation fails
    *container.mutable_file_info_req() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(file_info_request &req, message_container const &container)
{
    if (msg_type_lookup.at(container.msg_case()) != file_info_request::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // read file_info_request from message_container
    req.protobuf_ = container.file_info_req();
    return true;
}

/* ========================================================
 *                       create_file_request
 * ========================================================
 */
create_file_request::create_file_request(std::string const &filename)
{
    protobuf_.set_filename(filename);
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(create_file_request const &req, message_container &container)
{
    // should this be in a try-catch block?
    // msg.mutable_create_file_req() can throw if heap allocation fails
    *container.mutable_create_file_req() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(create_file_request &req, message_container const &container)
{
    if (msg_type_lookup.at(container.msg_case()) != create_file_request::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // read create_file_request from message_container
    req.protobuf_ = container.create_file_req();
    return true;
}

/* ========================================================
 *                       chunk_write_notification
 * ========================================================
 */
chunk_write_notification::chunk_write_notification(serverid server_id,
                                                   chunkid  chunk_id,
                                                   uint64_t version,
                                                   std::string const &filename,
                                                   uint32_t           new_size)
{
    server_id.serialize(std::back_inserter(*protobuf_.mutable_server_id()));
    chunk_id.serialize(std::back_inserter(*protobuf_.mutable_chunk_id()));
    protobuf_.set_version(version);
    protobuf_.set_filename(filename);
    protobuf_.set_new_size(new_size);
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(chunk_write_notification const &req, message_container &container)
{
    // should this be in a try-catch block?
    // msg.mutable_chunk_write_notify() can throw if heap allocation fails
    *container.mutable_chunk_write_notify() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(chunk_write_notification &req, message_container const &container)
{
    if (msg_type_lookup.at(container.msg_case()) !=
        chunk_write_notification::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // read chunk_write_notification from message_container
    req.protobuf_ = container.chunk_write_notify();
    return true;
}

/* ========================================================
 *                       chunk_location_request
 * ========================================================
 */
chunk_location_request::chunk_location_request(msgs::io_type      type,
                                               std::string const &filename,
                                               std::size_t        chunk_number)
{
    protobuf_.set_type(proto_io_type_lookup.at(type));
    protobuf_.set_filename(filename);
    protobuf_.set_chunk_number(chunk_number);
}

io_type
chunk_location_request::io_type() const
{
    return io_type_lookup.at(protobuf_.type());
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(chunk_location_request const &req, message_container &container)
{
    // should this be in a try-catch block?
    // msg.mutable_chunk_location_req() can throw if heap allocation fails
    *container.mutable_chunk_location_req() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(chunk_location_request &req, message_container const &container)
{
    if (msg_type_lookup.at(container.msg_case()) !=
        chunk_location_request::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // read chunk_location_request from message_container
    req.protobuf_ = container.chunk_location_req();
    return true;
}

/* ========================================================
 *                       chunk_server_heartbeat
 * ========================================================
 */
// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(chunk_server_heartbeat const &, message_container &container)
{
    // call mutable_chunk_server_heartbeat() so that the oneof field
    // is set to chunk_server_heartbeat, and discard the pointer
    return container.mutable_chunk_server_heartbeat() != nullptr;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(chunk_server_heartbeat &, message_container const &container)
{
    if (msg_type_lookup.at(container.msg_case()) !=
        chunk_server_heartbeat::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // nothing to extract
    return true;
}

/* ========================================================
 *                       join_network_request
 * ========================================================
 */
join_network_request::join_network_request(serverid      server_id,
                                           comm::service service,
                                           uint64_t      max_chunks,
                                           uint64_t      chunk_count)
{
    server_id.serialize(std::back_inserter(*protobuf_.mutable_server_id()));
    protobuf_.set_ip(to_string(service.ip()));
    protobuf_.set_port(to_int(service.port()));
    protobuf_.set_max_chunks(max_chunks);
    protobuf_.set_chunk_count(chunk_count);
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(join_network_request const &req, message_container &container)
{
    // should this be in a try-catch block?
    // msg.mutable_join_network_req() can throw if heap allocation fails
    *container.mutable_join_network_req() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(join_network_request &req, message_container const &container)
{
    if (msg_type_lookup.at(container.msg_case()) != join_network_request::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // read join_network_request from message_container
    req.protobuf_ = container.join_network_req();
    return true;
}
} // namespace master
} // namespace msgs
} // namespace sadfs
