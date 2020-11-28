#ifndef SADFS_MSGS_SERIALIZERS_HPP
#define SADFS_MSGS_SERIALIZERS_HPP

// sadfs-specific includes
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/messages.hpp>

namespace sadfs { namespace msgs {

namespace master {

// serializes messages intended for the master server
class serializer
{
public:
	serializer() = default; // TODO: remove
	serializer(host_id id);

	template <typename RawMessage>
	bool serialize(RawMessage const&, channel const&);

private:
	proto::master::control_message cm_{};

	friend class msgs::channel;
	bool serialize(gpio::ZeroCopyOutputStream*) const;
};

// template definition
template <typename RawMessage>
bool serializer::
serialize(RawMessage const& rm, channel const& ch)
{
	auto res = embed(rm, cm_) && ch.accept_serializer(*this);
	// we must not hold data after serialization
	cm_.clear_msg();
	return res;
}

} // master namespace

} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_SERIALIZERS_HPP
