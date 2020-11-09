/* example code for testing sending of protobuf message */
#include <sadfs/comm/inet.hpp>
#include <sadfs/comm/messages.pb.h>

#include <arpa/inet.h>
#include <array>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/util/delimited_message_util.h>

int
main(int argc, char** argv)
{
	using namespace sadfs;
	namespace gpio = google::protobuf::io;
	namespace gputil = google::protobuf::util;

	auto sock = sadfs::socket(socket::domain::inet, socket::type::stream);
	auto addr = sockaddr_in{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6666);
	addr.sin_addr = {inet::ip_addr{inet::constants::ip_localhost}.value()};
	if (connect(sock.descriptor(),
	            reinterpret_cast<sockaddr const*>(&addr),
	            sizeof(addr)) == -1)
	{
		std::cerr << "error: could not connect to echo server\n";
		std::exit(1);
	}

	auto req = comm::request{};
	req.set_type(comm::request::READ);
	req.set_sender(13);
	req.set_filename("/home/user/data/file.dat");
	req.set_offset(4096);
	req.set_length(512);
	{
		auto out = gpio::FileOutputStream(sock.descriptor());
		if (!gputil::SerializeDelimitedToZeroCopyStream(req, &out))
		{
			std::cerr << "error: could not send message\n";
			std::exit(1);
		}
	}

	auto echoed_req = comm::request{};
	{
		auto in = gpio::FileInputStream(sock.descriptor());
		if (!gputil::ParseDelimitedFromZeroCopyStream(
				&echoed_req, &in, nullptr))
		{
			std::cerr << "error: wrong message length received\n";
			std::exit(1);
		}
	}
	std::cout << "Received echoed request..."
	          << "\nSender id: " << echoed_req.sender()
	          << "\nFilename:  " << echoed_req.filename()
	          << "\nOffset:    " << echoed_req.offset()
	          << "\nLength:    " << echoed_req.length()
	          << "\n";

	return 0;
}
