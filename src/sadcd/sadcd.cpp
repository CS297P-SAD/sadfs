/* This creates the binary for the chunk server daemon, which should only be
 * be started by the sadcd-bootstrap process. It opens up a listener on an ip
 * and port specified by the command line to handle incoming requests.
 */

#include <sadfs/comm/inet.hpp>

#include <iostream>
#include <array>
#include <unistd.h> //Read/write
#include <cstdint>  // std::uint16_t

using namespace sadfs;

//struct to hold info needed to set up a chunk server
struct chunk_server_settings
{
	std::uint16_t port;
	sadfs::inet::ip_addr ip;
	void from_command_line(int, char**);
};

//Sets struct variables basd on command line arguments passed
void chunk_server_settings::
from_command_line(int argc, char *argv[])
{
	for (auto i = 1; i < argc - 1; i ++)
	{
		auto arg = std::string{argv[i]};
		if (arg.compare("--port") == 0)
		{
			i++;
			this->port = static_cast<std::uint16_t>(std::stoi(std::string{argv[i]}));
		}
		else if (arg.compare("--ipaddress") == 0)
		{
			i++;
			this->ip.set(argv[i]);
		}
	}
}

//Reads the message from a socket that just received some data
std::string
process_message(sadfs::socket* sock)
{
	auto buf = std::array<char, 512>{};
	auto len = 0;
	auto result = std::string{};
	while ((len = ::read(sock->descriptor(), buf.data(), buf.size())))
	{
		if (len == -1)
		{
			std::cerr << "read error\n";
			std::cerr << std::strerror(errno) << std::endl;
			std::exit(1);
		}
		if (::write(sock->descriptor(), buf.data(), len) == -1)
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

int 
main(int argc, char *argv[])
{

	//Populate chunk_server with info from command line args
	auto chunk_server = chunk_server_settings{};
	chunk_server.from_command_line(argc, argv);

	//Set up a listener for incoming messages
	auto listener = inet::listener{chunk_server.ip, chunk_server.port};

	while (true)
	{
		auto sock = listener.accept();
		auto result = process_message(&sock);

		std::cout << result << std::endl;
		// perform some action based on result
	}
}
