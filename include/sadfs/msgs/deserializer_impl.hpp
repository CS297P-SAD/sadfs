#ifndef SADFS_MSGS_DESERIALIZERS_HPP
#define SADFS_MSGS_DESERIALIZERS_HPP

// sadfs-specific includes
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/common.hpp>

// external includes
#include <google/protobuf/util/delimited_message_util.h>

namespace sadfs
{
namespace msgs
{

// Container refers to a container for control messages
template <typename Container> class deserializer
{
public:
    deserializer() = default; // TODO: remove

    template <typename ControlMessage>
    std::pair<bool, bool> deserialize(ControlMessage &, channel const &);

protected:
    using container_type = Container;
    Container container_{};

private:
    friend class msgs::channel;
    std::pair<bool, bool> deserialize(gpio::ZeroCopyInputStream *);
};

// template definitions
template <typename Container>
template <typename ControlMessage>
std::pair<bool, bool>
msgs::deserializer<Container>::deserialize(ControlMessage &cm,
                                           channel const & ch)
{
    auto res  = ch.accept_deserializer(*this);
    res.first = res.first && extract(cm, container_);

    // we must not hold data after deserialization
    container_.clear_msg();
    return res;
}

template <typename Container>
std::pair<bool, bool>
deserializer<Container>::deserialize(gpio::ZeroCopyInputStream *in)
{
    namespace gputil       = google::protobuf::util;
    auto const deserialize = gputil::ParseDelimitedFromZeroCopyStream;

    auto result = false, eof = false;
    result = deserialize(&container_, in, &eof);

    return {result, eof};
}

} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_DESERIALIZERS_HPP
