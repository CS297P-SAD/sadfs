/* implementation of control messages intended for chunk servers */
// sadfs-specific includes
#include <sadfs/msgs/chunk/messages.hpp>

// standard includes
#include <unordered_map>

namespace sadfs
{
namespace msgs
{
namespace chunk
{

using proto::chunk::message_container;
using MsgCase      = message_container::MsgCase;
using msg_type_map = std::unordered_map<MsgCase, msg_type>;
namespace
{

auto const msg_type_lookup = msg_type_map{
    {MsgCase::MSG_NOT_SET, msg_type::unknown},
    {MsgCase::kAck, msg_type::acknowledgement},
    {MsgCase::kChunkReq, msg_type::chunk_request},
};

} // unnamed namespace

/* ========================================================
 *                       chunk_request
 * ========================================================
 */
chunk_request::chunk_request(msgs::io_type type, std::size_t chunk_id)
{
    protobuf_.set_type(proto_io_type_lookup.at(type));
    protobuf_.set_chunk_id(chunk_id);
}

io_type
chunk_request::io_type() const
{
    return io_type_lookup.at(protobuf_.type());
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(chunk_request const &req, message_container &container)
{
    // should this be in a try-catch block?
    // msg.mutable_chunk_req() can throw if heap allocation fails
    *container.mutable_chunk_req() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(chunk_request &req, message_container const &container)
{
    if (msg_type_lookup.at(container.msg_case()) != chunk_request::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // read chunk_request from message_container
    req.protobuf_ = container.chunk_req();
    return true;
}

} // namespace chunk

/* ========================================================
 *                       chunk_request
 * ========================================================
 */
// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(chunk::acknowledgement const &ack, chunk::message_container &container)
{
    // should this be in a try-catch block?
    // msg.mutable_ack() can throw if heap allocation fails
    *container.mutable_ack() = ack.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(chunk::acknowledgement &ack, chunk::message_container const &container)
{
    if (chunk::msg_type_lookup.at(container.msg_case()) !=
        chunk::acknowledgement::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // read acknowledgement from message_container
    ack.protobuf_ = container.ack();
    return true;
}
} // namespace msgs
} // namespace sadfs
