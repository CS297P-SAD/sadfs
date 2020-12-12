#ifndef SADFS_MSGS_MASTER_MESSAGE_PROCESSOR_HPP
#define SADFS_MSGS_MASTER_MESSAGE_PROCESSOR_HPP

// sadfs-specific includes
#include <sadfs/is_detected.hpp>
#include <sadfs/msgs/master/deserializer.hpp>
#include <sadfs/msgs/master/messages.hpp>
#include <sadfs/util.hpp> // msgs::can_handle

// standard includes
#include <utility> // std::pair

namespace sadfs
{
namespace msgs
{
namespace master
{

class processor : public deserializer
{
public:
    template <typename Handler>
    std::pair<bool, bool> process_next(channel const&, Handler&);
};

// template definitions
template <typename Handler>
std::pair<bool, bool>
processor::process_next(channel const& ch, Handler& h)
{
    auto header         = message_header{};
    auto extract_header = [&header, this]() {
        header.host_id.deserialize(container_.header().host_id().data());
        return true;
    };
    auto [res, eof] = ch.accept_deserializer(*this);
    switch (container_.msg_case())
    {
    case container_type::MsgCase::kFileMetadataReq:
        if constexpr (is_detected_v<can_handle, Handler,
                                    file_metadata_request>)
        {
            auto msg = file_metadata_request{};
            res      = res && extract_header() && extract(msg, container_) &&
                  h.handle(msg, header, ch);
        }
        else
        {
            // cannot handle this message
            res = false;
        }
        break;
    case container_type::MsgCase::kCreateFileReq:
        if constexpr (is_detected_v<can_handle, Handler, create_file_request>)
        {
            auto msg = create_file_request{};
            res      = res && extract_header() && extract(msg, container_) &&
                  h.handle(msg, header, ch);
        }
        else
        {
            // cannot handle this message
            res = false;
        }
        break;
    case container_type::MsgCase::kChunkWriteNotify:
        if constexpr (is_detected_v<can_handle, Handler,
                                    chunk_write_notification>)
        {
            auto msg = chunk_write_notification{};
            res      = res && extract_header() && extract(msg, container_) &&
                  h.handle(msg, header, ch);
        }
        else
        {
            // cannot handle this message
            res = false;
        }
        break;
    case container_type::MsgCase::kChunkLocationReq:
        if constexpr (is_detected_v<can_handle, Handler,
                                    chunk_location_request>)
        {
            auto msg = chunk_location_request{};
            res      = res && extract_header() && extract(msg, container_) &&
                  h.handle(msg, header, ch);
        }
        else
        {
            // cannot handle this message
            res = false;
        }
        break;
    case container_type::MsgCase::kChunkServerHeartbeat:
        if constexpr (is_detected_v<can_handle, Handler,
                                    chunk_server_heartbeat>)
        {
            auto msg = chunk_server_heartbeat{};
            res      = res && extract_header() && extract(msg, container_) &&
                  h.handle(msg, header, ch);
        }
        else
        {
            // cannot handle this message
            res = false;
        }
        break;
    case container_type::MsgCase::kJoinNetworkReq:
        if constexpr (is_detected_v<can_handle, Handler, join_network_request>)
        {
            auto msg = join_network_request{};
            res      = res && extract_header() && extract(msg, container_) &&
                  h.handle(msg, header, ch);
        }
        else
        {
            // cannot handle this message
            res = false;
        }
        break;
    case container_type::MsgCase::kReleaseLock:
        if constexpr (is_detected_v<can_handle, Handler, release_lock>)
        {
            auto msg = release_lock{};
            res      = res && extract_header() && extract(msg, container_) &&
                  h.handle(msg, header, ch);
        }
        else
        {
            // cannot handle this message
            res = false;
        }
        break;
    case container_type::MsgCase::MSG_NOT_SET:
        // nothing to handle
        res = false;
        break;
    }
    container_.clear_msg();
    return {res, eof};
}

} // namespace master
} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_MASTER_MESSAGE_PROCESSOR_HPP
