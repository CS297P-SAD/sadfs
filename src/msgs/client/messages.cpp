/* defines control messages intended for clients */
//sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/client/messages.hpp>

// standard includes
#include <iterator>    // std::back_inserter

namespace sadfs { namespace msgs { namespace client {

using proto::client::message_container;
using MsgCase = message_container::MsgCase;
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
chunk_location_response(bool ok, std::vector<comm::service> const& services, 
                        chunkid chunk_id)
{
	protobuf_.set_ok(ok);
	for (auto service : services)
	{
		protobuf_.add_server_ips(to_string(service.ip()));
		protobuf_.add_ports(to_int(service.port()));
	}
	chunk_id.serialize(std::back_inserter(*protobuf_.mutable_chunk_id()));
}

// embeds a control message into a container that is
// (typically) sent over the wire
bool
embed(chunk_location_response const& res, message_container& cm)
{
	// should this be in a try-catch block?
	// msg.mutable_chunk_location_res() can throw if heap allocation fails
	*cm.mutable_chunk_location_res() = res.protobuf_;
	return true;
}

// extracts a control message from a container that is
// (typically) received over the wire
bool
extract(chunk_location_response& res, message_container const& cm)
{
	if (msg_type_lookup.at(cm.msg_case()) != chunk_location_response::type)
	{
		// cannot extract a msg that doesn't exist
		return false;
	}

	// read chunk_location_response from message_container
	res.protobuf_ = cm.chunk_location_res();
	return true;
}


} // client namespace
} // msgs namespace
} // sadfs namespace
