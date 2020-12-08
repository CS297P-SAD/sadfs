/* example code for testing sending chunk_location_request


Note: to see non-error cases, copy this code block to the top of sadmd::start()

create_file("/mnt/a/file.dat");
append_chunk_to_file("/mnt/a/file.dat", chunkid::generate());
append_chunk_to_file("/mnt/a/file.dat", chunkid::generate());
auto server1 = serverid::generate();
add_server_to_network(server1, "0.0.0.10", 6543, 1000, 0);
auto server2 = serverid::generate();
add_server_to_network(server2, "0.0.0.20", 9876, 1000, 0);
auto server3 = serverid::generate();
add_server_to_network(server3, "0.0.0.30", 9999, 1000, 0);

for (auto file : files_)
{
	auto ids = file.second.chunkids;
	for (auto i = 0; i < ids.size(); i++)
	{
		add_chunk_to_server(ids[i], server1);
		add_chunk_to_server(ids[i], server2);
		add_chunk_to_server(ids[i], server3);
	}
}

*/

#include <sadfs/msgs/messages.hpp>
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/client/deserializer.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/msgs/master/message_processor.hpp>
#include <sadfs/msgs/master/serializer.hpp>

#include <iostream>

using namespace sadfs;
constexpr auto delim = std::string_view{"******************************\n"};

void
print_chunk_location_req(msgs::master::chunk_location_request const& req)
{
	std::cout << delim
		<< "Chunk Location Request:"
		<< "\nFilename:     " << req.filename()
		<< "\nChunk number: " << req.chunk_number()
		<< "\n" << delim << "\n";
}

void
print_chunk_location_res(msgs::client::chunk_location_response const& res)
{
	std::cout << delim
		<< "Chunk location request:"
		<< "\nOK:           " << res.ok()
		<< "\nChunk id:     " << to_string(res.chunk_id());
	std::cout << "\n";
	for (auto i = 0; i < res.num_locations(); i++)
	{
		auto service = res.service(i);
		std::cout
		<< "\nService "
		<< i  << ": " << to_string(service.ip())
		<< ':' << to_int(service.port());
	}
	std::cout << "\n" << delim << "\n";
}

auto establish_conn = []() -> msgs::channel
{
	using namespace comm;
	auto&& echod = service{constants::ip_localhost, 6667};
	try
	{
		return msgs::channel{echod.connect()};
	}
	catch (std::system_error ex)
	{
		std::cerr << "[error]: failed to connect to echo server\n"
					<< ex.what() << "\n";
		std::exit(1);
	}
};
auto info = [](auto const& msg)
{
	std::cout << "[INFO]: " << msg << "\n";
};

// some version of this can be put in the client
void 
request_chunk(std::string filename, size_t offset, char rw)
{
	auto type = (rw == 'r') ? msgs::io_type::read : msgs::io_type::write;
	auto fr = msgs::master::chunk_location_request
	{
		type,
		filename,
		offset
	};
	
	auto ch = establish_conn();
	info("connection established with the echo server");

	// send chunk_location_request
	msgs::master::serializer{}.serialize(fr, ch);
	info("sent chunk location request");
	ch.flush();

	auto response = msgs::client::chunk_location_response{};
	msgs::client::deserializer{}.deserialize(response, ch);
	info("received chunk location response");
	print_chunk_location_res(response);

	std::cout << "\n";
}

int
main(int argc, char** argv)
{
	std::cout << std::boolalpha;
	request_chunk("/mnt/a/file.dat", 0, 'r');
	request_chunk("/mnt/a/file.dat", 1, 'w');
	request_chunk("/mnt/a/file.dat", 2, 'r');
	request_chunk("/mnt/a/file.da", 0, 'r');

	return 0;
}
