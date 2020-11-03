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
	auto buf = std::array<char, 512>{};
	auto len = 0;

	while (true)
	{
		auto sock = listener.accept();
		std::cout << "accepted: " << sock.descriptor() << "\n";

		while ((len = ::read(sock.descriptor(), buf.data(), buf.size())))
		{
			if (len == -1)
			{
				std::cerr << "read error\n";
				std::exit(1);
			}
			if (::write(sock.descriptor(), buf.data(), len) == -1)
			{
				std::cerr << "write error\n";
				std::exit(1);
			}

			std::cout << buf.data() << "\n";
			buf.fill({});
		}
	}
}
