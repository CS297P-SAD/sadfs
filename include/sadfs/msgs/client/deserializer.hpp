#ifndef SADFS_MSGS_CLIENT_DESERIALIZER_HPP
#define SADFS_MSGS_CLIENT_DESERIALIZER_HPP

#include <sadfs/proto/client.pb.h>
#include <sadfs/msgs/deserializer_impl.hpp>

// instantiate a deserializer to receive messages sent to clients
namespace sadfs { namespace msgs { namespace client {
using deserializer = msgs::deserializer<proto::client::message_container>;
} // client namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_CLIENT_DESERIALIZER_HPP
