#ifndef SADFS_MSGS_MASTER_MESSAGES_HPP
#define SADFS_MSGS_MASTER_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/master.pb.h>
#include <sadfs/types.hpp>

// standard includes
#include <cstddef> // std::size_t
#include <string>

namespace sadfs { namespace msgs { namespace master {
using message_container = proto::master::message_container;

// enumerates types of control messages
enum class msg_type
{
	unknown,
	chunk_location_request,
	chunk_server_heartbeat,
	file_metadata_request,
	join_network_request
};

class chunk_location_request
{
public:
	chunk_location_request() = default;
	chunk_location_request(msgs::io_type, std::string const& filename,
	                       std::size_t chunk_number);

	msgs::io_type      io_type()     const;
	std::string const& filename()    const;
	std::size_t        chunk_number() const;
	
	inline static msg_type type{msg_type::chunk_location_request};
private:
	proto::master::chunk_location_request protobuf_{};

	// provide embed/extract functions access to private members
	friend bool embed(chunk_location_request const&, message_container&);
	friend bool extract(chunk_location_request&, message_container const&);
};

// declarations
bool embed(chunk_location_request const&, message_container&);
bool extract(chunk_location_request&, message_container const&);

class chunk_server_heartbeat
{
public:
	chunk_server_heartbeat() = default;

	inline static msg_type type{msg_type::chunk_server_heartbeat};
private:
	proto::master::chunk_server_heartbeat protobuf_;

	friend bool embed(chunk_server_heartbeat const&, message_container&);
	friend bool extract(chunk_server_heartbeat&, message_container const&);
};

// declarations
bool embed(chunk_server_heartbeat const&, message_container&);
bool extract(chunk_server_heartbeat&, message_container const&);

class file_metadata_request
{
public:
	file_metadata_request() = default;
	file_metadata_request(std::string const& filename);

	std::string const& filename()	const;

	inline static msg_type type{msg_type::file_metadata_request};
private:
	proto::master::file_metadata_request protobuf_{};

	// provide embed/extract functions access to private members
	friend bool embed(file_metadata_request const&, message_container&);
	friend bool extract(file_metadata_request&, message_container const&);
};

// declarations
bool embed(file_metadata_request const&, message_container&);
bool extract(file_metadata_request&, message_container const&);

class join_network_request
{
public:
	join_network_request() = default;
	join_network_request(serverid server_id, comm::service service,
	                     uint64_t max_chunks, uint64_t chunk_count);

	serverid server_id()   const;
	comm::service service() const;
	uint64_t max_chunks()  const;
	uint64_t chunk_count() const;
	
	inline static msg_type type{msg_type::join_network_request};
private:
	proto::master::join_network_request protobuf_{};

	// provide embed/extract functions access to private members
	friend bool embed(join_network_request const&, message_container&);
	friend bool extract(join_network_request&, message_container const&);
};

// declarations
bool embed(join_network_request const&, message_container&);
bool extract(join_network_request&, message_container const&);

// ==================================================================
//                     inline function definitions
// ==================================================================
inline std::string const& chunk_location_request::
filename() const
{
	return protobuf_.filename();
}

inline std::size_t chunk_location_request::
chunk_number() const
{
	return protobuf_.chunk_number();
}

inline std::string const& file_metadata_request::
filename() const
{
	return protobuf_.filename();
}

inline serverid join_network_request::
server_id() const
{
	auto id = serverid{};
	id.deserialize(protobuf_.server_id().data());
	return id;
}

inline comm::service join_network_request::
service() const
{
	return {protobuf_.ip().c_str(), protobuf_.port()};
}

inline uint64_t join_network_request::
max_chunks() const
{
	return protobuf_.max_chunks();
}

inline uint64_t join_network_request::
chunk_count() const
{
	return protobuf_.chunk_count();
}

} // master namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_MASTER_MESSAGES_HPP
