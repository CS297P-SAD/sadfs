#ifndef SADFS_SADFSD_SADFILESYS_HPP
#define SADFS_SADFSD_SADFILESYS_HPP
#include <sadfs/comm/inet.hpp>

#include <cerrno> // ENOENT

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 26
#endif
#define _FILE_OFFSET_BITS 64
#include <fuse.h>

namespace sadfs {

class sadfilesys
{
public:
    // delete copy constructor and copy assignment operator
    sadfilesys(sadfilesys const&) = delete;
    sadfilesys &operator=(sadfilesys const&) = delete;
    
    // contructor
    sadfilesys(char const* ip, int port);

    // starts fuse_main()
    int bootstrap(int argc, char** argv);

private:
    // assign file system operation function pointers to the corresponding
    // file system operation definitions
    void load_operations();

    // retrieves instance of this class with the help of fuse_get_context() and
    // returns a static pointer to it
    static sadfilesys* this_();
    
    // get file attributes
    int getattr(char const* path, struct stat* stbuf);

    // read directory
    int readdir(char const* path, void* buf, fuse_fill_dir_t filler, off_t off,
                fuse_file_info* fi);

    // open a file
    int open(char const* path, fuse_file_info* fi);

    // read data from an open file
    int read(char const* path, char* buf, size_t size, off_t offset,
             fuse_file_info* fi);

    fuse_operations operations_;
    
    inet::service master_service_;
};

} // sadfs namespace;

#endif // SADFS_SADFSD_SADFILESYS_HPP
