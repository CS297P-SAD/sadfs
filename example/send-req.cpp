/* example code for testing sending of protobuf message */
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/msgs/deserializers.hpp>
#include <sadfs/msgs/serializers.hpp>

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
print_chunk_req(msgs::chunk::chunk_request const& req)
{
	std::cout << delim
		<< "Chunk Request:"
		<< "\nChunk ID:  " << req.chunk_id()
		<< "\n" << delim << "\n";
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

void 
request_chunk(std::string filename, size_t offset)
{
	auto fr = msgs::master::chunk_location_request
	{
		msgs::io_type::read,
		filename,
		offset
	};
	
	auto ch = establish_conn();
	info("connection established with the echo server");

	// send chunk_location_request
	msgs::master::serializer{}.serialize(fr, ch);
	info("sent chunk location request");
	ch.flush();

	auto response = msgs::chunk::chunk_request{};
	msgs::chunk::deserializer{}.deserialize(response, ch);
	info("received response: " + std::to_string(response.chunk_id()));

	std::cout << "\n";
}

int
main(int argc, char** argv)
{

	request_chunk("/mnt/a/file.dat", 0);
	request_chunk("/mnt/a/file.dat", 1);
	request_chunk("/mnt/a/file.dat", 2);
	request_chunk("/mnt/a/file.da", 0);

	return 0;
}
