#ifndef SADFS_MSGS_CHANNEL_HPP
#define SADFS_MSGS_CHANNEL_HPP

// sadfs-specific includes
#include <sadfs/comm/socket.hpp>

// standard includes
#include <memory>  // std::unique_ptr
#include <utility> // std::pair

// external includes
#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace sadfs { namespace msgs {
namespace gpio = google::protobuf::io;

// communication channel for control messages
class channel
{
public:
	channel(comm::socket&&);

	// accepts a serializer, and provides it
	// access to the undelrying output stream
	template <typename Serializer>
	bool accept_serializer(Serializer const&)     const noexcept;

	// accepts a deserializer, and provides it
	// access to the underlying input stream
	template <typename Deserializer>
	std::pair<bool, bool> accept_deserializer(Deserializer&) const noexcept;

	// flushes the output buffer
	void flush()   const noexcept;

	// close channel
	// output buffers are flushed, and closed
	// unread data in input buffers will be lost
	void close()         noexcept;
	bool is_open() const noexcept;

private:
	comm::socket                            sock_;
	std::unique_ptr<gpio::FileInputStream>  istream_;
	std::unique_ptr<gpio::FileOutputStream> ostream_;
};

// template definitions
template <typename Serializer>
bool channel::
accept_serializer(Serializer const& serializer) const noexcept
{
	return serializer.serialize(ostream_.get());
}

template <typename Deserializer>
std::pair<bool, bool> channel::
accept_deserializer(Deserializer& deserializer) const noexcept
{
	return deserializer.deserialize(istream_.get());
}

// inline function definitions
inline channel::
channel(comm::socket&& sock) : sock_{std::move(sock)},
	istream_{std::make_unique<gpio::FileInputStream>(sock_.descriptor())},
	ostream_{std::make_unique<gpio::FileOutputStream>(sock_.descriptor())}
{
	// do nothing else
}

inline void channel::
flush() const noexcept
{
	ostream_->Flush();
}

inline bool channel::
is_open() const noexcept
{
	return istream_ && ostream_;
}

inline void channel::
close() noexcept
{
	istream_.reset(nullptr);
	ostream_.reset(nullptr);
	auto tmp = std::move(sock_);
}

} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_CHANNEL_HPP
