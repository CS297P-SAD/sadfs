/* main process for the chunk server */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/sadcd/sadcd.hpp>

// standard includes
#include <array>
#include <cstring>  // std::strerror
#include <iostream>
#include <unistd.h> // read/write

namespace
{
// struct to hold info needed to set up a server
struct settings
{
	const char* ip;
	sadfs::inet::port_no port;
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
	return settings{ip, static_cast<std::uint16_t>(port)};
}

} // unnamed namespace

int
main(int argc, char** argv)
{
    // populate server_settings from command line args
    auto server_settings = read_command_line(argc, argv);

    auto master = sadcd{server_settings.ip, server_settings.port};
    
    master.start();

}
