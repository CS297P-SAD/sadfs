/* Code for the sadcd class */

// sadfs-specific includes
#include <sadfs/sadcd/heart.hpp>
#include <sadfs/sadcd/sadcd.hpp>
#include <sadfs/comm/inet.hpp>

// standard includes
#include <array>
#include <chrono>
#include <cstring>  // std::strerror
#include <future>   // std::promise, std::future
#include <iostream>
#include <string>
#include <unistd.h> // read/write

namespace sadfs {

namespace {

using namespace std::chrono_literals;
auto ready = [](auto const& future, auto duration) { return future.wait_for(duration) == std::future_status::ready; };

} // unnamed namespace

sadcd::
sadcd(char const* ip, int port) : master_(ip, port)
{
	// do nothing
}

void sadcd::
start()
{
	// set up
	run();
}

void sadcd::
run()
{
	// kill_switch is a switch we flip to kill the main service thread
	// *_death are "flags" child threads will use to indicate death
	auto kill_switch = std::promise<void>{};
	auto main_thread_dead = std::future<void>{};
	auto main_thread = std::thread{};
	{
		// grab future that will indicate when the main thread dies
		// due to an error
		auto death = std::promise<void>{};
		main_thread_dead = death.get_future();

		// start main thread
		main_thread = std::thread
		{
			&sadcd::start_main,
			this,
			std::move(death),
			kill_switch.get_future()
		};
	}

	// start heartbeat
	auto heart = chunk::heart{master_};
	heart.start();

	// poll for failures
	while (!ready(main_thread_dead, 1s) && heart.beating())
	{
		// everything fine; do nothing
	}

	// something went wrong; engage the kill switch
	kill_switch.set_value();
	main_thread.join();
	if (heart.beating())
	{
		heart.stop();
	}
}

void sadcd::
start_main(std::promise<void> death, std::shared_future<void> kill_switch)
{
	while (!ready(kill_switch, 1s))
	{
		std::cout << "[INFO]: chunk server working...\n";
	}
	death.set_value(); // unnecessary, but doing this so we don't forget
}

} // sadfs namespace
