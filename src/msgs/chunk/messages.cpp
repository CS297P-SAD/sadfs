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
    {MsgCase::kReadReq, msg_type::read_request},
    {MsgCase::kReadRes, msg_type::read_response},
};

} // unnamed namespace

/* ========================================================
 *                       read_request
 * ========================================================
 */
read_request::read_request(chunkid chunk_id, uint32_t offset, uint32_t length)
{
    chunk_id.serialize(std::back_inserter(*protobuf_.mutable_chunk_id()));
    protobuf_.set_offset(offset);
    protobuf_.set_length(length);
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(read_request const& req, message_container& container)
{
    // should this be in a try-catch block?
    // container.mutable_read_req() can throw if heap allocation fails
    *container.mutable_read_req() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(read_request& req, message_container const& container)
{
    if (msg_type_lookup.at(container.msg_case()) != read_request::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // copy the request from the container
    req.protobuf_ = container.read_req();
    return true;
}

/* ========================================================
 *                       read_response
 * ========================================================
 */
read_response::read_response(bool ok, std::string&& data)
{
    protobuf_.set_ok(ok);
    if (ok)
    {
        // TODO: see if the statements below can be replaced with:
        // protobuf_.set_allocated_data(new std::string{std::move(data)});
        auto data_ptr = std::make_unique<std::string>(std::move(data));
        protobuf_.set_allocated_data(data_ptr.release());
    }
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(read_response const& res, message_container& container)
{
    // should this be in a try-catch block?
    // container.mutable_read_res() can throw if heap allocation fails
    *container.mutable_read_res() = res.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(read_response& res, message_container& container)
{
    if (msg_type_lookup.at(container.msg_case()) != read_response::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // copy the response from the container
    res.protobuf_ = container.read_res();
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
