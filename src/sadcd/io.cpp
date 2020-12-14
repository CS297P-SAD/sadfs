/* defines io operation functions used by chunk servers */
// sadfs-specific includes
#include <sadfs/logger.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/chunk/deserializer.hpp>
#include <sadfs/msgs/master/messages.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/sadcd/io.hpp>

// standard includes
#ifndef __APPLE__
#include <filesystem> // std::create_directory
#else
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#endif // __APPLE__

#include <fstream> // std::ifstream, std::ofstream
#include <optional>
#include <string_view>
#include <system_error> // std::system_error

namespace sadfs
{
namespace io
{

namespace
{

auto const current_filename = "current"s;

bool
mkdir(std::string_view path)
{
    // get directory path
    path.remove_suffix(current_filename.size());
#ifndef __APPLE__
    auto ec = std::error_code{};
    if (std::filesystem::create_directory(path, ec))
    {
        return true;
    }
    logger::error(ec.message());
#else
    if (::mkdir(std::string{path}.c_str(),
                S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0)
    {
        return true;
    }
    logger::error(std::string_view{std::strerror(errno)});
#endif // __APPLE__
    return false;
}

std::string
filename(chunkid const& id, std::optional<uint32_t> version)
{
    // TODO: should be /var/lib/sadcd/chunk/ + ...
    return "chunk/" + to_string(id) + "/" +
           (version ? std::to_string(version.value()) : current_filename);
}

bool
write_to_disk(write_spec const spec)
{
    auto const path = filename(spec.id, {});
    if (spec.ver == 0 && !mkdir(path))
    {
        return false;
    }

    auto file = std::ofstream{path, std::ios_base::out | std::ios_base::in |
                                        std::ios_base::app};
    if (!file.is_open())
    {
        logger::error("could not open "s + path);
        return false;
    }
    file.seekp(spec.offset, std::ios_base::beg);
    file.write(spec.data.begin(), spec.length);
    if (!file.good())
    {
        logger::error("write to " + path + " failed");
        return false;
    }

    return true;
}

bool
notify_master(write_spec const spec, comm::service const& master,
              serverid const& sid)
{
    auto sock = master.connect();
    if (!sock.valid())
    {
        return false;
    }

    auto ch = msgs::channel{std::move(sock)};

    auto cwn = msgs::master::chunk_write_notification{
        spec.id, spec.ver + 1, spec.filename, spec.size + spec.length};

    // TODO: confirm from master that the write went through
    return msgs::master::serializer{{sid}}.serialize(cwn, ch);
}

} // unnamed namespace

bool
append(write_spec const spec, comm::service const& master, serverid const& sid)
{
    if (!write_to_disk(spec))
    {
        return false;
    }
    // TODO: write to DB
    if (!notify_master(spec, master, sid))
    {
        return false;
    }
    return true;
}

} // namespace io
} // namespace sadfs
