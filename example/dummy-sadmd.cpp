/* dummy implementation of sadmd that responds favourably to all messages */
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/chunk/messages.hpp>
#include <sadfs/msgs/chunk/serializer.hpp>
#include <sadfs/msgs/client/messages.hpp>
#include <sadfs/msgs/client/serializer.hpp>
#include <sadfs/msgs/master/message_processor.hpp>
#include <sadfs/uuid.hpp>

#include <iostream>

using namespace sadfs;
struct sadmd
{
    bool start()
    {
        auto listener = comm::listener{{comm::constants::ip_localhost, 6666}};
        while (true)
        {
            logger::debug("waiting for connections..."sv);
            auto sock = listener.accept();
            logger::debug("accepted connection"sv);
            serve_requests(std::move(sock));
        }
    }

    void serve_requests(msgs::channel ch)
    {
        auto processor = msgs::master::processor{};
        bool result{false}, eof{false};
        while (true)
        {
            auto [result, eof] = processor.process_next(ch, *this);
            if (result)
            {
                logger::debug("request served successfully"sv);
            }
            else if (eof)
            {
                logger::debug("EOF"sv);
                break;
            }
            else
            {
                logger::debug("request service failed"sv);
            }
        }
    }

    using clr = msgs::master::chunk_location_request;
    bool handle(clr const &req, msgs::message_header const &header,
                msgs::channel const &ch)
    {
        logger::debug("received req. from: " + to_string(header.host_id));
        auto response = msgs::client::chunk_location_response{
            /*ok=*/true,
            {{"10.0.0.13", 6666}},
            uuid::generate(),
            0 // version
        };
        auto result = msgs::client::serializer{}.serialize(response, ch);
        ch.flush();
        return result;
    }

    using hb = msgs::master::chunk_server_heartbeat;
    bool handle(hb const &hb, msgs::message_header const &header,
                msgs::channel const &ch)
    {
        logger::debug("received req. from: " + to_string(header.host_id));
        auto response = msgs::chunk::acknowledgement{/*ok=*/true};
        auto result   = msgs::chunk::serializer{}.serialize(response, ch);
        ch.flush();
        return result;
    }
};

int
main()
{
    return sadmd{}.start();
}
