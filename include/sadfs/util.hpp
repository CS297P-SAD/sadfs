/* defines sadfs-wise utilities */
#ifndef SADFS_UTIL_HPP
#define SADFS_UTIL_HPP

// sadfs-specific includes
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/common.hpp> // message_header

// standard includes
#include <utility> // std::declval

namespace sadfs
{
namespace msgs
{

// metafunction to help with the detection idiom
template <typename Handler, typename MessageType>
using can_handle = decltype(
    std::declval<Handler>().handle(std::declval<MessageType const &>(),
                                   std::declval<message_header const &>(),
                                   std::declval<msgs::channel const &>()));

} // namespace msgs

} // namespace sadfs

#endif // SADFS_UTIL_HPP
