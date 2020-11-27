// sadfs-specifc includes
#include <sadfs/comm/io.hpp>

// standard includes
#include <utility>   // std::move

namespace sadfs { namespace comm { namespace io {
namespace gpio = google::protobuf::io;

connection::
connection(socket&& sock) :
	sock_(std::move(sock)),
	istream_(std::make_unique<gpio::FileInputStream>(sock_.descriptor())),
	ostream_(std::make_unique<gpio::FileOutputStream>(sock_.descriptor()))
{
	// do nothing
}

gpio::ZeroCopyInputStream* connection::
istream() const noexcept
{
	return istream_.get();
}

gpio::ZeroCopyOutputStream* connection::
ostream() const noexcept
{
	return ostream_.get();
}

void connection::
flush() const noexcept
{
	ostream_->Flush();
}

} // io namespace
} // comm namespace
} // sadfs namespace
