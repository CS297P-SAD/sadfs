#ifndef SADFS_SADMD_INTERNAL_HPP
#define SADFS_SADMD_INTERNAL_HPP

// sadfs-specific includes
#include <sadfs/proto/internal.pb.h>
#include <sadfs/uuid.hpp>

// standard includes
#include <string>

namespace sadfs {

// wrapper class around sadfs protobuf object for storing list of uuids
class file_chunks
{
public:
	file_chunks() = default;

    void add_chunk(sadfs::uuid id)
    {
        protobuf_.add_uuids(to_string(id));
    }

	std::string as_string()
	{
        auto pb_str = std::string();
		protobuf_.SerializeToString(&pb_str);
        return pb_str;
	}

    void from_string(std::string str)
	{
        protobuf_.ParseFromString(str);
	}

    std::string 
    operator[](int i)
    {
        //TODO: handle error.. or convert this to an iterator?
        if (i > protobuf_.uuids_size()) return std::string{};
        return protobuf_.uuids(i);
    }

    size_t size()
    {
        return protobuf_.uuids_size();
    }

private:
	proto::internal::file_chunks protobuf_{};
};


} // sadfs namespace

#endif