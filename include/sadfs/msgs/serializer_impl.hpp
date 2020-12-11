#ifndef SADFS_MSGS_SERIALIZERS_HPP
#define SADFS_MSGS_SERIALIZERS_HPP

// sadfs-specific includes
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/common.hpp>

// standard includes
#include <iterator>

// external includes
#include <google/protobuf/util/delimited_message_util.h>

namespace sadfs
{
namespace msgs
{

// Container refers to a container for control messages
template <typename Container> class serializer
{
public:
    serializer() = default; // TODO: remove
    serializer(message_header header);

    template <typename ControlMessage>
    bool serialize(ControlMessage const &, channel const &);

private:
    Container      container_{};
    message_header header_{};

    friend class msgs::channel;
    bool serialize(gpio::ZeroCopyOutputStream *) const;
};

// template definitions
template <typename Container>
template <typename ControlMessage>
bool
msgs::serializer<Container>::serialize(ControlMessage const &cm,
                                       channel const &       ch)
{
    header_.host_id.serialize(
        std::back_inserter(*container_.mutable_header()->mutable_host_id()));
    auto res = embed(cm, container_) && ch.accept_serializer(*this);

    // we must not hold data after serialization
    container_.clear_msg();
    return res;
}

template <typename Container>
bool
serializer<Container>::serialize(gpio::ZeroCopyOutputStream *out) const
{
    namespace gputil     = google::protobuf::util;
    auto const serialize = gputil::SerializeDelimitedToZeroCopyStream;

    return serialize(container_, out);
}

// inline definitions
template <typename Container>
inline serializer<Container>::serializer(message_header header)
    : header_{std::move(header)}
{
}

} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_SERIALIZERS_HPP
