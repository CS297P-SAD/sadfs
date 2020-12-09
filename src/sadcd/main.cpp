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
	const char* master_ip;
	int         master_port;
	const char* server_id;
};

// returns settings based on command-line arguments
settings
read_command_line(int argc, char *argv[])
{
	constexpr auto port_switch = "--port"sv;
	constexpr auto ip_switch = "--ipaddress"sv;
	constexpr auto master_port_switch = "--master_port"sv;
	constexpr auto master_ip_switch = "--master_ipaddress"sv;
	constexpr auto serverid_switch = "--serverid"sv;
	auto port = "";
	auto ip   = "";	
	auto master_port = "";
	auto master_ip   = "";
	auto server_id = "";
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
		else if (master_port_switch == argv[i])
		{
			i++;
			master_port = argv[i];
			continue;
		}
		else if (master_ip_switch == argv[i])
		{
			i++;
			master_ip = argv[i];
		}
		else if (serverid_switch == argv[i])
		{
			i++;
			server_id = argv[i];
		}
	}

	return {ip, std::stoi(port), master_ip, std::stoi(master_port), server_id};
}

} // sadfs namespace

int
main(int argc, char** argv)
{
	using namespace sadfs;
	// populate server_settings from command line args
	auto server_settings = read_command_line(argc, argv);
	auto server = sadcd{server_settings.ip, 
	                    server_settings.port, 
	                    server_settings.master_ip, 
	                    server_settings.master_port, 
	                    server_settings.server_id}; 

	server.start();
}
