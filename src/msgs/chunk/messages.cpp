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
    {MsgCase::kStreamReq, msg_type::stream_request},
};

} // unnamed namespace

/* ========================================================
 *                       stream_request
 * ========================================================
 */
stream_request::stream_request(msgs::io_type type, chunkid chunk_id,
                               uint32_t offset, uint32_t length,
                               std::string&& data)
{
    protobuf_.set_type(proto_io_type_lookup.at(type));
    chunk_id.serialize(std::back_inserter(*protobuf_.mutable_chunk_id()));
    protobuf_.set_offset(offset);
    protobuf_.set_length(length);
    // TODO: see if the statements below can be replaced with:
    // protobuf_.set_allocated_data(new std::string{std::move(data)});
    auto data_ptr = std::make_unique<std::string>(std::move(data));
    protobuf_.set_allocated_data(data_ptr.release());
}

io_type
stream_request::io_type() const
{
    return io_type_lookup.at(protobuf_.type());
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(stream_request const& req, message_container& container)
{
    // should this be in a try-catch block?
    // container.mutable_stream_req() can throw if heap allocation fails
    *container.mutable_stream_req() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(stream_request& req, message_container& container)
{
    if (msg_type_lookup.at(container.msg_case()) != stream_request::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // pull stream_request out of message_container
    // we do not make a copy since copying the data will be expensive
    req.protobuf_ = std::move(*container.release_stream_req());
    return true;
}

} // namespace chunk

/* ========================================================
 *                  chunk::acknowledgement
 *
 * this is defined in the msgs namespace since
 * acknowledgement really is defined in that namespace
 * ========================================================
 */
// embeds a control message into a container that is
// (typically) sent over the wire
template <>
bool
embed(chunk::acknowledgement const& ack, chunk::message_container& container)
{
    // should this be in a try-catch block?
    // container.mutable_ack() can throw if heap allocation fails
    *container.mutable_ack() = ack.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
template <>
bool
extract(chunk::acknowledgement& ack, chunk::message_container const& container)
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
