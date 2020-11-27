/* implementation of control messages */
// sadfs-specific includes
#include <sadfs/comm/messages.hpp>
#include <sadfs/exceptions.hpp>

// standard includes
#include <unordered_map>

// external includes
#include <google/protobuf/util/delimited_message_util.h>

namespace sadfs { namespace msgs {

namespace gpio = google::protobuf::io;
namespace gputil = google::protobuf::util;
using io_type_map = std::unordered_map<proto::io_type,
                                            io_type>;
using proto_io_type_map = std::unordered_map<io_type,
                                            proto::io_type>;

namespace {
// handy function aliases (pointers)
auto const serialize = gputil::SerializeDelimitedToZeroCopyStream;
auto const deserialize = gputil::ParseDelimitedFromZeroCopyStream;

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

using MsgCase = proto::master::control_message::MsgCase;
using msg_type_map = std::unordered_map<MsgCase, msg_type>;
namespace {

auto const msg_type_lookup = msg_type_map
{
	{MsgCase::MSG_NOT_SET, msg_type::unknown},
	{MsgCase::kFileReq,    msg_type::file_request},
};

} // unnamed namespace

/* ========================================================
 *                      control_message 
 * ========================================================
 */
control_message::
control_message(host_id id)
{
	pb_.mutable_sender()->set_id(id.uuid);
}

msg_type control_message::
type() const noexcept
{
	return msg_type_lookup.at(pb_.msg_case());
}

bool control_message::
send(comm::io::connection const& conn) const noexcept
{
	if (this->type() == msg_type::unknown)
	{
		// nothing to send
		return false;
	}
	return serialize(pb_, conn.ostream());
}

bool control_message::
recv(comm::io::connection const& conn) noexcept
{
	return deserialize(&pb_, conn.istream(), nullptr);
}

/* ========================================================
 *                       file_request
 * ========================================================
 */
file_request::
file_request(io_type type, file_section const& section)
	: pb_ptr_{std::make_unique<protobuf>()}
{
	pb_ptr_->set_type(proto_io_type_lookup.at(type));
	pb_ptr_->set_filename(section.filename);
	pb_ptr_->set_offset(section.offset);
	pb_ptr_->set_length(section.length);
}

inline bool file_request::
is_set() const noexcept
{
	return bool{pb_ptr_};
}

file_section file_request::
section() const
{
	if (is_set())
	{
		return
		{
			.filename = pb_ptr_->filename(),
				.offset   = pb_ptr_->offset(),
				.length   = pb_ptr_->length()
		};
	}
	throw invalid_operation("message unavailable");
}

io_type file_request::
type() const
{
	if (is_set())
	{
		return io_type_lookup.at(pb_ptr_->type());
	}
	throw invalid_operation("message unavailable");
}

// extracts a raw message from the control message that is
// (typically) received over the wire
template <>
bool
extract<file_request>(file_request& raw_msg, control_message& ctl_msg)
{
	if (ctl_msg.type() != file_request::msg_type)
	{
		// cannot extract a msg that doesn't exist
		return false;
	}
	// ctl_msg currently owns the protobuf message
	// transfer the ownership to raw_msg
	raw_msg.pb_ptr_.reset(ctl_msg.pb_.release_file_req());
	return true;
}

// embeds a raw message into a control message that is
// (typically) sent over the wire
template <>
bool
embed<file_request>(file_request& raw_msg, control_message& ctl_msg)
{
	if (!raw_msg.is_set())
	{
		// cannot embed a msg that hasn't been set
		return false;
	}
	// raw_msg currently own the protobuf message
	// transfer the ownership to ctl_msg
	ctl_msg.pb_.set_allocated_file_req(raw_msg.pb_ptr_.release());
	return true;
}

} // master namespace

// ========================================================
//                      chunk namespace
// ========================================================
namespace chunk {

using MsgCase = proto::chunk::control_message::MsgCase;
using msg_type_map = std::unordered_map<MsgCase, msg_type>;
namespace {

auto const msg_type_lookup = msg_type_map
{
	{MsgCase::MSG_NOT_SET, msg_type::unknown},
	{MsgCase::kChunkReq,   msg_type::chunk_request},
};

} // unnamed namespace

/* ========================================================
 *                      control_message 
 * ========================================================
 */
control_message::
control_message(host_id id)
{
	pb_.mutable_sender()->set_id(id.uuid);
}

msg_type control_message::
type() const noexcept
{
	return msg_type_lookup.at(pb_.msg_case());
}

bool control_message::
send(comm::io::connection const& conn) const noexcept
{
	if (this->type() == msg_type::unknown)
	{
		// nothing to send
		return false;
	}
	return serialize(pb_, conn.ostream());
}

bool control_message::
recv(comm::io::connection const& conn) noexcept
{
	return deserialize(&pb_, conn.istream(), nullptr);
}

/* ========================================================
 *                       chunk_request
 * ========================================================
 */
chunk_request::
chunk_request(io_type type, std::size_t chunk_id)
	: pb_ptr_{std::make_unique<protobuf>()}
{
	pb_ptr_->set_type(proto_io_type_lookup.at(type));
	pb_ptr_->set_chunk_id(chunk_id);
}

inline bool chunk_request::
is_set() const noexcept
{
	return bool{pb_ptr_};
}

std::size_t chunk_request::
chunk_id() const
{
	if (pb_ptr_)
	{
		return pb_ptr_->chunk_id();
	}
	throw invalid_operation("message unavailable");
}

io_type chunk_request::
type() const
{
	if (pb_ptr_)
	{
		return io_type_lookup.at(pb_ptr_->type());
	}
	throw invalid_operation("message unavailable");
}

// extracts a raw message from the control message that is
// (typically) received over the wire
template <>
bool
extract<chunk_request>(chunk_request& raw_msg, control_message& ctl_msg)
{
	if (ctl_msg.type() != chunk_request::msg_type)
	{
		// cannot extract a msg that doesn't exist
		return false;
	}
	// ctl_msg currently owns the protobuf message
	// transfer the ownership to raw_msg
	raw_msg.pb_ptr_.reset(ctl_msg.pb_.release_chunk_req());
	return true;
}

// embeds a raw message into a control message that is
// (typically) sent over the wire
template <>
bool
embed<chunk_request>(chunk_request& raw_msg, control_message& ctl_msg)
{
	if (!raw_msg.is_set())
	{
		// cannot embed a msg that hasn't been set
		return false;
	}
	// raw_msg currently own the protobuf message
	// transfer the ownership to ctl_msg
	ctl_msg.pb_.set_allocated_chunk_req(raw_msg.pb_ptr_.release());
	return true;
}

} // chunk namespace
} // namespace msgs
} // namespace sadfs
