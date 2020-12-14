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

struct forwarding_spec
{
    chunkid const&             id;
    uint32_t                   length;
    std::string const&         filename;
    std::string_view           data;
    std::vector<comm::service> forwarding_list;
};

bool read(read_spec const);
bool append(write_spec, comm::service const&, serverid const&);

// forwards a write/append request to downstream chunk servers
void forward(forwarding_spec const& spec, serverid const& sid);

auto flush = [](auto const& ch) {
    ch.flush();
    return true;
};

} // namespace io
} // namespace sadfs

#endif //  SADFS_SADCD_IO_HPP
