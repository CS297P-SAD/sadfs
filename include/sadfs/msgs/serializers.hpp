#ifndef SADFS_MSGS_SERIALIZERS_HPP
#define SADFS_MSGS_SERIALIZERS_HPP

// sadfs-specific includes
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/messages.hpp>

// external includes
#include <google/protobuf/util/delimited_message_util.h>

namespace sadfs { namespace msgs {

// uses ControlMessage as a container for raw control messages
template <typename ControlMessage>
class serializer
{
public:
	serializer() = default; // TODO: remove
	serializer(host_id id);

	template <typename RawMessage>
	bool serialize(RawMessage const&, channel const&);

private:
	ControlMessage cm_{};

	friend class msgs::channel;
	bool serialize(gpio::ZeroCopyOutputStream*) const;
};

// template definitions
template <typename ControlMessage>
template <typename RawMessage>
bool msgs::serializer<ControlMessage>::
serialize(RawMessage const& rm, channel const& ch)
{
	auto res = embed(rm, cm_) && ch.accept_serializer(*this);
	// we must not hold data after serialization
	cm_.clear_msg();
	return res;
}

template <typename ControlMessage>
bool serializer<ControlMessage>::
serialize(gpio::ZeroCopyOutputStream* out) const
{
	namespace gputil = google::protobuf::util;
	auto const serialize = gputil::SerializeDelimitedToZeroCopyStream;

	return serialize(cm_, out);
}

// define a serializer to send messages to the master server
namespace master {
using serializer = msgs::serializer<proto::master::control_message>;
}

// define a serializer to send messages to chunk servers
namespace chunk {
using serializer = msgs::serializer<proto::chunk::control_message>;
}

} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_SERIALIZERS_HPP
