/* implements the chunk::heart class */
// sadfs-specific includes
#include <sadfs/exceptions.hpp>
#include <sadfs/msgs/master/messages.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/sadcd/heart.hpp>

namespace sadfs { namespace chunk {

namespace {

// heartbeat function
void
beat(comm::service master, std::promise<void> death, std::future<void> stop_token)
{
	auto msg          = msgs::master::chunk_server_heartbeat{};
	auto serializer   = msgs::master::serializer{};
	auto die = [&death]() { return death.set_value(); };

	// send heartbeats until requested to stop
	using namespace std::chrono_literals;
	while (stop_token.wait_for(1s) != std::future_status::ready)
	{
		try
		{
			auto ch = msgs::channel{master.connect()};
			if (serializer.serialize(msg, ch)) { continue; }
			throw sadfs::operation_failure{"could not send heartbeat"};
		}
		catch (std::exception const&)
		{
			return die();
		}
	}

	// stop requested
	stop_token.get();
	die();
}

} // unnamed namespace

// starts the heartbeat in a concurrent thread
void heart::
start()
{
	if (beating())
	{
		throw sadfs::logic_error{"heart beating already"};
	}

	auto death = std::promise<void>{};
	stop_request_ = std::promise<void>{};
	stopped_ = death.get_future();

	heartbeat_ = std::thread
	{
		beat,
		master_,
		std::move(death),
		stop_request_.get_future()
	};
}

// stops sending heartbeats
void heart::
stop()
{
	if (!beating())
	{
		throw sadfs::logic_error{"heart not beating"};
	}

	// notify the heartbeat thread that we want it to stop :(
	stop_request_.set_value();
	heartbeat_.join();
	stopped_.get();
}

} // chunk namespace
} // sadfs namespace
