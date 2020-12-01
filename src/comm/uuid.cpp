/* implementation of sadfs::uuid */

#include <sadfs/comm/uuid.hpp>

#include <boost/uuid/uuid_generators.hpp>

namespace sadfs  {

uuid::
uuid() : boost::uuids::uuid(boost::uuids::random_generator()())
{
    // do nothing
}

} // sadfs namespace