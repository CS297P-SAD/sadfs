#include <sadfs/comm/inet.hpp>

#include <iostream>
#include <array>
#include <unistd.h> //Read/write
#include <cstdint>  // std::uint16_t

using namespace sadfs;

void
parse_args(int argc, char *argv[])
{
	for (auto i = 1; i < argc - 1; i += 2)
	{
		auto arg = std::string{argv[i]};
		auto val = std::string{argv[i + 1]};
		//Set attribute to val basd on arg
	}
}

//Temporary
std::uint16_t
get_port(int argc, char *argv[])
{
	auto port_flag = std::string{"--port"};

	for (auto i = 1; i < argc - 1; i += 2)
	{
		auto arg = std::string{argv[i]};
		auto val = std::string{argv[i + 1]};
		if (port_flag.compare(arg) == 0)
		{
			return static_cast<std::uint16_t>(std::stoi(val));
		}
	}

	return 0;
}

//struct

int 
main(int argc, char *argv[])
{
	auto port = get_port(argc, argv);
	std::cout << port << std::endl;

	auto listener = inet::listener{inet::constants::ip_localhost, port};
	auto buf = std::array<char, 10>{};
	auto len = 0;

	while (true)
	{
		auto sock = listener.accept();

		while ((len = ::read(sock.descriptor(), buf.data(), buf.size() - 1)))
		{
			std::cout << len << " character read\n";
			if (len == -1)
			{
				std::cerr << "read error\n";
				std::cerr << errno << std::strerror(errno) << std::endl;
				std::exit(1);
			}
			if (::write(sock.descriptor(), buf.data(), len) == -1)
			{
				std::cerr << "write error\n";
				std::exit(1);
			}

			std::cout << buf.data();
			buf.fill({});
		}
		std::cout << std::endl;
	}
}
