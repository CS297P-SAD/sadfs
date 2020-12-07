/* example code for testing sending of protobuf message */
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/client/deserializer.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/msgs/master/message_processor.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/uuid.hpp>

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
	auto service = res.service();
	std::cout << delim
		<< "Chunk Location Response:"
		<< "\nOK:       " << res.ok()
		<< "\nIP:       " << to_string(service.ip())
		<< "\nPort:     " << to_int(service.port())
		<< "\nChunk id: " << to_string(res.chunk_id())
		<< "\nPayload   " << res.payload()
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
	auto fr = msgs::master::chunk_location_request
	{
		msgs::io_type::read,
		"/mnt/a/file.dat",
		6
	};
	print_chunk_location_req(fr);

	/*
	auto cr = msgs::chunk::chunk_request
	{
		msgs::io_type::read,
		78234
	};
	print_chunk_req(cr);
	*/

	auto establish_conn = []() -> msgs::channel
	{
		auto&& service = comm::service{comm::constants::ip_localhost, 6666};
		try
		{
			return msgs::channel{service.connect()};
		}
		catch (std::system_error ex)
		{
			std::cerr << "[error]: failed to connect to the server\n"
			          << ex.what() << "\n";
			std::exit(1);
		}
	};
	auto info = [](auto const& msg)
	{
		std::cout << "[INFO]: " << msg << "\n";
	};

	auto ch = establish_conn();
	info("connection established with the server");

	// send chunk_location_request
	msgs::master::serializer{}.serialize(fr, ch);
	info("sent chunk location request");

	// send chunk_location_request a second time to test
	// deserialization
	msgs::master::serializer{}.serialize(fr, ch);
	info("sent chunk location request again");

	// to make sure that the serialized msgs are sent,
	// flush the output buffer
	ch.flush();

	std::cout << "\n";
	// should receive 2 chunk_location_response s
	auto new_clr1 = msgs::client::chunk_location_response{};
	msgs::client::deserializer{}.deserialize(new_clr1, ch);
	info("received chunk location response");
	print_chunk_location_res(new_clr1);

	auto new_clr2 = msgs::client::chunk_location_response{};
	msgs::client::deserializer{}.deserialize(new_clr2, ch);
	info("received chunk location response");
	print_chunk_location_res(new_clr2);

	// receive chunk_request
	/*
	auto new_cr = msgs::chunk::chunk_request{};
	msgs::chunk::deserializer{}.deserialize(new_cr, ch);
	info("received chunk request");
	print_chunk_req(new_cr);
	*/

	return 0;
}
