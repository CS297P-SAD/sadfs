#ifndef SADFS_MSGS_MASTER_MESSAGES_HPP
#define SADFS_MSGS_MASTER_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/master.pb.h>
#include <sadfs/uuid.hpp>

// standard includes
#include <cstddef> // std::size_t
#include <string>

namespace sadfs { namespace msgs { namespace master {
using serverid = uuid;
using message_container = proto::master::message_container;

// enumerates types of raw messages
enum class msg_type
{
	unknown,
	chunk_location_request,
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

class join_network_request
{
public:
	join_network_request() = default;
	join_network_request(serverid id);

	//TODO
	//serverid      id()     const;
	
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

} // master namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_MSGS_MASTER_MESSAGES_HPP
