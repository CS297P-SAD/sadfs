/* defines control messages intended for clients */
//sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/client_messages.hpp>

// standard includes
#include <iterator>    // std::back_inserter

namespace sadfs { namespace msgs { namespace client {

using proto::client::control_message;
using MsgCase = control_message::MsgCase;
using msg_type_map = std::unordered_map<MsgCase, msg_type>;
namespace {

auto const msg_type_lookup = msg_type_map
{
	{MsgCase::MSG_NOT_SET,       msg_type::unknown},
	{MsgCase::kChunkLocationRes, msg_type::chunk_location_response},
};

} // unnamed namespace

// ==================================================================
//                     chunk_location_response
// ==================================================================
chunk_location_response::
chunk_location_response(bool ok, comm::service const& service,
                        chunkid chunk_id, std::string const& payload)
{
	protobuf_.set_ok(ok);
	protobuf_.set_server_ip(to_string(service.ip()));
	protobuf_.set_port(to_int(service.port()));
	chunk_id.serialize(std::back_inserter(*protobuf_.mutable_chunk_id()));
	protobuf_.set_payload(payload);
}

// embeds a raw message into control message that is
// (typically) sent over the wire
bool
embed(chunk_location_response const& res, control_message& cm)
{
	// should this be in a try-catch block?
	// msg.mutable_chunk_location_res() can throw if heap allocation fails
	*cm.mutable_chunk_location_res() = res.protobuf_;
	return true;
}

// extracts a raw message from the control message that is
// (typically) received over the wire
bool
extract(chunk_location_response& res, control_message const& cm)
{
	if (msg_type_lookup.at(cm.msg_case()) != chunk_location_response::type)
	{
		// cannot extract a msg that doesn't exist
		return false;
	}

	// read chunk_location_response from control_message
	res.protobuf_ = cm.chunk_location_res();
	return true;
}


} // client namespace
} // msgs namespace
} // sadfs namespace
