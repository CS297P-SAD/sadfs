#ifndef SADFS_COMM_UUID_HPP
#define SADFS_COMM_UUID_HPP

#include <boost/uuid/uuid.hpp>

namespace sadfs {

struct uuid : boost::uuids::uuid
{
    uuid();
};

} // sadfs namespace

#endif // SADFS_COMM_UUID_HPP