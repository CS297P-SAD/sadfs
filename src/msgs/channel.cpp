/* implementation of channel class that abstracts IO
 * channels used to send and receive control messages*/

// sadfs-specific includes
#include <sadfs/msgs/channel.hpp>

namespace sadfs { namespace msgs {

channel::
channel(comm::socket&& sock) : sock_{std::move(sock)},
	istream_{std::make_unique<gpio::FileInputStream>(sock_.descriptor())},
	ostream_{std::make_unique<gpio::FileOutputStream>(sock_.descriptor())}
{
	// do nothing else
}

void channel::
flush() const noexcept
{
	ostream_->Flush();
}

bool channel::
is_open() const noexcept
{
	return istream_ && ostream_;
}

comm::socket&& channel::
close() noexcept
{
	istream_.reset(nullptr);
	ostream_.reset(nullptr);
	return std::move(sock_);
}

void channel::
open(comm::socket&& sock) noexcept
{
	sock_ = std::move(sock);
	istream_.reset(new gpio::FileInputStream{sock_.descriptor()});
	ostream_.reset(new gpio::FileOutputStream{sock_.descriptor()});
}

} // msgs namespace
} // sadfs namespace
