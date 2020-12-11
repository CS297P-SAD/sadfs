#ifndef SADFS_MSGS_COMMON_HPP
#define SADFS_MSGS_COMMON_HPP

// sadfs-specific includes
#include <sadfs/proto/common.pb.h>
#include <sadfs/uuid.hpp>

// standard includes
#include <cstddef> // std::size_t
#include <string>

namespace sadfs
{
namespace msgs
{

enum class io_type
{
    unknown,
    read,
    write,
};

// header of every control message
struct message_header
{
    uuid host_id;
};

using io_type_map       = std::unordered_map<proto::io_type, io_type>;
using proto_io_type_map = std::unordered_map<io_type, proto::io_type>;
inline auto const proto_io_type_lookup = proto_io_type_map{
    {io_type::unknown, proto::io_type::UNKNOWN},
    {io_type::read, proto::io_type::READ},
    {io_type::write, proto::io_type::WRITE},
};

inline auto const io_type_lookup = io_type_map{
    {proto::io_type::UNKNOWN, io_type::unknown},
    {proto::io_type::READ, io_type::read},
    {proto::io_type::WRITE, io_type::write},
};

// ==================================================================
//                            acknowledgement
// ==================================================================
template <typename MessageContainer, typename Type, Type msg_type>
class acknowledgement
{
public:
    acknowledgement() = default;
    acknowledgement(bool ok) { protobuf_.set_ok(ok); }

    bool ok() const noexcept { return protobuf_.ok(); }

    inline static Type type{msg_type};

private:
    proto::acknowledgement protobuf_{};

    // provide embed/extract functions access to private members
    friend bool embed(acknowledgement const &, MessageContainer &);
    friend bool extract(acknowledgement &, MessageContainer const &);
};

} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_COMMON_HPP
