#ifndef SADFS_MSGS_MASTER_SERIALIZER_HPP
#define SADFS_MSGS_MASTER_SERIALIZER_HPP

#include <sadfs/proto/master.pb.h>
#include <sadfs/msgs/serializer_impl.hpp>

// instantiate a serializer to send messages to the master server
namespace sadfs { namespace msgs { namespace master {
using serializer = msgs::serializer<proto::master::message_container>;
} // master namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_MASTER_SERIALIZER_HPP
