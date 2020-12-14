/* defines io operation functions used by chunk servers */
// sadfs-specific includes
#include <sadfs/logger.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/chunk/deserializer.hpp>
#include <sadfs/msgs/chunk/messages.hpp>
#include <sadfs/msgs/chunk/serializer.hpp>
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
write_to_disk(write_spec const& spec)
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
notify_master(write_spec const& spec, comm::service const& master,
              serverid const& sid)
{
    auto sock = master.connect();
    if (!sock.valid())
    {
        return false;
    }

    auto ch           = msgs::channel{std::move(sock)};
    auto ack          = msgs::chunk::acknowledgement{};
    auto serializer   = msgs::master::serializer{{sid}};
    auto deserializer = msgs::chunk::deserializer{};

    auto flush = [](auto const& ch) {
        ch.flush();
        return true;
    };

    auto cwn = msgs::master::chunk_write_notification{
        spec.id, spec.ver + 1, spec.filename, spec.size + spec.length};

    // confirm from master that the write went through
    return (serializer.serialize(cwn, ch) && flush(ch) &&
            deserializer.deserialize(ack, ch).first && ack.ok());
}

bool
forward(forwarding_spec const& spec, msgs::channel const& ch, size_t start)
{
    auto serializer   = msgs::chunk::serializer{};
    auto deserializer = msgs::chunk::deserializer{};
    auto ack          = msgs::chunk::acknowledgement{};
    auto req          = msgs::chunk::append_forward_request{
        spec.id,
        spec.length,
        {spec.forwarding_list.begin() + start, spec.forwarding_list.end()},
        spec.filename};
    auto stream = msgs::chunk::stream{std::string{spec.data}};

    return serializer.serialize(req, ch) && flush(ch) &&
           deserializer.deserialize(ack, ch).first && ack.ok() &&
           serializer.serialize(stream, ch) && flush(ch) &&
           deserializer.deserialize(ack, ch).first && ack.ok();
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

void
forward(forwarding_spec const& spec, serverid const& sid)
{
    // stream data to the first server in the forwarding list
    auto [sock, start] = [&spec]() {
        for (auto iter = spec.forwarding_list.begin();
             iter != spec.forwarding_list.end(); iter++)
        {
            auto sock = iter->connect();
            if (sock.valid())
            {
                size_t connected = iter - spec.forwarding_list.begin();
                // return socket and index of the next server in the list
                // so that the server we connected to can then forward
                // the write/append request
                return std::make_pair(std::move(sock), connected + 1);
            }
            logger::error("data could not be forwarded to " +
                          to_string(iter->ip()) + ":" +
                          std::to_string(to_int(iter->port())));
        }
        return std::make_pair(comm::socket{}, spec.forwarding_list.size());
    }();
    if (!sock.valid())
    {
        return;
    }

    if (!forward(spec, std::move(sock), start))
    {
        logger::error("data could not be forwarded"sv);
    }
    return;
}

} // namespace io
} // namespace sadfs
