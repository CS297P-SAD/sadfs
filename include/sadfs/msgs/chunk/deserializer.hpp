#ifndef SADFS_MSGS_CHUNK_DESERIALIZER_HPP
#define SADFS_MSGS_CHUNK_DESERIALIZER_HPP

#include <sadfs/proto/chunk.pb.h>
#include <sadfs/msgs/deserializer_impl.hpp>

// instantiate a deserializer to receive messages sent chunk servers
namespace sadfs { namespace msgs { namespace chunk {
using deserializer = msgs::deserializer<proto::chunk::message_container>;
} // chunk namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_CHUNK_DESERIALIZER_HPP
