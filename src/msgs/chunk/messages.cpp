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
    {MsgCase::kAppendReq, msg_type::append_request},
    {MsgCase::kAppendForwardReq, msg_type::append_forward_request},
    {MsgCase::kReadReq, msg_type::read_request},
    {MsgCase::kStream, msg_type::stream},
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
 *                       append_request
 * ========================================================
 */
append_request::append_request(chunkid chunk_id, uint32_t length,
                               std::vector<comm::service> const& replicas,
                               std::string const&                filename)
{
    chunk_id.serialize(std::back_inserter(*protobuf_.mutable_chunk_id()));
    protobuf_.set_length(length);
    for (auto const& replica : replicas)
    {
        auto r = protobuf_.add_replicas();
        r->set_ip(to_string(replica.ip()));
        r->set_port(to_int(replica.port()));
    }
    protobuf_.set_filename(filename);
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(append_request const& req, message_container& container)
{
    // should this be in a try-catch block?
    // container.mutable_append_req() can throw if heap allocation fails
    *container.mutable_append_req() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(append_request& req, message_container const& container)
{
    if (msg_type_lookup.at(container.msg_case()) != append_request::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // copy the request from the container
    req.protobuf_ = container.append_req();
    return true;
}

/* ========================================================
 *                       append_forward_request
 * ========================================================
 */
append_forward_request::append_forward_request(
    chunkid chunk_id, uint32_t length,
    std::vector<comm::service> const& replicas, std::string const& filename)
{
    chunk_id.serialize(std::back_inserter(*protobuf_.mutable_chunk_id()));
    protobuf_.set_length(length);
    for (auto const& replica : replicas)
    {
        auto r = protobuf_.add_replicas();
        r->set_ip(to_string(replica.ip()));
        r->set_port(to_int(replica.port()));
    }
    protobuf_.set_filename(filename);
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(append_forward_request const& req, message_container& container)
{
    // should this be in a try-catch block?
    // container.mutable_append_forward_req() can throw if heap allocation
    // fails
    *container.mutable_append_forward_req() = req.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(append_forward_request& req, message_container const& container)
{
    if (msg_type_lookup.at(container.msg_case()) !=
        append_forward_request::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // copy the request from the container
    req.protobuf_ = container.append_forward_req();
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

/* ========================================================
 *                       chunk::stream
 *
 * these are defined in the msgs namespace since
 * stream really is defined in that namespace
 * ========================================================
 */
// embeds a control message into a container that is
// (typically) sent over the wire
template <>
bool
embed(chunk::stream const& stream, chunk::message_container& container)
{
    // should this be in a try-catch block?
    // container.mutable_stream() can throw if heap allocation fails
    *container.mutable_stream() = stream.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
template <>
bool
extract(chunk::stream& stream, chunk::message_container& container)
{
    if (chunk::msg_type_lookup.at(container.msg_case()) != chunk::stream::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // pull streamed data from message_container
    stream.protobuf_ = *container.release_stream();
    return true;
}

} // namespace msgs
} // namespace sadfs
