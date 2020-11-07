#ifndef SADFS_SADMD_SADMD_HPP
#define SADFS_SADMD_SADMD_HPP
#include <sadfs/comm/inet.hpp>

#include <string>

class sadmd
{
public:
    sadfs::inet::port_no port;
    sadfs::inet::ip_addr ip;
    // starts server by opening a listener
    void start();
private:
    // reads the message from a socket that just received some data
    std::string process_message(sadfs::socket&);
};

#endif // SADFS_SADMD_SADMD_HPP