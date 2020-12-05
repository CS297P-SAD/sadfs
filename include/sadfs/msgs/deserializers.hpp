#ifndef SADFS_MSGS_DESERIALIZERS_HPP
#define SADFS_MSGS_DESERIALIZERS_HPP

// sadfs-specific includes
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/messages.hpp>

// external includes
#include <google/protobuf/util/delimited_message_util.h>

namespace sadfs { namespace msgs {

// Container refers to a container for control messages
template <typename Container>
class deserializer
{
public:
	deserializer() = default; // TODO: remove
	deserializer(host_id id);

	template <typename ControlMessage>
	bool deserialize(ControlMessage&, channel const&);

private:
	Container container_{};

	friend class msgs::channel;
	bool deserialize(gpio::ZeroCopyInputStream*);
};

// template definitions
template <typename Container>
template <typename ControlMessage>
bool msgs::deserializer<Container>::
deserialize(ControlMessage& cm, channel const& ch)
{
	auto res =  ch.accept_deserializer(*this) && extract(cm, container_);
	// we must not hold data after serialization
	container_.clear_msg();
	return res;
}

template <typename Container>
bool deserializer<Container>::
deserialize(gpio::ZeroCopyInputStream* in)
{
	namespace gputil = google::protobuf::util;
	auto const deserialize = gputil::ParseDelimitedFromZeroCopyStream;

	// TODO: change API to return clean_eof
	return deserialize(&container_, in, /*clean_eof=*/nullptr);
}

// define a deserializer to receive messages sent to the master server
namespace master {
using deserializer = msgs::deserializer<proto::master::message_container>;
}

// define a deserializer to receive messages sent to chunk servers
namespace chunk {
using deserializer = msgs::deserializer<proto::chunk::message_container>;
}

// define a deserializer to receive messages sent to clients
namespace client {
using deserializer = msgs::deserializer<proto::client::control_message>;
}

} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_DESERIALIZERS_HPP
