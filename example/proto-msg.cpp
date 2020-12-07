/* example code for testing sending of protobuf message */
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/msgs/deserializers.hpp>
#include <sadfs/msgs/serializers.hpp>
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
	std::cout << std::boolalpha;
	auto fr = msgs::master::chunk_location_request
	{
		msgs::io_type::read,
		"/mnt/a/file.dat",
		6
	};
	print_chunk_location_req(fr);

	auto cr = msgs::chunk::chunk_request
	{
		msgs::io_type::read,
		78234
	};
	print_chunk_req(cr);

	auto clr = msgs::client::chunk_location_response
	{
		true,
		{{"10.0.0.13", 6666}},
		uuid::generate(),
	};
	print_chunk_location_res(clr);

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

	auto ch = establish_conn();
	info("connection established with the echo server");

	// send chunk_location_request
	msgs::master::serializer{}.serialize(fr, ch);
	info("sent chunk location request");

	// send chunk_request
	msgs::chunk::serializer{}.serialize(cr, ch);
	info("sent chunk request");

	// send chunk_location_response
	msgs::client::serializer{}.serialize(clr, ch);
	info("sent chunk location response");

	// to make sure that the serialized msgs are sent,
	// flush the channel
	ch.flush();

	std::cout << "\n";
	// receive chunk_location_request
	auto new_fr = msgs::master::chunk_location_request{};
	msgs::master::deserializer{}.deserialize(new_fr, ch);
	info("received chunk location request");
	print_chunk_location_req(new_fr);

	// receive chunk_request
	auto new_cr = msgs::chunk::chunk_request{};
	msgs::chunk::deserializer{}.deserialize(new_cr, ch);
	info("received chunk request");
	print_chunk_req(new_cr);

	// receive chunk_location_response
	auto new_clr = msgs::client::chunk_location_response{};
	msgs::client::deserializer{}.deserialize(new_clr, ch);
	info("received chunk location response");
	print_chunk_location_res(new_clr);

	return 0;
}
