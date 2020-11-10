/* Code for the sadcd class */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/sadcd/sadcd.hpp>

// standard includes
#include <array>
#include <cstring>  // std::strerror
#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h> // read/write

namespace sadfs {
using namespace std::string_view_literals;

void sadcd::
start()
{
	auto listener = sadfs::inet::listener{ip, port};

	while (true)
	{
		auto sock = listener.accept();
		auto result = process_message(sock);

		std::cout << result << "\n";
		// perform some action based on result
	}
}

// reads the message from a socket that just received some data
std::string sadcd::
process_message(sadfs::socket& sock)
{
	auto buf = std::array<char, 512>{};
	auto len = 0;
	auto result = std::string{};
	while ((len = ::read(sock.descriptor(), buf.data(), buf.size())))
	{
		if (len == -1)
		{
			std::cerr << "read error\n";
			std::cerr << std::strerror(errno) << std::endl;
			std::exit(1);
		}
		if (::write(sock.descriptor(), buf.data(), len) == -1)
		{
			std::cerr << "write error\n";
			std::exit(1);
		}
		for (auto i = 0; i < len; i++)
		{
			result += buf[i];
		}
		buf.fill({});
	}

	return result;
}

} // sadfs namespace
