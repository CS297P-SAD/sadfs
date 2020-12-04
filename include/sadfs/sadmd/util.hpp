#ifndef SADFS_SADMD_UTIL_HPP
#define SADFS_SADMD_UTIL_HPP

// sadfs-specific includes
#include <sadfs/proto/internal.pb.h>
#include <sadfs/uuid.hpp>

// standard includes
#include <string>

namespace sadfs { namespace util {

using chunkid = sadfs::uuid;

// wrapper class around sadfs protobuf object for storing list of uuids
class file_chunks
{
public:
    file_chunks() = default;

    void add_chunk(chunkid const& id)
    {
        protobuf_.add_chunkids(to_string(id));
    }

    std::string serialize()
    {
        auto pb_str = std::string{};
        protobuf_.SerializeToString(&pb_str);
        return pb_str;
    }

    void deserialize(std::string const& str)
    {
        protobuf_.ParseFromString(str);
    }

    chunkid operator[](int const i)
    {
        return chunkid::from_string(protobuf_.chunkids(i));
    }

    std::size_t size()
    {
        return protobuf_.chunkids_size();
    }

private:
	proto::internal::file_chunks protobuf_{};
};

} // util namespace
} // sadfs namespace

#endif
