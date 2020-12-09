#ifndef SADFS_SADCD_SADCD_HPP
#define SADFS_SADCD_SADCD_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/socket.hpp>

// standard includes
#include <future>
#include <string>

namespace sadfs
{

class sadcd
{
public:
    sadcd(char const *ip, int port);

    // starts server by opening a listener
    void start();

private:
    // runs the chunk service
    void run();

    // starts main server thread
    // stops when asked to, indicates death via std::promise
    void start_main(std::promise<void>, std::shared_future<void>);

    comm::service const master_;
};

} // namespace sadfs

#endif // SADFS_SADCD_SADCD_HPP
