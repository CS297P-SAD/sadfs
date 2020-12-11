/* implements the chunk::heart class */
// sadfs-specific includes
#include <sadfs/exceptions.hpp>
#include <sadfs/msgs/chunk/deserializer.hpp>
#include <sadfs/msgs/chunk/messages.hpp>
#include <sadfs/msgs/master/messages.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/sadcd/heart.hpp>

namespace sadfs
{
namespace chunk
{

// heartbeat function
void
heart::beat(std::promise<void> death, std::future<void> stop_token)
{
    auto msg          = msgs::master::chunk_server_heartbeat{};
    auto serializer   = msgs::master::serializer{{.host_id = host_id_}};
    auto deserializer = msgs::chunk::deserializer{};
    auto die          = [&death]() { return death.set_value(); };

    // send heartbeats until requested to stop
    using namespace std::chrono_literals;
    while (stop_token.wait_for(1s) != std::future_status::ready)
    {
        try
        {
            auto ch    = msgs::channel{master_.connect()};
            auto ack   = msgs::chunk::acknowledgement{};
            auto flush = [](auto const &ch) {
                ch.flush();
                return true;
            };
            if (serializer.serialize(msg, ch) && flush(ch) &&
                deserializer.deserialize(ack, ch).first && ack.ok())
            {
                continue;
            }
            throw sadfs::operation_failure{"could not send heartbeat"};
        }
        catch (std::exception const &)
        {
            return die();
        }
    }

    // stop requested
    stop_token.get();
    die();
}

// starts the heartbeat in a concurrent thread
void
heart::start()
{
    if (beating())
    {
        throw sadfs::logic_error{"heart beating already"};
    }

    auto death    = std::promise<void>{};
    stop_request_ = std::promise<void>{};
    stopped_      = death.get_future();

    heartbeat_ = std::thread{&heart::beat, this, std::move(death),
                             stop_request_.get_future()};
}

// stops sending heartbeats
void
heart::stop()
{
    if (!beating())
    {
        throw sadfs::logic_error{"heart not beating"};
    }

    // notify the heartbeat thread that we want it to stop :(
    stop_request_.set_value();
    heartbeat_.join();
    stopped_.get();
}

} // namespace chunk
} // namespace sadfs
