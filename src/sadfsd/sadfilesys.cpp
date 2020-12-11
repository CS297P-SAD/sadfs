/* Definitions for the sadfilesys class */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/client/deserializer.hpp>
#include <sadfs/msgs/client/messages.hpp>
#include <sadfs/msgs/master/messages.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/sadfsd/sadfilesys.hpp>

// external includes
#include <cstring> // strerror()

namespace sadfs
{

sadfilesys::sadfilesys(char const* ip, int port) : master_service_(ip, port)
{
    load_operations();
}

int
sadfilesys::bootstrap(int argc, char** argv)
{
    return ::fuse_main(argc, argv, &operations_, this);
}

sadfilesys*
sadfilesys::this_()
{
    return static_cast<sadfilesys*>(fuse_get_context()->private_data);
}

void
sadfilesys::load_operations()
{
    operations_.getattr = [](char const* path, struct stat* stbuf) -> int {
        return this_()->getattr(path, stbuf);
    };
    operations_.open = [](char const* path, fuse_file_info* fi) -> int {
        return this_()->open(path, fi);
    };
    operations_.read = [](char const* path, char* buf, size_t size,
                          off_t offset, fuse_file_info* fi) -> int {
        return this_()->read(path, buf, size, offset, fi);
    };
    operations_.readdir = [](char const* path, void* buf,
                             fuse_fill_dir_t filler, off_t off,
                             fuse_file_info* fi) -> int {
        return this_()->readdir(path, buf, filler, off, fi);
    };
}

int
sadfilesys::getattr(char const* path, struct stat* stbuf)
{
    logger::debug("gettattr() called at " + std::string(path));

    auto result{0};
    // base directory where filesystem is mounted
    if (strcmp(path, "/") == 0)
    {
        logger::debug("path identified as base directory"sv);
        stbuf->st_mode = S_IFDIR | 0755;
        return result;
    }

    auto filename     = std::string{path};
    auto request      = msgs::master::file_metadata_request{filename};
    auto response     = msgs::client::file_metadata_response{};
    auto serializer   = msgs::master::serializer{};
    auto deserializer = msgs::client::deserializer{};
    auto sock         = master_service_.connect();
    if (!sock.valid())
    {
        // Protocol error, modify it to something more apt
        result = -EPROTO;
        logger::debug("gettattr() failed with error " +
                      std::string(strerror(-result)));
        return result;
    }
    auto ch = msgs::channel{std::move(sock)};

    auto flush = [](auto const& ch) {
        ch.flush();
        return true;
    };

    if (!(serializer.serialize(request, ch) && flush(ch) &&
          deserializer.deserialize(response, ch).first))
    {
        // Protocol error, modify it to something more apt
        result = -EPROTO;
        logger::debug("gettattr() failed with error " +
                      std::string(strerror(-result)));
        return result;
    }
    if (!response.ok())
    {
        result = -ENOENT; // No such file or directory
        logger::debug("gettattr() failed with error " +
                      std::string(strerror(-result)));
        return result;
    }

    logger::debug("path identified as valid filename"sv);

    stbuf->st_mode = S_IFREG | 0666;
    stbuf->st_size = response.size();
    return result;
}

int
sadfilesys::readdir(char const* path, void* buf, fuse_fill_dir_t filler,
                    off_t off, fuse_file_info* fi)
{
    // TODO
    return -ENOENT;
}

int
sadfilesys::open(char const* path, fuse_file_info* fi)
{
    // TODO
    return 0;
}

int
sadfilesys::read(char const* path, char* buf, size_t size, off_t offset,
                 fuse_file_info* fi)
{
    // TODO
    return -ENOENT;
}

} // namespace sadfs
