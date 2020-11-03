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
	auto buf = std::array<char, 64>{};

	while (true)
	{
		auto sock = listener.accept();
		std::cout << "accepted connection; fd: " << sock.descriptor() << "\n";

		auto len = 0;
		while ((len = ::read(sock.descriptor(), buf.data(), buf.size())))
		{
			if (len == -1)
			{
				std::cerr << "read error\n";
				std::exit(1);
			}
			std::cout << "received:    " << len << " byte(s)\n";

			if (len != ::write(sock.descriptor(), buf.data(), len))
			{
				std::cerr << "write error\n";
				std::exit(1);
			}
			std::cout << "echoed back: " << len << " byte(s)\n";
			buf.fill({}); // clear buffer
		}
		std::cout << "connection closed\n";
	}
}
