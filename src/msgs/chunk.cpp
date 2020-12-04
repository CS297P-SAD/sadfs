/* implementation of control messages intended for chunk servers */
// sadfs-specific includes
#include <sadfs/msgs/chunk_messages.hpp>

// standard includes
#include <unordered_map>

namespace sadfs { namespace msgs { namespace chunk {

using proto::chunk::control_message;
using MsgCase = control_message::MsgCase;
using msg_type_map = std::unordered_map<MsgCase, msg_type>;
namespace {

auto const msg_type_lookup = msg_type_map
{
	{MsgCase::MSG_NOT_SET, msg_type::unknown},
	{MsgCase::kChunkReq,   msg_type::chunk_request},
};

} // unnamed namespace

/* ========================================================
 *                       chunk_request
 * ========================================================
 */
chunk_request::
chunk_request(msgs::io_type type, std::size_t chunk_id)
{
	protobuf_.set_type(proto_io_type_lookup.at(type));
	protobuf_.set_chunk_id(chunk_id);
}

io_type chunk_request::
io_type() const
{
	return io_type_lookup.at(protobuf_.type());
}

// embeds a raw message into a control message that is
// (typically) sent over the wire
bool
embed(chunk_request const& req, control_message& cm)
{
	// should this be in a try-catch block?
	// msg.mutable_chunk_req() can throw if heap allocation fails
	*cm.mutable_chunk_req() = req.protobuf_;
	return true;
}

// extracts a raw message from the control message that is
// (typically) received over the wire
bool
extract(chunk_request& req, control_message const& cm)
{
	if (msg_type_lookup.at(cm.msg_case()) != chunk_request::type)
	{
		// cannot extract a msg that doesn't exist
		return false;
	}

	// read chunk_request from control_message
	req.protobuf_ = cm.chunk_req();
	return true;
}

} // chunk namespace
} // msgs namespace
} // sadfs namespace
