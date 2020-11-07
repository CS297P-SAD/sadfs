/* main process for the master server */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/sadmd/sadmd.hpp>

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
	sadfs::inet::port_no port;
	const char* ip;
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
	return settings{static_cast<std::uint16_t>(port), std::string(ip).c_str()};
}

} // unnamed namespace

int
main(int argc, char** argv)
{
    // populate server_settings from command line args
    auto server_settings = read_command_line(argc, argv);

    auto master = sadmd{server_settings.port, server_settings.ip};
    
    master.start();

}
