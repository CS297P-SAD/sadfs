#ifndef SADFS_MSGS_CLIENT_MESSAGES_HPP
#define SADFS_MSGS_CLIENT_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/client.pb.h>
#include <sadfs/uuid.hpp>

namespace sadfs { namespace msgs { namespace client {
using control_message = proto::client::control_message;
using chunkid = uuid;

// enumerates types of control messages sent to clients
enum class msg_type
{
	unknown,
	chunk_location_response,
};

class chunk_location_response
{
public:
	chunk_location_response() = default;
	chunk_location_response(bool ok, std::vector<comm::service> const& services, 
							chunkid chunk_id);

	bool               ok()       const noexcept;
	comm::service      service(int)  const;
	chunkid            chunk_id() const noexcept;
	int                num_locations()  const;

	inline static msg_type type{msg_type::chunk_location_response};
private:
	proto::client::chunk_location_response protobuf_{};

	friend bool embed(chunk_location_response const&, control_message&);
	friend bool extract(chunk_location_response&, control_message const&);
};

// ==================================================================
//                     inline function definitions
// ==================================================================
inline bool chunk_location_response::
ok() const noexcept
{
	return protobuf_.ok();
}

inline comm::service chunk_location_response::
service(int i) const
{
	return {protobuf_.server_ips(i).c_str(), protobuf_.ports(i)};
}

inline chunkid chunk_location_response::
chunk_id() const noexcept
{
	auto id = chunkid{};
	id.deserialize(protobuf_.chunk_id().data());
	return id;
}

inline int chunk_location_response::
num_locations() const
{
	return protobuf_.server_ips_size();
}

} // client namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_CLIENT_MESSAGES_HPP
