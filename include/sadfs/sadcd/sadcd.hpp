#ifndef SADFS_SADCD_SADCD_HPP
#define SADFS_SADCD_SADCD_HPP
#include <sadfs/comm/inet.hpp>

#include <string>

namespace sadfs {

class sadcd
{
public:
    sadfs::inet::ip_addr ip;
    sadfs::inet::port_no port;
    // starts server by opening a listener
    void start();
private:
    // reads the message from a socket that just received some data
    std::string process_message(sadfs::socket&);
};

} // sadfs namespace

#endif // SADFS_SADCD_SADCD_HPP
