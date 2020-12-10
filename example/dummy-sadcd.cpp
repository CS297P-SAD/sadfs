/* dummy implementation of sadcd that sends heartbeats for 5 seconds */
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/sadcd/heart.hpp>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using namespace sadfs;
struct sadcd
{
    void run(comm::service const &master)
    {
        auto heart = chunk::heart{master};
        logger::debug("starting heartbeat...");
        heart.start();
        logger::debug("started");

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5s);

        logger::debug("stopping heartbeat...");
        heart.stop();
        logger::debug("stopped");
    }
};

int
main()
{
    sadcd{}.run({comm::constants::ip_localhost, 6666});
}
