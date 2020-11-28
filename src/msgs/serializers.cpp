/* defines serializer classes */

// sadfs-specific includes
#include <sadfs/msgs/serializers.hpp>

// external includes
#include <google/protobuf/util/delimited_message_util.h>

namespace sadfs { namespace msgs {

namespace gpio = google::protobuf::io;
namespace gputil = google::protobuf::util;

// useful aliases
auto const serialize = gputil::SerializeDelimitedToZeroCopyStream;

namespace master {

bool serializer::
serialize(gpio::ZeroCopyOutputStream* out) const
{
	return msgs::serialize(cm_, out);
}

} // master namespace

} // msgs namespace
} // sadfs namespace
