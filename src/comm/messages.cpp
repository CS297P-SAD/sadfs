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
	{io_type::read, proto::io_type::READ},
	{io_type::write, proto::io_type::WRITE},
};

auto const io_type_lookup = io_type_map
{
	{proto::io_type::READ, io_type::read},
	{proto::io_type::WRITE, io_type::write},
};

} // unnamed namespace

/* ========================================================
 *                       request_base
 * ========================================================
 */
template <typename Protobuf>
bool request_base::
send(socket const& sock, Protobuf const& protobuf) const noexcept
{
	auto out = gpio::FileOutputStream(sock.descriptor());
	return serialize(protobuf, &out);
}

template <typename Protobuf>
bool request_base::
recv(socket const& sock, Protobuf& protobuf) noexcept
{
	auto in = gpio::FileInputStream(sock.descriptor());
	return deserialize(&protobuf, &in, nullptr);
}

/* ========================================================
 *                       file_request
 * ========================================================
 */
file_request::
file_request(std::size_t sender, io_type type,
             file_section const section)
{
	protobuf_.set_type(proto_io_type_lookup.at(type));
	protobuf_.set_sender(sender);
	protobuf_.set_filename(section.filename);
	protobuf_.set_offset(section.offset);
	protobuf_.set_length(section.length);
}

bool file_request::
send(socket const& sock) const noexcept
{
	return request_base::send(sock, protobuf_);
}

bool file_request::
recv(socket const& sock) noexcept
{
	return request_base::recv(sock, protobuf_);
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

io_type file_request::
type() const noexcept
{
	return io_type_lookup.at(protobuf_.type());
}

/* ========================================================
 *                       chunk_request
 * ========================================================
 */
chunk_request::
chunk_request(std::size_t sender, io_type type,
              std::size_t chunk_id)
{
	protobuf_.set_type(proto_io_type_lookup.at(type));
	protobuf_.set_sender(sender);
	protobuf_.set_chunk_id(chunk_id);
}

bool chunk_request::
send(socket const& sock) const noexcept
{
	return request_base::send(sock, protobuf_);
}

bool chunk_request::
recv(socket const& sock) noexcept
{
	return request_base::recv(sock, protobuf_);
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

io_type chunk_request::
type() const noexcept
{
	return io_type_lookup.at(protobuf_.type());
}

} // namespace comm
} // namespace sadfs
