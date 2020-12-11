#ifndef SADFS_MSGS_COMMON_HPP
#define SADFS_MSGS_COMMON_HPP

// sadfs-specific includes
#include <sadfs/proto/common.pb.h>

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

// uniquely identifies a host
struct host_id
{
    std::size_t uuid;
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
// forward declarations
template <typename MessageContainer, typename Type, Type msg_type>
class acknowledgement;

template <typename MessageContainer, typename Type, Type msg_type>
bool embed(acknowledgement<MessageContainer, Type, msg_type> const&,
           MessageContainer&);

template <typename MessageContainer, typename Type, Type msg_type>
bool extract(acknowledgement<MessageContainer, Type, msg_type>&,
             MessageContainer const&);

// class declaration
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
    // one-to-one relationship
    //
    // further reading:
    // https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Making_New_Friends
    friend bool embed<>(acknowledgement const&, MessageContainer&);
    friend bool extract<>(acknowledgement&, MessageContainer const&);
};

} // namespace msgs
} // namespace sadfs

#endif // SADFS_MSGS_COMMON_HPP
