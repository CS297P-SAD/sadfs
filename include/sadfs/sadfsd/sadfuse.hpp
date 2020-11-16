#ifndef SADFS_SADFSD_SADFUSE_HPP
#define SADFS_SADFSD_SADFUSE_HPP
#include <sadfs/comm/socket.hpp>

#include <memory> // std::shared_ptr

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 35
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

    // returns singleton instance of the class
    static sadfuse& instance();
    
    // creates connection to master at ip and port and stores it in
    // master_socket_
    void set_master_connection(char const* ip, int port);

    // starts fuse_main()
    int run(int argc, char** argv);

    // returns operations_
    fuse_operations& operations();

private:
    // private constructor to enforce singleton property
    sadfuse();

    // assign file system operation function pointers to the corresponding
    // file system operation definitions
    void load_operations();
    
    // get file attributes
    static int getattr(const char* path, struct stat* stbuf, fuse_file_info* fi);

    // read directory
    static int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t off,
                fuse_file_info* fi, fuse_readdir_flags flags);

    // open a file
    static int open(const char* path, fuse_file_info* fi);

    // read data from an open file
    static int read(const char* path, char* buf, size_t size, off_t offset,
             fuse_file_info* fi);

    fuse_operations operations_;
    
    std::shared_ptr<sadfs::socket> master_socket_;
};

} // sadfs namespace;

#endif // SADFS_SADFSD_SADFUSE_HPP
