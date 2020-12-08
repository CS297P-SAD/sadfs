/* implements the chunk::heart class */
// sadfs-specific includes
#include <sadfs/exceptions.hpp>
#include <sadfs/sadcd/heart.hpp>

namespace sadfs { namespace chunk {

// starts the heartbeat in a concurrent thread
void heart::
start()
{
	if (heartbeat_.joinable())
	{
		throw sadfs::logical_error{"heart beating already"};
	}

	// start heartbeat
	heartbeat_ = std::jthread{beat};
}

// stops the heartbeat
void heart::
stop()
{
	if (!heartbeat_.joinable())
	{
		throw sadfs::logical_error{"heart not beating"};
	}

	// discard the boolean return value since only one thread
	// will ever request a stop
	(void) heartbeat_.request_stop();
	heartbeat_.join();
}

// heartbeat function
void heart::
beat(std::stop_token stoken)
{
	auto msg          = msgs::master::chunk_server_heartbeat{};
	auto serializer   = msgs::master::serializer{};

	// until requested to stop;
	// unsigned char wraps back to zero in a well-defined way
	for (unsigned char i = 0U; !stoken.stop_requested(); i++)
	{
		using namespace std::chrono_literals;
		constexpr auto sleep_duration = 1s;
		std::this_thread::sleep_for(sleep_duration);

		// send a heartbeat only every 5th iteration.
		// we do this so that when a stop is requested, the requester
		// is blocked only for upto 1/5th of the duration of a heartbeat
		if ((i % 5) != 0) { continue; }

		auto ch = msgs::channel{master_.connect()};
		if (!serializer.serialize(msg, ch))
		{
			throw sadfs::operation_failure{"failed to send heartbeat"};
		}

	}
}

} // chunk namespace
} // sadfs namespace
