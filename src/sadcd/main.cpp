/* main process for the chunk server */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/sadcd/sadcd.hpp>

// standard includes
#include <array>
#include <cstring>  // std::strerror
#include <iostream>
#include <string>   // std::stoi
#include <string_view>
#include <unistd.h> // read/write

namespace sadfs {
using namespace std::string_view_literals;

// struct to hold info needed to set up a server
struct settings
{
	const char* ip;
	int         port;
};

// returns settings based on command-line arguments
settings
read_command_line(int argc, char *argv[])
{
	constexpr auto port_switch = "--port"sv;
	constexpr auto ip_switch = "--ipaddress"sv;
	auto port = "";
	auto ip   = "";
	for (auto i = 1; i < argc - 1; i ++)
	{
		if (port_switch == argv[i])
		{
			i++;
			port = argv[i];
			continue;
		}
		else if (ip_switch == argv[i])
		{
			i++;
			ip = argv[i];
		}
	}

	return {ip, std::stoi(port)};
}

} // sadfs namespace

int
main(int argc, char** argv)
{
	using namespace sadfs;
	// populate server_settings from command line args
	auto server_settings = read_command_line(argc, argv);

	auto server = sadcd{server_settings.ip, server_settings.port};

	server.start();
}
