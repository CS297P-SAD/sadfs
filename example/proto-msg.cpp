/* example code for testing sending of protobuf message */
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/messages.hpp>
#include <sadfs/proto/messages.pb.h>

#include <array>
#include <cstring>
#include <iostream>

#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace sadfs;
namespace gpio = google::protobuf::io;

void
recv_id_msg(gpio::ZeroCopyInputStream* in)
{
	auto echoed_id = comm::msgs::identification{};
	if (!echoed_id.recv(in))
	{
		std::cerr << "error: wrong message length received\n";
		std::exit(1);
	}

	std::cout << "\nReceived echoed host identification..."
		<< "\nHost id: " << echoed_id.id()
		<< "\n";
}

void
recv_file_req(gpio::ZeroCopyInputStream* in)
{
	auto echoed_req = comm::msgs::file_request{};
	if (!echoed_req.recv(in))
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

void
recv_chunk_req(gpio::ZeroCopyInputStream* in)
{
	auto echoed_req = comm::msgs::chunk_request{};
	if (!echoed_req.recv(in))
	{
		std::cerr << "error: wrong message length received\n";
		std::exit(1);
	}

	std::cout << "\nReceived echoed chunk request..."
		<< "\nSender id: " << echoed_req.sender()
		<< "\nChunk ID:  " << echoed_req.chunk_id()
		<< "\n";
}

void
recv_msg(gpio::ZeroCopyInputStream* in)
{
	auto mid = comm::msgs::msg_id{};
	if (!mid.recv(in))
	{
		std::cerr << "error: could not receive message ID\n";
		std::exit(1);
	}
	switch (mid.type())
	{
		case comm::msgs::msg_type::file:
			recv_file_req(in);
			break;
		case comm::msgs::msg_type::chunk:
			recv_chunk_req(in);
			break;
		case comm::msgs::msg_type::id:
		default:
			recv_id_msg(in);
			break;
	}
}

int
main(int argc, char** argv)
{
	auto sock = inet::service{inet::constants::ip_localhost, 6666}.connect();
	auto in = gpio::FileInputStream(sock.descriptor());
	{
		auto req = comm::msgs::file_request
		{
			13,
			comm::msgs::io_type::read,
			{"/mnt/a/file.dat", 0, 4096}
		};
		if (!req.send(sock))
		{
			std::cerr << "error: could not send message\n";
			std::exit(1);
		}
	}
	{
		auto req = comm::msgs::chunk_request
		{
			13,
			comm::msgs::io_type::read,
			78234
		};
		if (!req.send(sock))
		{
			std::cerr << "error: could not send message\n";
			std::exit(1);
		}
	}
	{
		auto id = comm::msgs::identification
		{
			comm::msgs::host_type::client,
			13,
		};
		if (!id.send(sock))
		{
			std::cerr << "error: could not send message\n";
			std::exit(1);
		}
	}
	recv_msg(&in);
	recv_msg(&in);
	recv_msg(&in);
	return 0;
}
