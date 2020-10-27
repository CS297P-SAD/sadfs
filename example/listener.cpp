/* example code for testing sadfs::inet::listener */
#include <sadfs/comm/inet.hpp>

#include <array>
#include <iostream>
#include <unistd.h>

int
main()
{
	using namespace sadfs;
	auto listener = inet::listener{inet::constants::ip_localhost, 6666};
	auto sock = listener.accept();
	auto buf = std::array<char, 512>{};

	::read(sock.descriptor(), buf.data(), buf.size());
	std::cout << "accepted: " << sock.descriptor() << '\n'
	          << buf.data() << '\n';
}
