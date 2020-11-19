/* example code for testing sending of protobuf message */
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/messages.hpp>
#include <sadfs/proto/messages.pb.h>

#include <arpa/inet.h>
#include <array>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int
main(int argc, char** argv)
{
	using namespace sadfs;

	auto sock = inet::service{inet::constants::ip_localhost, 6666}.connect();
	{
		auto req = comm::file_request
		{
			13,
			comm::request_type::read,
			{"/mnt/a/file.dat", 0, 4096}
		};
		if (!req.send(sock))
		{
			std::cerr << "error: could not send message\n";
			std::exit(1);
		}

		auto echoed_req = comm::file_request{};
		if (!echoed_req.recv(sock))
		{
			std::cerr << "error: wrong message length received\n";
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

	{
		auto req = comm::chunk_request
		{
			13,
			comm::request_type::read,
			78234
		};
		if (!req.send(sock))
		{
			std::cerr << "error: could not send message\n";
			std::exit(1);
		}

		auto echoed_req = comm::chunk_request{};
		if (!echoed_req.recv(sock))
		{
			std::cerr << "error: wrong message length received\n";
			std::exit(1);
		}

		std::cout << "\nReceived echoed chunk request..."
			<< "\nSender id: " << echoed_req.sender()
			<< "\nChunk ID:  " << echoed_req.chunk_id()
			<< "\n";
	}

	return 0;
}
