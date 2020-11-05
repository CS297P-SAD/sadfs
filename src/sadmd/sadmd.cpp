/* main process of the Master server */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>

// standard includes
#include <array>
#include <cstring>  // std::strerror
#include <iostream>
#include <unistd.h> // read/write

namespace
{

// struct to hold info needed to set up a chunk server
struct settings
{
	sadfs::inet::port_no port;
	sadfs::inet::ip_addr ip;
};

// returns settings based on command-line arguments
settings
read_command_line(int argc, char *argv[])
{
	auto port = 0;
	auto ip = "";
	for (auto i = 1; i < argc - 1; i ++)
	{
		auto arg = std::string{argv[i]};
		if (arg.compare("--port") == 0)
		{
			i++;
			port = std::stoi(std::string{argv[i]});
		}
		else if (arg.compare("--ipaddress") == 0)
		{
			i++;
			ip = argv[i];
		}
	}

	// TODO: verify that port fits in 16 bits in edge case that this program is
	// started by some method other than sadcd-bootstrap
	// should also double check that an ip and port number were in fact passed
	return settings{static_cast<std::uint16_t>(port), ip};
}

// reads the message from a socket that just received some data
std::string
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

} // unnamed namespace

int
main(int argc, char** argv)
{

	// populate server_settings from command line args
	auto server_settings = read_command_line(argc, argv);

// Put this in a class. master.start()

	// set up a listener for incoming messages
	auto listener = sadfs::inet::listener{server_settings.ip, server_settings.port};

	while (true)
	{
		auto sock = listener.accept();
		auto result = process_message(sock);

		std::cout << result << "\n";
		// perform some action based on result
	}
}