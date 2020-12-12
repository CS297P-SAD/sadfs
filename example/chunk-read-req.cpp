/* example code for testing data streaming */
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/chunk/deserializer.hpp>
#include <sadfs/msgs/chunk/serializer.hpp>
#include <sadfs/msgs/client/deserializer.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/uuid.hpp>

#include <iostream>

using namespace sadfs;
constexpr auto delim = std::string_view{"******************************\n"};

void
print_read_req(msgs::chunk::read_request const& req)
{
    std::cout << delim << "Read Request:"
              << "\nChunk ID:  " << to_string(req.chunk_id())
              << "\nOffset:    " << req.offset()
              << "\nLength:    " << req.length() << "\n"
              << delim << "\n";
}

void
print_read_res(msgs::client::read_response const& res)
{
    std::cout << delim << "Read Response:"
              << "\nOK:  " << res.ok() << "\n";
    if (res.ok())
    {
        std::cout << "Data length: " << res.data().size() << "\n";
        std::cout << "Data:        " << res.data() << "\n";
    }
    std::cout << delim << "\n";
}

void
send_req(std::string id, uint32_t offset, uint32_t length)
{
    auto req =
        msgs::chunk::read_request{chunkid::from_string(id), offset, length};
    print_read_req(req);

    auto service = comm::service{comm::constants::ip_localhost, 6667};
    auto sock    = service.connect();
    if (!sock.valid())
    {
        logger::error("failed to establish a connection "
                      "with the chunk server"sv);
        return;
    }
    auto ch = msgs::channel{std::move(sock)};

    logger::info("connection established with the server"sv);

    msgs::chunk::serializer{}.serialize(req, ch);
    logger::info("sent write stream request"sv);

    // to make sure that the serialized msgs are sent,
    // flush the output buffer
    ch.flush();

    auto res = msgs::client::read_response{};
    msgs::client::deserializer{}.deserialize(res, ch);
    print_read_res(res);
}

int
main(int argc, char** argv)
{
    std::cout << std::boolalpha;

    /* send_req(chunkid, offset, length); */
    send_req(to_string(chunkid::generate()), 4 * 1024, 64);

    // should see <uuid>00000...
    send_req("6cfe7de1-b508-4e26-864e-7887fbc700ef"s, 0, 64);

    // should see <uuid>33333...
    send_req("8cf03844-5a57-47e6-ab60-81f2de556aa4"s, 43 * 64, 64);

    // should fail
    send_req("bf5b432c-3889-4484-aa3c-77a08d30901b"s, 16 * 1024, 64);
    return 0;
}
