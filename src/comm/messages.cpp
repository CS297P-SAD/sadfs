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
using request_type_map = std::unordered_map<file_request::type,
                                            proto::request_type>;

namespace {
// handy function aliases (pointers)
const auto serialize = gputil::SerializeDelimitedToZeroCopyStream;
const auto deserialize = gputil::ParseDelimitedFromZeroCopyStream;

const auto lookup = request_type_map
{
	{file_request::type::read, proto::request_type::READ},
	{file_request::type::write, proto::request_type::WRITE},
};

} // unnamed namespace

file_request::
file_request(std::size_t sender, file_request::type req_type,
             file_section const section)
{
	protobuf_.set_type(lookup.at(req_type));
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

std::size_t file_request::
sender() const noexcept
{
	return protobuf_.sender();
}

} // namespace comm
} // namespace sadfs
