/* example code for testing sending of protobuf message */
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/msgs/serializers.hpp>

#include <iostream>

using namespace sadfs;

/*
void
recv_id_msg(comm::connection const& conn)
{
	auto echoed_id = comm::msgs::identification{};
	if (!echoed_id.recv(conn))
	{
		std::cerr << "error: could not receive id msg\n";
		std::exit(1);
	}

	std::cout << "\nReceived echoed host identification..."
		<< "\nHost id: " << echoed_id.id()
		<< "\n";
}

void
recv_file_req(comm::connection const& conn)
{
	auto echoed_req = comm::msgs::file_request{};
	if (!echoed_req.recv(conn))
	{
		std::cerr << "error: could not receive file request\n";
		std::exit(1);
	}

	auto const& section = echoed_req.section();
	std::cout << "Received echoed file request..."
		<< "\nSender id: " << echoed_req.sender()
		<< "\nFilename:  " << section.filename
		<< "\nOffset:    " << section.offset
		<< "\nLength:    " << section.length
		<< "\n";
}

void
recv_chunk_req(comm::connection const& conn)
{
	auto echoed_req = comm::msgs::chunk_request{};
	if (!echoed_req.recv(conn))
	{
		std::cerr << "error: could not receive chunk request\n";
		std::exit(1);
	}

	std::cout << "\nReceived echoed chunk request..."
		<< "\nSender id: " << echoed_req.sender()
		<< "\nChunk ID:  " << echoed_req.chunk_id()
		<< "\n";
}

void
recv_msg(comm::connection const& conn)
{
	auto mid = comm::msgs::msg_id{};
	if (!mid.recv(conn))
	{
		std::cerr << "error: could not receive message ID\n";
		std::exit(1);
	}
	switch (mid.type())
	{
		case comm::msgs::msg_type::file:
			recv_file_req(conn);
			break;
		case comm::msgs::msg_type::chunk:
			recv_chunk_req(conn);
			break;
		case comm::msgs::msg_type::id:
		default:
			recv_id_msg(conn);
			break;
	}
}
*/

int
main(int argc, char** argv)
{
	auto fr = msgs::master::file_request
	{
		msgs::io_type::read,
		{"/mnt/a/file.dat", 0, 4096}
	};

	auto cr = msgs::chunk::chunk_request
	{
		msgs::io_type::read,
		78234
	};

	auto ch = msgs::channel
	{
		comm::service{comm::constants::ip_localhost, 6666}.connect()
	};

	auto serializer = msgs::master::serializer{};

	std::cout << "[master]: contacting server...\n";
	serializer.serialize(fr, ch);
	std::cout << "[master]: sent file_request\n";
	std::cout << "[master]: received file_request back\n";

	std::cout << "\n";
	std::cout << "[chunk]: contacting server...\n";
	std::cout << "[chunk]: sent chunk_request\n";
	std::cout << "[chunk]: received chunk_request back\n";

	return 0;
}
