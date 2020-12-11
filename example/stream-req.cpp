/* example code for testing data streaming */
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/client/deserializer.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/msgs/chunk/serializer.hpp>
#include <sadfs/msgs/chunk/deserializer.hpp>
#include <sadfs/uuid.hpp>

#include <iostream>

using namespace sadfs;
constexpr auto delim = std::string_view{"******************************\n"};

void
print_stream_req(msgs::chunk::stream_request const& req)
{
	std::cout << delim
		<< "Stream Request:"
		<< "\nChunk ID:  " << to_string(req.chunk_id())
		<< "\nOffset:    " << req.offset()
		<< "\nLength:    " << req.length()
		<< "\nData:\n    " << req.data()
		<< "\n" << delim << "\n";
}

int
main(int argc, char** argv)
{
	std::cout << std::boolalpha;

	auto data = std::string{
		"This is a long string that is supposed\n"
		"to be the data we stream to a chunk server"};
	auto sr = msgs::chunk::stream_request
	{
		msgs::io_type::write,
		chunkid::generate(),
		1024,
		data.size(),
		std::move(data)
	};
	print_stream_req(sr);

	auto service = comm::service{comm::constants::ip_localhost, 6666};
	auto sock = service.connect();
	if (!sock.valid())
	{
		logger::error("failed to establish a connection "
				"with the chunk server"sv);
		return 1;
	}
	auto ch = msgs::channel{std::move(sock)};

	logger::info("connection established with the server"sv);

	msgs::chunk::serializer{}.serialize(sr, ch);
	logger::info("sent write stream request"sv);

	// to make sure that the serialized msgs are sent,
	// flush the output buffer
	ch.flush();

	auto sr_recv = msgs::chunk::stream_request{};
	msgs::chunk::deserializer{}.deserialize(sr_recv, ch);
	print_stream_req(sr_recv);

	return 0;
}
