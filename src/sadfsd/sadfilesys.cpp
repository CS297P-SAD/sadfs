/* Definitions for the sadfilesys class */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/sadfsd/sadfilesys.hpp>

namespace sadfs {

sadfilesys::
sadfilesys(char const* ip, int port) : master_service_(ip, port)
{
    load_operations();
}

int sadfilesys::
bootstrap(int argc, char** argv)
{
    return ::fuse_main(argc, argv, &operations_, this);
}

sadfilesys* sadfilesys::
this_()
{
    return static_cast<sadfilesys*>(fuse_get_context()->private_data);
}

void sadfilesys::
load_operations()
{
    operations_.getattr     = [](char const* path, struct stat* stbuf)->int
                              {
                                return this_()->getattr(path, stbuf);
                              };
    operations_.open        = [](char const* path, fuse_file_info* fi)->int
                              {
                                return this_()->open(path, fi);
                              };
    operations_.read        = [](char const* path, char* buf, size_t size,
                                 off_t offset, fuse_file_info* fi)->int
                              {
                                return this_()->read(path, buf, size, offset,
                                                     fi);
                              };
    operations_.readdir     = [](char const* path, void* buf,
                                 fuse_fill_dir_t filler, off_t off,
                                 fuse_file_info* fi)->int
                              {
                                return this_()->readdir(path, buf, filler, off,
                                                        fi);
                              };
}

int sadfilesys::
getattr(char const* path, struct stat* stbuf)
{
    // TODO
    return -ENOENT;
}

int sadfilesys::
readdir(char const* path, void* buf, fuse_fill_dir_t filler, off_t off,
        fuse_file_info* fi)
{
    // TODO
    return -ENOENT;
}

int sadfilesys::
open(char const* path, fuse_file_info* fi)
{
    // TODO
    return -ENOENT;
}

int sadfilesys::
read(char const* path, char* buf, size_t size, off_t offset,
     fuse_file_info* fi)
{
    // TODO
    return -ENOENT;
}

} // sadfs namespace
