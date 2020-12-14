#ifndef SADFS_SADCD_IO_HPP
#define SADFS_SADCD_IO_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/types.hpp>

// standard includes
#include <optional>
#include <string>
#include <string_view>

namespace sadfs
{
namespace io
{

struct read_spec
{
    chunkid const& id;
    version        ver;
    uint32_t       offset;
    uint32_t       length;
    std::string&   data;
};

struct write_spec
{
    chunkid const&     id;
    version            ver;
    uint32_t           size;
    uint32_t           offset;
    uint32_t           length;
    std::string const& filename;
    std::string_view   data;
};

bool read(read_spec const);
bool append(write_spec const, comm::service const&, serverid const&);

} // namespace io
} // namespace sadfs

#endif //  SADFS_SADCD_IO_HPP
