/* example code for testing sending protobuf requests to master
   this program takes on the role of both a client and chunk server
*/

#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/client/deserializer.hpp>
#include <sadfs/msgs/master/message_processor.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/types.hpp>
#include <iostream>

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
chunkid
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

	return response.chunk_id();
}

void
join_network(serverid id)
{
    auto jr = msgs::master::join_network_request{
        id, comm::service{"127.0.0.1", 6668}, 1000, 0};

    auto ch = establish_conn();

    // send join_network_request
    msgs::master::serializer{}.serialize(jr, ch);
    ch.flush();
}

void
notify_chunk(serverid sid, chunkid cid, version v, uint32_t num_bytes, std::string filename)
{
    auto cwn = msgs::master::chunk_write_notification{
        sid, cid, v, filename, num_bytes};

    auto ch = establish_conn();

    // send chunk_write_notification
    msgs::master::serializer{}.serialize(cwn, ch);
    ch.flush();
}

void
create_file(std::string filename)
{
    auto cfr = msgs::master::create_file_request{filename};

    auto ch = establish_conn();

    // send chunk_write_notification
    msgs::master::serializer{}.serialize(cfr, ch);
    ch.flush();
}

bool
file_exists(std::string filename)
{
    auto fmr = msgs::master::file_metadata_request{filename};

    auto ch = establish_conn();

    // send chunk_write_notification
    msgs::master::serializer{}.serialize(fmr, ch);
    ch.flush();

    auto response = msgs::client::file_metadata_response{};
    msgs::client::deserializer{}.deserialize(response, ch);

	//info(filename + " has size " + std::to_string(response.size()));
    return response.ok();
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
    std::cout << std::boolalpha;
	auto fn = "/mnt/a/file.dat";
    create_file(fn);
    if (file_exists(fn))
    {
        assert(true); // file should exist
        auto sid1 = serverid::generate();
        auto sid2 = serverid::generate();
        join_network(sid1);
        join_network(sid2);
		auto cid1 = request_chunk(fn, 0, 'w');
        notify_chunk(sid1, cid1, 0, 200, fn);
        notify_chunk(sid2, cid1, 0, 200, fn);
		auto cid2 = request_chunk(fn, 0, 'w');
        // file should be locked
        assert(cid2 == uuid::from_string("00000000-0000-0000-0000-000000000000"));
        release_lock(fn);
		cid2 = request_chunk(fn, 0, 'w');
        //file should be unlocked and we should get the same chunkid
        assert(cid1 == cid2);
        auto rest_of_chunk = bytes_per_chunk - 200;
        notify_chunk(sid1, cid1, 1, rest_of_chunk, fn);
        notify_chunk(sid2, cid1, 1, rest_of_chunk, fn);
        release_lock(fn);
		cid2 = request_chunk(fn, 1, 'w');
        //Should have a new chunkid
        assert(!(cid1 == cid2));
        notify_chunk(sid1, cid2, 0, bytes_per_chunk, fn);
        notify_chunk(sid2, cid2, 0, bytes_per_chunk, fn);
    }
    info("All test passed");
    return 0;
}
