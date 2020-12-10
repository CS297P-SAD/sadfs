#ifndef SADFS_TYPES_HPP
#define SADFS_TYPES_HPP

#include <sadfs/uuid.hpp>

namespace sadfs {
using chunkid  = uuid;
using serverid = uuid;
using version = unsigned int;
} // sadfs namespace

#endif // SADFS_TYPES_HPP
