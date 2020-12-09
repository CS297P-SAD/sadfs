/* dummy implementation of sadcd that sends heartbeats for 5 seconds */
#include <sadfs/comm/inet.hpp>
#include <sadfs/sadcd/heart.hpp>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

auto info = [](std::string msg) { std::cout << "[INFO]: " << msg << "\n"; };

using namespace sadfs;
struct sadcd
{
	void run(comm::service const& master)
	{
		auto heart = chunk::heart{master};
		info("starting heartbeat...");
		heart.start();
		info("started");

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5s);

		info("stopping heartbeat...");
		heart.stop();
		info("stopped");
	}
};

int
main()
{
	sadcd{}.run({comm::constants::ip_localhost, 6666});
}
