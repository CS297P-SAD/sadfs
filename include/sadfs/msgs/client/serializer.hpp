#ifndef SADFS_MSGS_CLIENT_SERIALIZER_HPP
#define SADFS_MSGS_CLIENT_SERIALIZER_HPP

#include <sadfs/proto/client.pb.h>
#include <sadfs/msgs/serializer_impl.hpp>

// instantiate a serializer to send messages to the client server
namespace sadfs { namespace msgs { namespace client {
using serializer = msgs::serializer<proto::client::message_container>;
} // client namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_CLIENT_SERIALIZER_HPP
