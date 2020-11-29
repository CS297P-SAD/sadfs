/* implementation of control messages */
// sadfs-specific includes
#include <sadfs/msgs/messages.hpp>

// standard includes
#include <unordered_map>

namespace sadfs { namespace msgs {

using io_type_map = std::unordered_map<proto::io_type,
                                            io_type>;
using proto_io_type_map = std::unordered_map<io_type,
                                            proto::io_type>;
namespace {

auto const proto_io_type_lookup = proto_io_type_map
{
	{io_type::unknown, proto::io_type::UNKNOWN},
	{io_type::read, proto::io_type::READ},
	{io_type::write, proto::io_type::WRITE},
};

auto const io_type_lookup = io_type_map
{
	{proto::io_type::UNKNOWN, io_type::unknown},
	{proto::io_type::READ, io_type::read},
	{proto::io_type::WRITE, io_type::write},
};

} // unnamed namespace

// ========================================================
//                      master namespace
// ========================================================
namespace master {

using proto::master::control_message;
using MsgCase = control_message::MsgCase;
using msg_type_map = std::unordered_map<MsgCase, msg_type>;
namespace {

auto const msg_type_lookup = msg_type_map
{
	{MsgCase::MSG_NOT_SET, msg_type::unknown},
	{MsgCase::kFileReq,    msg_type::file_request},
};

} // unnamed namespace

/* ========================================================
 *                       file_request
 * ========================================================
 */
file_request::
file_request(msgs::io_type type, file_section const& section)
{
	protobuf_.set_type(proto_io_type_lookup.at(type));
	protobuf_.set_filename(section.filename);
	protobuf_.set_offset(section.offset);
	protobuf_.set_length(section.length);
}

file_section file_request::
section() const
{
	return
	{
		.filename = protobuf_.filename(),
		.offset   = protobuf_.offset(),
		.length   = protobuf_.length()
	};
}

io_type file_request::
io_type() const
{
	return io_type_lookup.at(protobuf_.type());
}

// embeds a raw message into control message that is
// (typically) sent over the wire
bool
embed(file_request const& req, control_message& cm)
{
	// should this be in a try-catch block?
	// msg.mutable_file_req() can throw if heap allocation fails
	*cm.mutable_file_req() = req.protobuf_;
	return true;
}

// extracts a raw message from the control message that is
// (typically) received over the wire
bool
extract(file_request& req, control_message const& cm)
{
	if (msg_type_lookup.at(cm.msg_case()) != file_request::type)
	{
		// cannot extract a msg that doesn't exist
		return false;
	}

	// read file_request from control_message
	req.protobuf_ = cm.file_req();
	return true;
}

} // master namespace

// ========================================================
//                      chunk namespace
// ========================================================
namespace chunk {

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

std::size_t chunk_request::
chunk_id() const
{
	return protobuf_.chunk_id();
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
	// msg.mutable_file_req() can throw if heap allocation fails
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

	// read file_request from control_message
	req.protobuf_ = cm.chunk_req();
	return true;
}

} // chunk namespace

} // namespace msgs
} // namespace sadfs
