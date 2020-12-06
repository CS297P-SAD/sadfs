/* dummy implementation of sadmd that responds favourably to all messages */
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/client/messages.hpp>
#include <sadfs/msgs/client/serializer.hpp>
#include <sadfs/msgs/master/deserializer.hpp>
#include <sadfs/msgs/master/message_processor.hpp>
#include <sadfs/uuid.hpp>

#include <iostream>

auto info = [](auto msg) { std::cout << "[INFO]: " << msg << "\n"; };

using namespace sadfs;
struct sadmd
{
	bool start()
	{
		auto listener = comm::listener{{comm::constants::ip_localhost, 6666}};
		while (true)
		{
			info("waiting for connections...");
			auto sock = listener.accept();
			info("accepted connection");
			serve_requests(std::move(sock));
		}
	}

	void serve_requests(msgs::channel ch)
	{
		auto processor = msgs::master::processor{};
		bool result{false}, eof{false};
		while (true)
		{
			auto [result, eof] = processor.process_next(ch, *this);
			if (result)
			{
				info("request served successfully");
			}
			else if (eof)
			{
				info("EOF");
				break;
			}
			else
			{
				info("request service failed");
			}
		}
	}

	using clr = msgs::master::chunk_location_request;
	bool handle(clr const& req, msgs::channel const& ch)
	{
		auto response = msgs::client::chunk_location_response
		{
			/*ok=*/true,
			{"10.0.0.13", 6666},
			uuid::generate(),
			"secret payload"
		};
		auto result = msgs::client::serializer{}.serialize(response, ch);
		ch.flush();
		return result;
	}
};

int
main()
{
	return sadmd{}.start();
}
