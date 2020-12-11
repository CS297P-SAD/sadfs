/* dummy implementation of sadcd that sends heartbeats for 5 seconds */
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/sadcd/heart.hpp>
#include <sadfs/types.hpp>
#include <sadfs/uuid.hpp>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using namespace sadfs;
struct sadcd
{
    serverid id;
    sadcd() : id{serverid::generate()} {}
    void run(comm::service const &master)
    {
        auto heart = chunk::heart{master, id};
        logger::debug("starting heartbeat..."sv);
        heart.start();
        logger::debug("started"sv);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5s);

        logger::debug("stopping heartbeat..."sv);
        heart.stop();
        logger::debug("stopped"sv);
    }
};

int
main()
{
    sadcd{}.run({comm::constants::ip_localhost, 6666});
}
