/* Code for the sadcd class */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/sadcd/heart.hpp>
#include <sadfs/sadcd/sadcd.hpp>

// standard includes
#include <chrono> // std::chrono_literals
#include <future> // std::promise, std::future

namespace sadfs
{

namespace
{

using namespace std::chrono_literals;
auto ready = [](auto const& future, auto duration) {
    return future.wait_for(duration) == std::future_status::ready;
};

} // unnamed namespace

namespace constants
{

constexpr auto max_chunks = 1000;

} // namespace constants

sadcd::sadcd(char const* ip, int port, char const* master_ip, int master_port,
             char const* server_id)
    : service_(ip, port), master_(master_ip, master_port),
      serverid_(serverid::from_string(server_id))
{
    // do nothing
}

void
sadcd::start()
{
    join_network();
    // set up
    run();
}

void
sadcd::run()
{
    // kill_switch is a switch we flip to kill the main service thread
    // *_death are "flags" child threads will use to indicate death
    auto kill_switch      = std::promise<void>{};
    auto main_thread_dead = std::future<void>{};
    auto main_thread      = std::thread{};
    {
        // grab future that will indicate when the main thread dies
        // due to an error
        auto death       = std::promise<void>{};
        main_thread_dead = death.get_future();

        // start main thread
        main_thread = std::thread{&sadcd::start_main, this, std::move(death),
                                  kill_switch.get_future()};
    }

    // start heartbeat
    auto heart = chunk::heart{master_, serverid_};
    heart.start();

    // poll for failures
    while (true)
    {
        if (ready(main_thread_dead, 1s))
        {
            // get threads to propagate error condition using futures
            logger::error("main thread has died"sv);
            main_thread_dead.get();
            heart.stop();
            break;
        }
        if (!heart.beating())
        {
            logger::error("heartbeats have stopped"sv);
            kill_switch.set_value();
            main_thread.join();
            break;
        }
    }
}

void
sadcd::start_main(std::promise<void>       death,
                  std::shared_future<void> kill_switch)
{
    while (!ready(kill_switch, 1s))
    {
        logger::info("chunk server working..."sv);
    }
    death.set_value(); // unnecessary, but doing this so we don't forget
}

bool
sadcd::join_network()
{
    auto sock = master_.connect();
    if (!sock.valid())
    {
        return false;
    }

    auto ch = msgs::channel{std::move(sock)};
    auto jr = msgs::master::join_network_request{
        service_, constants::max_chunks, /*chunk_count=*/0};

    // send join_network_request
    msgs::master::serializer{{.host_id = serverid_}}.serialize(jr, ch);
    // TODO: verify success
    return true;
}

bool
sadcd::notify_master_of_write(chunkid chunk, version version_num,
                              std::string const& filename,
                              uint32_t           new_chunk_size)
{
    auto sock = master_.connect();
    if (!sock.valid())
    {
        return false;
    }

    auto ch = msgs::channel{std::move(sock)};

    auto cwn = msgs::master::chunk_write_notification{
        chunk, version_num, filename, new_chunk_size};

    msgs::master::serializer{{.host_id = serverid_}}.serialize(cwn, ch);
    // TODO: confirm from master that the write went through
    return true;
}

} // namespace sadfs
