/* example code for testing sending protobuf requests to master
   this program takes on the role of both a client and chunk server
*/

#include <iostream>
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/chunk/deserializer.hpp>
#include <sadfs/msgs/client/deserializer.hpp>
#include <sadfs/msgs/master/message_processor.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/types.hpp>

using namespace sadfs;

auto establish_conn = []() -> msgs::channel {
    using namespace comm;
    auto&& echod = service{constants::ip_localhost, 6667};
    try
    {
        return msgs::channel{echod.connect()};
    }
    catch (std::system_error ex)
    {
        std::cerr << "[error]: failed to connect to master server\n"
                  << ex.what() << "\n";
        std::exit(1);
    }
};
auto info = [](auto const& msg) { std::cout << "[INFO]: " << msg << "\n"; };

// some version of this can be put in the client
std::pair<bool, chunkid>
request_chunk(std::string filename, size_t offset, char rw)
{
    auto type = (rw == 'r') ? msgs::io_type::read : msgs::io_type::write;
    auto fr   = msgs::master::chunk_location_request{type, filename, offset};

    auto ch = establish_conn();

    // send chunk_location_request
    msgs::master::serializer{}.serialize(fr, ch);
    ch.flush();

    auto response = msgs::client::chunk_location_response{};
    msgs::client::deserializer{}.deserialize(response, ch);

    return {response.ok(), response.chunk_id()};
}

void
join_network(serverid id)
{
    auto jr = msgs::master::join_network_request{
        comm::service{"127.0.0.1", 6668}, 1000, 0};

    auto ch = establish_conn();

    // send join_network_request
    msgs::master::serializer{{.host_id = id}}.serialize(jr, ch);
    ch.flush();
}

void
notify_chunk(serverid sid, chunkid cid, version v, uint32_t num_bytes,
             std::string filename)
{
    auto cwn =
        msgs::master::chunk_write_notification{cid, v, filename, num_bytes};

    auto ch = establish_conn();

    // send chunk_write_notification
    msgs::master::serializer{{.host_id = sid}}.serialize(cwn, ch);
    ch.flush();

    auto ack          = msgs::chunk::acknowledgement{};
    auto deserializer = msgs::chunk::deserializer{};
    if (!(deserializer.deserialize(ack, ch).first && ack.ok()))
    {
        std::cerr << "Notification rejected\n";
    }
}

void
create_file(std::string filename)
{
    auto cfr = msgs::master::create_file_request{filename};

    auto ch = establish_conn();

    // send chunk_write_notification
    msgs::master::serializer{}.serialize(cfr, ch);
    ch.flush();

    auto ack = msgs::client::acknowledgement{};
    if (!(msgs::client::deserializer{}.deserialize(ack, ch).first && ack.ok()))
    {
        std::cerr << "Couldn't create file\n";
    }
}

std::pair<bool, uint64_t>
file_metadata(std::string filename)
{
    auto fmr = msgs::master::file_metadata_request{filename};

    auto ch = establish_conn();

    // send chunk_write_notification
    msgs::master::serializer{}.serialize(fmr, ch);
    ch.flush();

    auto response = msgs::client::file_metadata_response{};
    msgs::client::deserializer{}.deserialize(response, ch);

    return {response.ok(), response.size()};
}

void
release_lock(std::string filename)
{
    auto cfr = msgs::master::release_lock{filename};

    auto ch = establish_conn();

    // send chunk_write_notification
    msgs::master::serializer{}.serialize(cfr, ch);
    ch.flush();
}

int
main(int argc, char** argv)
{
    auto bytes_per_chunk = 64 * 1024 * 1024;
    auto fn              = "/mnt/a/file.dat";

    create_file(fn);
    auto metadata = file_metadata(fn);
    // file should exist and be size zero
    assert(metadata.first);
    assert(metadata.second == 0);

    // add servers
    auto sid1 = serverid::generate();
    auto sid2 = serverid::generate();
    join_network(sid1);
    join_network(sid2);

    // request chunk in empty file
    auto response = request_chunk(fn, 0, 'w');
    assert(response.first);
    auto cid1 = response.second;

    // "write" 200 bytes
    notify_chunk(sid1, cid1, 0, 200, fn);
    notify_chunk(sid2, cid1, 0, 200, fn);
    response = request_chunk(fn, 0, 'w');
    // file should be locked
    assert(!(response.first));

    // unlock and request same chunk
    release_lock(fn);
    response = request_chunk(fn, 0, 'w');
    // file should be unlocked and we should get the same chunkid
    assert(response.first);
    auto cid2 = response.second;
    assert(cid1 == cid2);

    // fill the rest of the chunk
    auto rest_of_chunk = bytes_per_chunk - 200;
    notify_chunk(sid1, cid1, 1, rest_of_chunk, fn);
    notify_chunk(sid2, cid1, 1, rest_of_chunk, fn);
    release_lock(fn);

    // we filled first chunk: request another
    response = request_chunk(fn, 1, 'w');
    assert(response.first);
    cid2 = response.second;
    // Should have a new chunkid
    assert(!(cid1 == cid2));
    notify_chunk(sid1, cid2, 0, bytes_per_chunk, fn);
    notify_chunk(sid2, cid2, 0, bytes_per_chunk, fn);

    // file should now be 2 chunks
    metadata = file_metadata(fn);
    assert(metadata.first);
    assert(metadata.second == bytes_per_chunk * 2);

    info("All test passed");
    return 0;
}
