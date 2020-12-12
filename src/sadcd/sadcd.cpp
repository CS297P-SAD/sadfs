/* Code for the sadcd class */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/chunk/message_processor.hpp>
#include <sadfs/msgs/client/messages.hpp>
#include <sadfs/msgs/client/serializer.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/sadcd/heart.hpp>
#include <sadfs/sadcd/sadcd.hpp>

// standard includes
#include <chrono>  // std::chrono_literals
#include <fstream> // std::ifstream, std::ofstream
#include <future>  // std::promise, std::future
#include <utility> // std::pair

namespace sadfs
{

namespace
{

using namespace std::chrono_literals;
auto ready = [](auto const& future, auto duration) {
    return future.wait_for(duration) == std::future_status::ready;
};

namespace limits
{
constexpr auto requests_per_conn = 3U;
} // namespace limits

} // unnamed namespace

namespace constants
{

constexpr auto max_chunks = 1000;

} // namespace constants

// ==================================================================
//                                sadcd
// ==================================================================
sadcd::sadcd(char const* ip, int port, char const* master_ip, int master_port,
             char const* server_id)
    : service_(ip, port), master_(master_ip, master_port),
      serverid_(serverid::from_string(server_id))
{
    // do nothing
}

void
sadcd::start()
{
    join_network();
    // set up
    run();
}

void
sadcd::run()
{
    // kill_switch is a switch we flip to kill the main service thread
    // *_death are "flags" child threads will use to indicate death
    auto kill_switch      = std::promise<void>{};
    auto main_thread_dead = std::future<void>{};
    auto main_thread      = std::thread{};
    {
        // grab future that will indicate when the main thread dies
        // due to an error
        auto death       = std::promise<void>{};
        main_thread_dead = death.get_future();

        // start main thread
        main_thread = std::thread{&sadcd::start_main, this, std::move(death),
                                  kill_switch.get_future()};
    }

    // start heartbeat
    auto heart = chunk::heart{master_, serverid_};
    heart.start();

    // poll for failures
    while (true)
    {
        if (ready(main_thread_dead, 1s))
        {
            // get threads to propagate error condition using futures
            logger::error("main thread has died"sv);
            main_thread_dead.get();
            heart.stop();
            break;
        }
        if (!heart.beating())
        {
            logger::error("heartbeats have stopped"sv);
            kill_switch.set_value();
            main_thread.join();
            break;
        }
    }
}

void
sadcd::start_main(std::promise<void>       death,
                  std::shared_future<void> kill_switch)
{
    auto handler = request_handler{serverid_};
    try
    {
        auto listener = comm::listener{service_};
        while (!ready(kill_switch, 0ms))
        {
            serve_client(listener, handler); // serve one client's request(s)
        }
    }
    catch (std::exception const& ex)
    {
        logger::error("fata error occurred: "s + ex.what());
    }
    death.set_value();
}

void
sadcd::serve_client(comm::listener const& listener, request_handler& handler)
{
    logger::debug("waiting for a client connection..."sv);
    auto sock = listener.accept();
    if (!sock.valid())
    {
        return;
    }

    auto ch        = msgs::channel{std::move(sock)};
    auto processor = msgs::chunk::processor{};
    for (auto i = 0U; i < limits::requests_per_conn; i++)
    {
        auto [result, eof] = processor.process_next(ch, handler);
        if (result)
        {
            continue;
        }
        if (eof)
        {
            logger::debug("EOF"sv);
            break;
        }
        logger::debug("unable to serve request; terminating connection"sv);
        break;
    }
}

void
sadcd::serve_requests(msgs::channel ch)
{
	auto processor = msgs::chunk::processor{};
	bool result{false}, eof{false};
	while (true)
	{
		auto [result, eof] = processor.process_next(ch, *this);
		if (result)
		{
			logger::info("request served successfully"sv);
		}
		else if (eof)
		{
			break;
		}
		else
		{
			logger::error("request service failed"sv);
		}
	}
}

bool
sadcd::handle(msgs::chunk::read_request const& rr, msgs::message_header const&,
	      msgs::channel const& ch)
{
	auto response = msgs::client::read_response{
		true,		/* ok */
		"chunk chonk"	/* data */
	};
	auto result = msgs::client::serializer{}.serialize(response, ch);
	ch.flush();
	return result;
}

bool
sadcd::join_network()
{
    auto sock = master_.connect();
    if (!sock.valid())
    {
        return false;
    }

    auto ch = msgs::channel{std::move(sock)};
    auto jr = msgs::master::join_network_request{
        service_, constants::max_chunks, /*chunk_count=*/0};

    // send join_network_request
    msgs::master::serializer{{.host_id = serverid_}}.serialize(jr, ch);
    // TODO: verify success
    return true;
}

bool
sadcd::notify_master_of_write(chunkid chunk, version version_num,
                              std::string const& filename,
                              uint32_t           new_chunk_size)
{
    auto sock = master_.connect();
    if (!sock.valid())
    {
        return false;
    }

    auto ch = msgs::channel{std::move(sock)};

    auto cwn = msgs::master::chunk_write_notification{
        chunk, version_num, filename, new_chunk_size};

    msgs::master::serializer{{.host_id = serverid_}}.serialize(cwn, ch);
    // TODO: confirm from master that the write went through
    return true;
}

// ==================================================================
//                           request_handler
// ==================================================================
namespace
{

// get version info of the newest version of a chunk
auto latest_version = [](auto it) {
    return std::max_element(it->second.versions().begin(),
                            it->second.versions().end(),
                            [](auto lmeta, auto rmeta) {
                                return lmeta.version() < rmeta.version();
                            });
};

std::string
filename(chunkid const& id, uint32_t version)
{
    // TODO: should be /var/lib/sadcd/chunk/ + ...
    return "chunk/" + to_string(id) + "/" + std::to_string(version);
}

bool
read(chunkid const& id, uint32_t version, uint32_t offset, uint32_t length,
     std::string& buf)
{
    logger::debug("looking for file: " + filename(id, version));
    auto file = std::ifstream{filename(id, version), std::ios_base::in};
    if (!file.is_open())
    {
        return false;
    }

    file.seekg(offset, std::ios_base::beg);
    file.read(buf.data(), length);

    if (file.good() && file.gcount() == length)
    {
        return true;
    }
    logger::debug("file read error"sv);
    return false;
}

} // unnamed namespace
request_handler::request_handler(serverid id) : serverid_{std::move(id)}
{
    /* for testing
    auto insert = [&](auto id, auto version, auto size) {
        auto entry = chunk_metadata_[chunkid::from_string(id)].add_versions();
        entry->set_version(version);
        entry->set_size(size);
    };
    insert("6cfe7de1-b508-4e26-864e-7887fbc700ef"s, 2U, 6 * 1024U);
    insert("8cf03844-5a57-47e6-ab60-81f2de556aa4"s, 1U, 8 * 1024U);
    insert("bf5b432c-3889-4484-aa3c-77a08d30901b"s, 5U, 16 * 1024U);
    insert("229217b9-6bf0-47f0-9752-64541a99a67a"s, 1U, 4 * 1024U);
    insert("d7e30bce-1c62-4b9a-b88b-c9d2632142a7"s, 6U, 60 * 1024U);
    insert("d7e30bce-1c62-4b9a-b88b-c9d2632142a7"s, 7U, 64 * 1024U);
    */
}

bool
request_handler::handle(msgs::chunk::read_request const& req,
                        msgs::message_header const&, msgs::channel const& ch)
{
    auto respond = [this, &ch](auto ok, std::string&& data) {
        auto flush = [&ch] {
            ch.flush();
            return true;
        };
        return msgs::client::serializer{{serverid_}}.serialize(
                   msgs::client::read_response{ok, std::move(data)}, ch) &&
               flush();
    };
    // validate chunk_id
    auto it = chunk_metadata_.find(req.chunk_id());
    if (it == chunk_metadata_.end())
    {
        logger::debug("chunk does not exist on this server"sv);
        return respond(false, {});
    }

    // validate offset and length
    auto m_it = latest_version(it);
    if (req.offset() >= m_it->size() ||
        (m_it->size() - req.offset()) < req.length())
    {
        logger::debug("request to read invalid section of chunk"sv);
        return respond(false, {});
    }

    // approved
    auto data    = std::string(req.length(), '\0');
    auto success = read(req.chunk_id(), m_it->version(), req.offset(),
                        req.length(), data);

    // return true IFF we were able to read data AND send it to the client
    return respond(success, success ? std::move(data) : ""s) && success;
}

} // namespace sadfs
