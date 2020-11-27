#ifndef SADFS_COMM_MESSAGES_HPP
#define SADFS_COMM_MESSAGES_HPP

// sadfs-specific includes
#include <sadfs/comm/io.hpp>
#include <sadfs/proto/chunk.pb.h>
#include <sadfs/proto/master.pb.h>

// standard includes
#include <cstddef> // std::size_t
#include <string>

namespace sadfs { namespace msgs {

enum class io_type
{
	unknown,
	read,
	write,
};

// uniquely identifies a host
struct host_id
{
	std::size_t uuid;
};

// ========================================================
//              interfaces to program against
// ========================================================
namespace master {

// enumerates types of raw messages
enum class msg_type
{
	unknown,
	file_request,
};

class control_message
{
public:
	control_message() = default;
	control_message(host_id);

	bool send(comm::io::connection const& conn) const noexcept;
	bool recv(comm::io::connection const& conn)       noexcept;

	msg_type type() const noexcept;
private:
	proto::master::control_message pb_;

	// provide extract and embed functions access to private members
	template <typename RawMsg>
	friend bool extract(RawMsg&, control_message&);

	template <typename RawMsg>
	friend bool embed(RawMsg&, control_message&);
};

// extract/embed function declarations
template <typename RawMsg>
bool extract(RawMsg&, control_message&);

template <typename RawMsg>
bool embed(RawMsg&, control_message&);

// specifies a section of a file
struct file_section
{
	std::string const filename;
	std::size_t const offset;
	std::size_t const length;
};

class file_request
{
public:
	file_request() = default;
	file_request(io_type, file_section const&);

	io_type      type()    const;
	file_section section() const;
	bool         is_set()  const noexcept;
	
	inline static msg_type msg_type{msg_type::file_request};
private:
	using protobuf = proto::master::file_request;
	std::unique_ptr<protobuf> pb_ptr_{};

	// allow extract and embed functions access to private members
	friend bool extract<file_request>(file_request&, control_message&);
	friend bool embed<file_request>(file_request&, control_message&);
};

} // master namespace

namespace chunk {

enum class msg_type
{
	chunk_request,
	unknown,
};

class control_message
{
public:
	control_message() = default;
	control_message(host_id);

	bool send(comm::io::connection const& conn) const noexcept;
	bool recv(comm::io::connection const& conn)       noexcept;

	msg_type type() const noexcept;
private:
	proto::chunk::control_message pb_;

	// provide extract and embed functions access to private members
	template <typename RawMsg>
	friend bool extract(RawMsg&, control_message&);

	template <typename RawMsg>
	friend bool embed(RawMsg&, control_message&);
};

// extract/embed function declarations
template <typename RawMsg>
bool extract(RawMsg&, control_message&);

template <typename RawMsg>
bool embed(RawMsg&, control_message&);

class chunk_request
{
public:
	// constructors
	chunk_request() = default;
	chunk_request(io_type, std::size_t);

	io_type      type()     const;
	std::size_t  chunk_id() const;
	bool         is_set()  const noexcept;

	inline static msg_type msg_type{msg_type::chunk_request};
private:
	using protobuf = proto::chunk::chunk_request;
	std::unique_ptr<protobuf> pb_ptr_{};

	// allow extract and embed functions access to private members
	friend bool extract<chunk_request>(chunk_request&, control_message&);
	friend bool embed<chunk_request>(chunk_request&, control_message&);
};

} // chunk namespace
} // msgs namespace
} // sadfs namespace

#endif // SADFS_COMM_MESSAGES_HPP
