/* defines control messages intended for clients */
// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/client/messages.hpp>

// standard includes
#include <iterator> // std::back_inserter

namespace sadfs
{
namespace msgs
{
namespace client
{

using proto::client::message_container;
using MsgCase      = message_container::MsgCase;
using msg_type_map = std::unordered_map<MsgCase, msg_type>;
namespace
{

auto const msg_type_lookup = msg_type_map{
    {MsgCase::MSG_NOT_SET, msg_type::unknown},
    {MsgCase::kAck, msg_type::acknowledgement},
    {MsgCase::kChunkLocationRes, msg_type::chunk_location_response},
    {MsgCase::kFileMetadataRes, msg_type::file_metadata_response},
    {MsgCase::kReadRes, msg_type::read_response},
};

} // unnamed namespace

// ==================================================================
//                     file_metadata_response
// ==================================================================
file_metadata_response::file_metadata_response(bool ok, uint32_t size)
{
    protobuf_.set_ok(ok);
    protobuf_.set_size(size);
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(file_metadata_response const& res, message_container& container)
{
    // should this be in a try-catch block?
    // container.mutable_chunk_location_res() can throw if heap allocation
    // fails
    *container.mutable_file_metadata_res() = res.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(file_metadata_response& res, message_container const& container)
{
    if (msg_type_lookup.at(container.msg_case()) !=
        file_metadata_response::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // read file_metadata_response from message_container
    res.protobuf_ = container.file_metadata_res();
    return true;
}

// ==================================================================
//                     chunk_location_response
// ==================================================================
chunk_location_response::chunk_location_response(
    bool ok, std::vector<comm::service> const& services, chunkid chunk_id,
    version version_num)
{
    protobuf_.set_ok(ok);
    for (auto service : services)
    {
        protobuf_.add_server_ips(to_string(service.ip()));
        protobuf_.add_ports(to_int(service.port()));
    }
    chunk_id.serialize(std::back_inserter(*protobuf_.mutable_chunk_id()));
    protobuf_.set_version_num(version_num);
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(chunk_location_response const& res, message_container& container)
{
    // should this be in a try-catch block?
    // container.mutable_chunk_location_res() can throw if heap allocation
    // fails
    *container.mutable_chunk_location_res() = res.protobuf_;
    return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(chunk_location_response& res, message_container const& container)
{
    if (msg_type_lookup.at(container.msg_case()) !=
        chunk_location_response::type)
    {
        // cannot extract a msg that doesn't exist
        return false;
    }

    // read chunk_location_response from message_container
    res.protobuf_ = container.chunk_location_res();
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

} // namespace client

/* ========================================================
 *                  client::acknowledgement
 *
 * this is defined in the msgs namespace since
 * acknowledgement really is defined in that namespace
 * ========================================================
 */
// embeds a control message into a container that is
// (typically) sent over the wire
template <>
bool
embed(client::acknowledgement const& ack, client::message_container& container)
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
extract(client::acknowledgement&         ack,
        client::message_container const& container)
{
    if (client::msg_type_lookup.at(container.msg_case()) !=
        client::acknowledgement::type)
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
