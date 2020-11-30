#ifndef SADFS_MSGS_DESERIALIZERS_HPP
#define SADFS_MSGS_DESERIALIZERS_HPP

// sadfs-specific includes
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/messages.hpp>

// external includes
#include <google/protobuf/util/delimited_message_util.h>

namespace sadfs { namespace msgs {

// uses ControlMessage as a container for raw control messages
template <typename ControlMessage>
class deserializer
{
public:
	deserializer() = default; // TODO: remove
	deserializer(host_id id);

	template <typename RawMessage>
	bool deserialize(RawMessage&, channel const&);

private:
	ControlMessage cm_{};

	friend class msgs::channel;
	bool deserialize(gpio::ZeroCopyInputStream*);
};

// template definitions
template <typename ControlMessage>
template <typename RawMessage>
bool msgs::deserializer<ControlMessage>::
deserialize(RawMessage& rm, channel const& ch)
{
	auto res =  ch.accept_deserializer(*this) && extract(rm, cm_);
	// we must not hold data after serialization
	cm_.clear_msg();
	return res;
}

template <typename ControlMessage>
bool deserializer<ControlMessage>::
deserialize(gpio::ZeroCopyInputStream* in)
{
	namespace gputil = google::protobuf::util;
	auto const deserialize = gputil::ParseDelimitedFromZeroCopyStream;

	// TODO: change API to return clean_eof
	return deserialize(&cm_, in, /*clean_eof=*/nullptr);
}

// define a deserializer to receive messages sent to the master server
namespace master {
using deserializer = msgs::deserializer<proto::master::control_message>;
}

// define a deserializer to receive messages sent to chunk servers
namespace chunk {
using deserializer = msgs::deserializer<proto::chunk::control_message>;
}
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_DESERIALIZERS_HPP
