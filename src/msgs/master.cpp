/* implementation of control messages intended for the master server */
// sadfs-specific includes
#include <sadfs/msgs/master_messages.hpp>

// standard includes
#include <unordered_map>

namespace sadfs { namespace msgs { namespace master {

using proto::master::control_message;
using MsgCase = control_message::MsgCase;
using msg_type_map = std::unordered_map<MsgCase, msg_type>;
namespace {

auto const msg_type_lookup = msg_type_map
{
	{MsgCase::MSG_NOT_SET,       msg_type::unknown},
	{MsgCase::kChunkLocationReq, msg_type::chunk_location_request},
};

} // unnamed namespace

/* ========================================================
 *                       chunk_location_request
 * ========================================================
 */
chunk_location_request::
chunk_location_request(msgs::io_type type,
                       std::string const& filename,
                       std::size_t chunk_number)
{
	protobuf_.set_type(proto_io_type_lookup.at(type));
	protobuf_.set_filename(filename);
	protobuf_.set_chunk_number(chunk_number);
}

io_type chunk_location_request::
io_type() const
{
	return io_type_lookup.at(protobuf_.type());
}

// embeds a raw message into control message that is
// (typically) sent over the wire
bool
embed(chunk_location_request const& req, control_message& cm)
{
	// should this be in a try-catch block?
	// msg.mutable_chunk_location_req() can throw if heap allocation fails
	*cm.mutable_chunk_location_req() = req.protobuf_;
	return true;
}

// extracts a raw message from the control message that is
// (typically) received over the wire
bool
extract(chunk_location_request& req, control_message const& cm)
{
	if (msg_type_lookup.at(cm.msg_case()) != chunk_location_request::type)
	{
		// cannot extract a msg that doesn't exist
		return false;
	}

	// read chunk_location_request from control_message
	req.protobuf_ = cm.chunk_location_req();
	return true;
}

} // master namespace
} // msgs namespace
} // sadfs namespace
