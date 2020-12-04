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
print_file_req(msgs::master::file_request const& req)
{
	auto const& section = req.section();
	std::cout << delim
		<< "File Request:"
		<< "\nFilename:  " << section.filename
		<< "\nOffset:    " << section.offset
		<< "\nLength:    " << section.length
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

int
main(int argc, char** argv)
{
	auto fr = msgs::master::file_request
	{
		msgs::io_type::read,
		{"/mnt/a/file.dat", 0, 4096}
	};
	print_file_req(fr);

	auto cr = msgs::chunk::chunk_request
	{
		msgs::io_type::read,
		78234
	};
	print_chunk_req(cr);

	auto establish_conn = []() -> msgs::channel
	{
		using namespace comm;
		auto&& echod = service{constants::ip_localhost, 6666};
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
	auto info = [](auto const& msg)
	{
		std::cout << "[INFO]: " << msg << "\n";
	};

	auto ch = establish_conn();
	info("connection established with the echo server");

	auto ch = establish_conn();
	info("connection established with the echo server");

	// send file_request
	msgs::master::serializer{}.serialize(fr, ch);
	info("sent file request");

	// send chunk_request
	msgs::chunk::serializer{}.serialize(cr, ch);
	info("sent chunk request");

	// to make sure that the serialized msgs are sent,
	// flush the channel
	ch.flush();

	// send chunk_request
	msgs::chunk::serializer{}.serialize(cr, ch);
	info("sent chunk request");

	// to make sure that the serialized msgs are sent,
	// flush the channel
	ch.flush();

	std::cout << "\n";
	// receive file_request
	auto new_fr = msgs::master::file_request{};
	msgs::master::deserializer{}.deserialize(new_fr, ch);
	info("received file request");
	print_file_req(new_fr);

	// receive chunk_request
	auto new_cr = msgs::chunk::chunk_request{};
	msgs::chunk::deserializer{}.deserialize(new_cr, ch);
	info("received chunk request");
	print_chunk_req(new_cr);

	return 0;
}
