#ifndef SADFS_COMM_UUID_HPP
#define SADFS_COMM_UUID_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

namespace sadfs {

struct uuid_generator : boost::uuids::random_generator
{
};

} // sadfs namespace

#endif // SADFS_COMM_UUID_HPP