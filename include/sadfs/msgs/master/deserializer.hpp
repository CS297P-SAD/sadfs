#ifndef SADFS_MSGS_MASTER_DESERIALIZER_HPP
#define SADFS_MSGS_MASTER_DESERIALIZER_HPP

#include <sadfs/proto/master.pb.h>
#include <sadfs/msgs/deserializer_impl.hpp>

// instantiate a deserializer to receive messages sent to the master server
namespace sadfs { namespace msgs { namespace master {
using deserializer = msgs::deserializer<proto::master::message_container>;
} // master namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_MASTER_DESERIALIZER_HPP
