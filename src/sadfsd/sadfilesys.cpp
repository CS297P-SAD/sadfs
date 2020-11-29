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
    operations_.readlink    = nullptr;
    operations_.mknod       = nullptr;
    operations_.mkdir       = nullptr;
    operations_.unlink      = nullptr;
    operations_.rmdir       = nullptr;
    operations_.symlink     = nullptr;
    operations_.rename      = nullptr;
    operations_.link        = nullptr;
    operations_.chmod       = nullptr;
    operations_.chown       = nullptr;
    operations_.truncate    = nullptr;
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
    operations_.write       = nullptr;
    operations_.statfs      = nullptr;
    operations_.flush       = nullptr;
    operations_.release     = nullptr;
    operations_.fsync       = nullptr;
    operations_.setxattr    = nullptr;
    operations_.getxattr    = nullptr;
    operations_.listxattr   = nullptr;
    operations_.removexattr = nullptr;
    operations_.opendir     = nullptr;
    operations_.readdir     = [](char const* path, void* buf,
                                 fuse_fill_dir_t filler, off_t off,
                                 fuse_file_info* fi)->int
                              {
                                return this_()->readdir(path, buf, filler, off,
                                                        fi);
                              };
    operations_.releasedir  = nullptr;
    operations_.fsyncdir    = nullptr;
    operations_.init        = nullptr;
    operations_.destroy     = nullptr;
    operations_.access      = nullptr;
    operations_.create      = nullptr;
    operations_.lock        = nullptr;
    operations_.utimens     = nullptr;
    operations_.bmap        = nullptr;
    operations_.ioctl       = nullptr;
    operations_.poll        = nullptr;
    operations_.write_buf   = nullptr;
    operations_.read_buf    = nullptr;
    operations_.flock       = nullptr;
    operations_.fallocate   = nullptr; 
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
