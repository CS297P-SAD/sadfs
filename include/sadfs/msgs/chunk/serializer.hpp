#ifndef SADFS_MSGS_CHUNK_SERIALIZER_HPP
#define SADFS_MSGS_CHUNK_SERIALIZER_HPP

#include <sadfs/proto/chunk.pb.h>
#include <sadfs/msgs/serializer_impl.hpp>

// instantiate a serializer to send messages to the chunk server
namespace sadfs { namespace msgs { namespace chunk {
using serializer = msgs::serializer<proto::chunk::message_container>;
} // chunk namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_CHUNK_SERIALIZER_HPP
