#ifndef SADFS_SADCD_HEART_HPP
#define SADFS_SADCD_HEART_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp> // comm::service

// standard includes
#include <chrono>
#include <future>
#include <thread>

namespace sadfs
{
namespace chunk
{

// sends heartbeats to the master server
class heart
{
    comm::service master_;
    std::thread heartbeat_{};
    std::promise<void> stop_request_;
    std::future<void> stopped_;

  public:
    heart(comm::service const &master) : master_{master} {}

    void start();
    void stop();
    bool beating();
};

inline bool
heart::beating()
{
    if (!stopped_.valid())
    {
        return false;
    }

    using namespace std::chrono_literals;
    auto beating = stopped_.wait_for(0ms) != std::future_status::ready;

    if (!beating)
    {
        stopped_.get(); // avoid bugs
    }
    return beating;
}

} // namespace chunk
} // namespace sadfs

#endif // SADFS_SADCD_HEART_HPP
