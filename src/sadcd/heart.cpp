/* implements the chunk::heart class */
// sadfs-specific includes
#include <sadfs/exceptions.hpp>
#include <sadfs/msgs/master/messages.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/sadcd/heart.hpp>

namespace sadfs { namespace chunk {

// starts the heartbeat in a concurrent thread
void heart::
start()
{
	if (stop_token_.valid())
	{
		throw sadfs::logic_error{"heart beating already"};
	}

	// start heartbeat
	stop_token_ = stop_request_.get_future();
	heartbeat_ = std::thread{[this](){ this->beat(); }};
}

// stops the heartbeat
void heart::
stop()
{
	if (!stop_token_.valid())
	{
		throw sadfs::logic_error{"heart not beating"};
	}

	// notify the heartbeat thread that we want it to stop :(
	stop_request_.set_value();
	heartbeat_.join();
}

// heartbeat function
void heart::
beat() const
{
	auto msg          = msgs::master::chunk_server_heartbeat{};
	auto serializer   = msgs::master::serializer{};

	// send heartbeats until requested to stop
	while (true)
	{
		using namespace std::chrono_literals;
		auto status = stop_token_.wait_for(1s);
		if (status == std::future_status::ready)
		{
			// stop requested
			return;
		}

		auto ch = msgs::channel{master_.connect()};
		if (!serializer.serialize(msg, ch))
		{
			throw sadfs::operation_failure{"failed to send heartbeat"};
		}
	}
}

} // chunk namespace
} // sadfs namespace
