#ifndef SADFS_SADCD_HEART_HPP
#define SADFS_SADCD_HEART_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp> // comm::service
#include <sadfs/types.hpp>

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
    comm::service      master_;
    serverid           host_id_;
    std::thread        heartbeat_{};
    std::promise<void> stop_request_;
    std::future<void>  stopped_;

    void beat(std::promise<void>, std::future<void>);

public:
    heart(comm::service const &master, serverid id)
        : master_{master}, host_id_{id}
    {
    }
    ~heart();

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

inline heart::~heart()
{
    if (beating())
    {
        stop();
    }
    if (heartbeat_.joinable())
    {
        heartbeat_.join();
    }
}

} // namespace chunk
} // namespace sadfs

#endif // SADFS_SADCD_HEART_HPP
