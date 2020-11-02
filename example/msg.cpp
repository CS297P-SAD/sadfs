#include <comm/inet.hpp>

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

	auto sock = sadfs::socket(socket::domain::inet, socket::type::stream);
	auto addr = sockaddr_in{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6666);
	addr.sin_addr = {inet::constants::ip_localhost.value()};
	if (connect(sock.descriptor(),
	            reinterpret_cast<sockaddr const*>(&addr),
	            sizeof(addr)) == -1)
	{
		return -1;
	}

	char const* msg{(argc > 1) ? argv[1] : "hi, there"};
	if (send(sock.descriptor(), msg, strlen(msg), 0) == -1)
	{
		return -2;
	}

	auto buf = std::array<char, 128>{};
	if (recv(sock.descriptor(), buf.data(), buf.size(), 0) == -1)
	{
		return -3;
	}
	std::cout << buf.data() << '\n';

	return 0;
}
