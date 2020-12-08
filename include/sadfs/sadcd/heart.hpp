#ifndef SADFS_SADCD_HEART_HPP
#define SADFS_SADCD_HEART_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp> // comm::service

// standard includes
#include <thread>

namespace sadfs { namespace chunk {

// sends heartbeats to the master server
class heart
{
	comm::service master_;
	std::jthread  heartbeat_;

	void beat(std::stop_token) const;

public:
	heart(comm::service const& master) : master_{master} { }

	void start();
	void stop();
};

} // chunk namespace
} // sadfs namespace

#endif // SADFS_SADCD_HEART_HPP
