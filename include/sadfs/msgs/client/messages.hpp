#ifndef SADFS_MSGS_CLIENT_MESSAGES_HPP
#define SADFS_MSGS_CLIENT_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/client.pb.h>
#include <sadfs/uuid.hpp>

namespace sadfs { namespace msgs { namespace client {
using proto::client::message_container;
using chunkid = uuid;
using version = unsigned int;

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
				chunkid chunk_id, version version_num);

	bool               ok()             const noexcept;
	comm::service      service(int)     const;
	chunkid            chunk_id()       const noexcept;
	uint32_t           version_num()    const;
	int                locations_size() const noexcept;

	inline static msg_type type{msg_type::chunk_location_response};
private:
	proto::client::chunk_location_response protobuf_{};

	friend bool embed(chunk_location_response const&, message_container&);
	friend bool extract(chunk_location_response&, message_container const&);
};

// declarations
bool embed(chunk_location_response const&, message_container&);
bool extract(chunk_location_response&, message_container const&);

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
locations_size() const noexcept
{
	return protobuf_.server_ips_size();
}

inline uint32_t chunk_location_response::
version_num() const
{
	return protobuf_.version_num();
}
  
} // client namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_CLIENT_MESSAGES_HPP
