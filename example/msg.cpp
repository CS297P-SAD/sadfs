/* example code for testing outgoing connections */
#include <sadfs/comm/inet.hpp>

#include <arpa/inet.h>
#include <array>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

ssize_t
len(char const* msg)
{
	constexpr auto max_len = 256;
	auto len = std::strlen(msg);
	if (len > max_len)
	{
		std::cerr << "msg length " << len << " exceeds limit of "
		          << max_len << "\n";
		std::exit(1);
	}
	return len;
}

int
main(int argc, char** argv)
{
	using namespace sadfs;

	if (argc > 2)
	{
		std::cout << "Usage: " << argv[0] << " [MSG]\n";
		return 1;
	}

	auto const& msg = (argc == 2) ? argv[1] : "hi, there";
	auto const msg_len = len(msg);

	auto sock = sadfs::socket(socket::domain::inet, socket::type::stream);
	auto addr = sockaddr_in{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6666);
	addr.sin_addr = {inet::constants::ip_localhost.value()};
	if (connect(sock.descriptor(),
	            reinterpret_cast<sockaddr const*>(&addr),
	            sizeof(addr)) == -1)
	{
		std::cerr << "error: could not connect to echo server\n";
		std::exit(1);
	}

	if (send(sock.descriptor(), msg, msg_len, 0) != msg_len)
	{
		std::cerr << "error: could not send message\n";
		std::exit(1);
	}

	auto buf = std::array<char, 64>{};
	auto read_len = decltype(msg_len){0};
	while (read_len < msg_len) // msg yet to be fully received
	{
		auto recv_len = recv(sock.descriptor(),
		                     buf.data(), buf.size(), 0);
		if (recv_len == -1)
		{
			std::cerr << "error: could not receive echoed message\n";
			std::exit(1);
		}

		read_len += recv_len;
		for (auto i = 0; i < recv_len; i++)
		{
			std::cout << buf[i];
		}
		buf.fill({}); // clear buffer for next read
	}
	std::cout << "\n";

	return 0;
}
