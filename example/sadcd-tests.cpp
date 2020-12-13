/* tests chunk servers */
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/chunk/messages.hpp>
#include <sadfs/msgs/chunk/serializer.hpp>
#include <sadfs/msgs/client/deserializer.hpp>
#include <sadfs/msgs/client/messages.hpp>
#include <sadfs/types.hpp>
#include <sadfs/uuid.hpp>

#include <iostream>

using namespace sadfs;
auto const     service = comm::service{"127.0.0.1", 6667};
auto           passed  = 0;
auto           total   = 0;
constexpr auto delim   = std::string_view{"******************************\n"};

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
test_read_match(std::string id, uint32_t offset, uint32_t length,
                bool expected, std::string data)
{
    total++;
    auto cid = uuid::from_string(id);
    auto req = msgs::chunk::read_request{cid, offset, length};
    print_read_req(req);
    auto sock = service.connect();
    if (!sock.valid())
    {
        logger::error("invalid socket"sv);
        return;
    }

    auto ch = msgs::channel{std::move(sock)};
    if (!msgs::chunk::serializer{}.serialize(req, ch))
    {
        logger::error("could not send request"sv);
        return;
    }
    ch.flush();

    auto res = msgs::client::read_response{};
    if (!msgs::client::deserializer{}.deserialize(res, ch).first)
    {
        logger::error("could not receive response"sv);
        return;
    }

    if (res.ok() != expected)
    {
        logger::error("unexpected response"sv);
        return;
    }

    if (expected && res.data().size() != length)
    {
        logger::error("unexpected data size"sv);
        return;
    }

    if (data != res.data())
    {
        logger::error("unexpected data"sv);
        return;
    }
    ++passed;
}

void
test_read(std::string id, uint32_t offset, uint32_t length, bool expected)
{
    total++;
    auto cid = uuid::from_string(id);
    auto req = msgs::chunk::read_request{cid, offset, length};
    print_read_req(req);
    auto sock = service.connect();
    if (!sock.valid())
    {
        logger::error("invalid socket"sv);
        return;
    }

    auto ch = msgs::channel{std::move(sock)};
    if (!msgs::chunk::serializer{}.serialize(req, ch))
    {
        logger::error("could not send request"sv);
        return;
    }
    ch.flush();

    auto res = msgs::client::read_response{};
    if (!msgs::client::deserializer{}.deserialize(res, ch).first)
    {
        logger::error("could not receive response"sv);
        return;
    }

    if (res.ok() != expected)
    {
        logger::error("unexpected response"sv);
        return;
    }

    if (expected && res.data().size() != length)
    {
        logger::error("unexpected data size"sv);
        return;
    }
    ++passed;
}

void
test_append(std::string id, uint32_t length, std::string data, bool expected)
{
    total++;
    auto cid = uuid::from_string(id);
    auto req =
        msgs::chunk::append_request{cid, length, {}, "irrelevant_name.dat"};
    auto sock = service.connect();
    if (!sock.valid())
    {
        logger::error("invalid socket"sv);
        return;
    }

    auto ch = msgs::channel{std::move(sock)};
    if (!msgs::chunk::serializer{}.serialize(req, ch))
    {
        logger::error("could not send request"sv);
        return;
    }
    ch.flush();

    auto ack = msgs::client::acknowledgement{};
    if (!msgs::client::deserializer{}.deserialize(ack, ch).first)
    {
        logger::error("could not receive response"sv);
        return;
    }

    if (ack.ok() != expected)
    {
        logger::error("unexpected response"sv);
        return;
    }
    if (!expected)
    {
        ++passed;
        return;
    }

    auto stream = msgs::chunk::stream{std::move(data)};
    if (!msgs::chunk::serializer{}.serialize(stream, ch))
    {
        logger::error("could not stream data"sv);
        return;
    }
    ch.flush();

    ++passed;
}

int
main()
{
    auto const id0 = to_string(uuid::generate());
    auto const data =
        "This is a long string that we want to send to a chunk server...\n"s;

    // try to read a chunk that doesn't exist
    test_read(id0, 0, 64, false);

    // append to it, thus creating it
    test_append(id0, data.size(), data, true);

    // try reading now
    test_read(id0, 0, 64, true);
    // try reading at a high offset
    test_read(id0, 4 * 1024, 64, false);

    // append a couple more times
    test_append(id0, data.size(), data, true);
    test_append(id0, data.size(), data, true);

    // read all three data segments back
    test_read_match(id0, 0 * 64, 64, true, data);
    test_read_match(id0, 1 * 64, 64, true, data);
    test_read_match(id0, 2 * 64, 64, true, data);

    // try to read another
    test_read(id0, 3 * 64, 64, false);

    std::cout << "\nRESULTS: " << passed << "/" << total << " tests passed.\n";
}
