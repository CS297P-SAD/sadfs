#ifndef SADFS_SADFSD_SADFUSE_HPP
#define SADFS_SADFSD_SADFUSE_HPP
#include <sadfs/comm/inet.hpp>

#include <memory> // std::shared_ptr
#include <cerrno> // ENOENT

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 26
#endif
#define _FILE_OFFSET_BITS 64
#include <fuse.h>

namespace sadfs {

class sadfuse
{
public:
    // delete copy constructor and copy assignment operator
    sadfuse(sadfuse const&) = delete;
    sadfuse &operator=(sadfuse const&) = delete;
    
    // contructor
    sadfuse(char const* ip, int port);

    // starts fuse_main()
    int run(int argc, char** argv);

private:
    // assign file system operation function pointers to the corresponding
    // file system operation definitions
    void load_operations();
    
    // get file attributes
    static int getattr(char const* path, struct stat* stbuf);

    // read directory
    static int readdir(char const* path, void* buf, fuse_fill_dir_t filler, off_t off,
                fuse_file_info* fi);

    // open a file
    static int open(char const* path, fuse_file_info* fi);

    // read data from an open file
    static int read(char const* path, char* buf, size_t size, off_t offset,
             fuse_file_info* fi);

    static fuse_operations operations_;
    
    inet::service master_service_;
};

} // sadfs namespace;

#endif // SADFS_SADFSD_SADFUSE_HPP
