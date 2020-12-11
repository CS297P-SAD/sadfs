#ifndef SADFS_MSGS_CHUNK_MESSAGE_PROCESSOR_HPP
#define SADFS_MSGS_CHUNK_MESSAGE_PROCESSOR_HPP

// sadfs-specific includes
#include <sadfs/is_detected.hpp>
#include <sadfs/msgs/chunk/deserializer.hpp>
#include <sadfs/msgs/client/serializer.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/msgs/messages.hpp>

// standard includes
#include <utility> // std::pair

namespace sadfs
{
namespace msgs
{
namespace chunk
{

class processor : public deserializer
{
public:
    template <typename Handler>
    std::pair<bool, bool> process_next(channel const&, Handler&);
};

// template definition
template <typename Handler>
std::pair<bool, bool>
processor::process_next(msgs::channel const& ch, Handler& h)
{
    auto header         = message_header{};
    auto extract_header = [&header, this]() {
        header.host_id.deserialize(container_.header().host_id().data());
        return true;
    };
    auto [res, eof] = ch.accept_deserializer(*this);
    switch (container_.msg_case())
    {
    case container_type::kAck:
        static_assert(
            !is_detected_v<can_handle, Handler, acknowledgement>,
            "acknowledgement control message must be handled explicitly");
        break;
    case container_type::kStreamReq:
        if constexpr (is_detected_v<can_handle, Handler, stream_request>)
        {
            auto msg = stream_request{};
            res      = res && extract_header() && extract(msg, container_) &&
                  h.handle(msg, header, ch);
        }
        else
        {
            // cannot handle this message
            res = false;
        }
        break;
    }
    container_.clear_msg();
    return {res, eof};
}

} // namespace chunk
} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_CHUNK_MESSAGE_PROCESSOR_HPP
