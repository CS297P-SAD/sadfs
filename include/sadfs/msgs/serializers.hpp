#ifndef SADFS_MSGS_SERIALIZERS_HPP
#define SADFS_MSGS_SERIALIZERS_HPP

// sadfs-specific includes
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/msgs/client_messages.hpp>

// external includes
#include <google/protobuf/util/delimited_message_util.h>

namespace sadfs { namespace msgs {

// Container refers to a container for control messages
template <typename Container>
class serializer
{
public:
	serializer() = default; // TODO: remove
	serializer(host_id id);

	template <typename ControlMessage>
	bool serialize(ControlMessage const&, channel const&);

private:
	Container container_{};

	friend class msgs::channel;
	bool serialize(gpio::ZeroCopyOutputStream*) const;
};

// template definitions
template <typename Container>
template <typename ControlMessage>
bool msgs::serializer<Container>::
serialize(ControlMessage const& cm, channel const& ch)
{
	auto res = embed(cm, container_) && ch.accept_serializer(*this);
	// we must not hold data after serialization
	container_.clear_msg();
	return res;
}

template <typename Container>
bool serializer<Container>::
serialize(gpio::ZeroCopyOutputStream* out) const
{
	namespace gputil = google::protobuf::util;
	auto const serialize = gputil::SerializeDelimitedToZeroCopyStream;

	return serialize(container_, out);
}

// define a serializer to send messages to the master server
namespace master {
using serializer = msgs::serializer<proto::master::message_container>;
}

// define a serializer to send messages to chunk servers
namespace chunk {
using serializer = msgs::serializer<proto::chunk::message_container>;
}

// define a serializer to send messages to clients
namespace client {
using serializer = msgs::serializer<proto::client::control_message>;
}

} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_SERIALIZERS_HPP
