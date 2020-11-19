/* implementation of control messages */
// sadfs-specific includes
#include <sadfs/comm/messages.hpp>

// standard includes
#include <unordered_map>

// external includes
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/util/delimited_message_util.h>

namespace sadfs { namespace comm {

namespace gpio = google::protobuf::io;
namespace gputil = google::protobuf::util;
using request_type_map = std::unordered_map<proto::request_type,
                                            request_type>;
using proto_request_type_map = std::unordered_map<request_type,
                                            proto::request_type>;

namespace {
// handy function aliases (pointers)
auto const serialize = gputil::SerializeDelimitedToZeroCopyStream;
auto const deserialize = gputil::ParseDelimitedFromZeroCopyStream;

auto const proto_req_type_lookup = proto_request_type_map
{
	{request_type::read, proto::request_type::READ},
	{request_type::write, proto::request_type::WRITE},
};

auto const req_type_lookup = request_type_map
{
	{proto::request_type::READ, request_type::read},
	{proto::request_type::WRITE, request_type::write},
};

} // unnamed namespace

/* ========================================================
 *                       file_request
 * ========================================================
 */
file_request::
file_request(std::size_t sender, request_type req_type,
             file_section const section)
{
	protobuf_.set_type(proto_req_type_lookup.at(req_type));
	protobuf_.set_sender(sender);
	protobuf_.set_filename(section.filename);
	protobuf_.set_offset(section.offset);
	protobuf_.set_length(section.length);
}

bool file_request::
send(socket const& sock) const noexcept
{
	auto out = gpio::FileOutputStream(sock.descriptor());
	return serialize(protobuf_, &out);
}

bool file_request::
recv(socket const& sock) noexcept
{
	auto in = gpio::FileInputStream(sock.descriptor());
	return deserialize(&protobuf_, &in, nullptr);
}

std::size_t file_request::
sender() const noexcept
{
	return protobuf_.sender();
}

file_section file_request::
section() const noexcept
{
	return
	{
		.filename = protobuf_.filename(),
		.offset   = protobuf_.offset(),
		.length   = protobuf_.length()
	};
}

request_type file_request::
type() const noexcept
{
	return req_type_lookup.at(protobuf_.type());
}

/* ========================================================
 *                       chunk_request
 * ========================================================
 */
chunk_request::
chunk_request(std::size_t sender, request_type req_type,
              std::size_t chunk_id)
{
	protobuf_.set_type(proto_req_type_lookup.at(req_type));
	protobuf_.set_sender(sender);
	protobuf_.set_chunk_id(chunk_id);
}

bool chunk_request::
send(socket const& sock) const noexcept
{
	auto out = gpio::FileOutputStream(sock.descriptor());
	return serialize(protobuf_, &out);
}

bool chunk_request::
recv(socket const& sock) noexcept
{
	auto in = gpio::FileInputStream(sock.descriptor());
	return deserialize(&protobuf_, &in, nullptr);
}

std::size_t chunk_request::
sender() const noexcept
{
	return protobuf_.sender();
}

std::size_t chunk_request::
chunk_id() const noexcept
{
	return protobuf_.chunk_id();
}

request_type chunk_request::
type() const noexcept
{
	return req_type_lookup.at(protobuf_.type());
}

} // namespace comm
} // namespace sadfs
