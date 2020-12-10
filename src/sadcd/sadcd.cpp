/* Code for the sadcd class */

// sadfs-specific includes
#include <sadfs/sadcd/sadcd.hpp>
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/master/serializer.hpp>

// standard includes
#include <array>
#include <cstring>  // std::strerror
#include <iostream>
#include <string>
#include <unistd.h> // read/write

namespace sadfs {

namespace constants {

constexpr auto max_chunks = 1000;

} // (local) constants namespace

sadcd::
sadcd(char const* ip, int port, 
      char const* master_ip, int master_port, 
      char const* server_id) : 
      service_(ip, port) , 
      master_(master_ip, master_port),
      serverid_(serverid::from_string(server_id))
{
	// do nothing
}

void sadcd::
start()
{
	join_network();
	auto listener = comm::listener{service_};

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
process_message(comm::socket const& sock)
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

bool sadcd::
join_network()
{
	/*
	//auto ch = channel{socket{domain::inet, type::stream, -1}}; // dummy channel
	try
	{
		ch = msgs::channel{master_.connect()}
	}
	catch
	{
		// log error and return false
		return false;
	}
	TODO: Replace next line  with above ^ once dummy channel returns !is_open()
	*/

	auto ch = msgs::channel{master_.connect()};

	auto jr = msgs::master::join_network_request
	{
		serverid_,
		service_,
		constants::max_chunks,
		/*chunk_count=*/ 0
	};

	// send join_network_request
	msgs::master::serializer{}.serialize(jr, ch);
	return true;

}

} // sadfs namespace
