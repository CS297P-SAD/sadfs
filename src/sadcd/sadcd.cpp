/* Code for the sadcd class */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/constants.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/chunk/message_processor.hpp>
#include <sadfs/msgs/chunk/messages.hpp>
#include <sadfs/msgs/chunk/serializer.hpp>
#include <sadfs/msgs/client/messages.hpp>
#include <sadfs/msgs/client/serializer.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/msgs/messages.hpp>
#include <sadfs/sadcd/heart.hpp>
#include <sadfs/sadcd/io.hpp>
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
    auto handler = request_handler{master_, serverid_};
    try
    {
        auto listener = comm::listener{{"0.0.0.0", to_int(service_.port())}};
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

// ==================================================================
//                           request_handler
// ==================================================================
namespace
{

template <typename Serializer, typename Acknowledgement>
auto ack = [](auto ok, auto const& ch) {
    return Serializer{}.serialize(Acknowledgement{ok}, ch) && io::flush(ch);
};

// get version info of the current version of a chunk
auto current_ver_and_size = [](auto it) {
    auto const& cur = it->second.current();
    return std::make_pair(cur.version(), cur.size());
};

template <typename Request, typename MetadataIt>
bool
is_valid(Request const& req, MetadataIt it, bool new_chunk)
{
    // validate length
    if (req.length() > constants::chunk_capacity)
    {
        logger::error("invalid length requested"sv);
        return false;
    }
    // existing chunk
    if (!new_chunk)
    {
        auto [ver, size] = current_ver_and_size(it);
        (void)ver; // unused
        if ((constants::chunk_capacity - size) < req.length())
        {
            logger::error("requested length exceeds free space in chunk"sv);
            return false;
        }
    }
    return true;
};

template <typename Request>
std::vector<comm::service>
forwarding_list(Request const& req)
{
    auto list = std::vector<comm::service>{};
    for (auto i = 0; i < req.replicas_size(); i++)
    {
        list.emplace_back(req.replicas(i));
    }
    return list;
}

// TODO: move read into io namespace, and delete this implementation
std::string
filename(chunkid const& id)
{
    return "chunk/" + to_string(id) + "/current";
}

bool
read(chunkid const& id, uint32_t offset, uint32_t length, std::string& buf)
{
    logger::debug("looking for file: " + filename(id));
    auto file = std::ifstream{filename(id)};
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

bool
receive_stream(msgs::channel const& ch, std::string& data)
{
    auto stream = msgs::chunk::stream{};
    if (!msgs::chunk::deserializer{}.deserialize(stream, ch).first)
    {
        return false;
    }
    data = stream.extract_data();
    return true;
}

} // unnamed namespace

request_handler::request_handler(comm::service const& master, serverid id)
    : master_{master}, serverid_{std::move(id)}
{
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
    auto [ver, size] = current_ver_and_size(it);
    (void)ver; // unused
    if (req.offset() >= size || (size - req.offset()) < req.length())
    {
        logger::debug("request to read invalid section of chunk"sv);
        return respond(false, {});
    }

    // approved
    auto data    = std::string(req.length(), '\0');
    auto success = read(req.chunk_id(), req.offset(), req.length(), data);

    // return true IFF we were able to read data AND send it to the client
    return respond(success, success ? std::move(data) : ""s) && success;
}

bool
request_handler::handle(msgs::chunk::append_request const& req,
                        msgs::message_header const&, msgs::channel const& ch)
{
    using msgs::client::acknowledgement;
    using msgs::client::serializer;
    return handle(req, ack<serializer, acknowledgement>, ch);
}

bool
request_handler::handle(msgs::chunk::append_forward_request const& req,
                        msgs::message_header const&, msgs::channel const& ch)
{
    using msgs::chunk::acknowledgement;
    using msgs::chunk::serializer;
    return handle(req, ack<serializer, acknowledgement>, ch);
}

template <typename Request, typename Ack>
bool
request_handler::handle(Request const& req, Ack ack, msgs::channel const& ch)
{
    // validate req
    auto it        = chunk_metadata_.find(req.chunk_id());
    auto new_chunk = it == chunk_metadata_.end();
    if (!is_valid(req, it, new_chunk))
    {
        return ack(false, ch);
    }

    // request validated, ok to receive data
    if (!ack(true, ch))
    {
        logger::error("unable to send ack to client"sv);
        return false;
    }

    auto [ver, size] = [it, new_chunk]() {
        auto const make_pair = std::make_pair<uint32_t, uint32_t>;
        if (new_chunk)
        {
            return make_pair(0, 0);
        }
        return current_ver_and_size(it);
    }();

    auto buf = std::string{};
    if (!receive_stream(ch, buf))
    {
        return false;
    }

    // clang-format off
	// append data to chunk
    if (!io::append(
            {
                .id       = req.chunk_id(),
                .ver      = ver,
                .size     = size, // current size
                .offset   = size, // append
                .length   = req.length(),
                .filename = req.filename(),
                .data     = buf
            }, // write_spec
            master_,
			serverid_))
	{
		logger::error("append operation has failed"sv);
		return ack(false, ch) && false; // we could not handle the request
	}
    // clang-format on
    // the append operation is now guaranteed to persist
    // update data structures in memory
    if (new_chunk)
    {
        chunk_metadata_[req.chunk_id()];
    }
    auto cur = chunk_metadata_[req.chunk_id()].mutable_current();
    cur->set_version(cur->version() + 1);
    cur->set_size(cur->size() + req.length());

    // tell the client that the operation succeeded
    if (!ack(true, ch))
    {
        logger::error("unable to send ack to client"sv);
        // TODO: revisit this, and see if we should return at this
        //       point or forward the operation anyway
        return false;
    }

    // append has succeeded; forward data to other chunk servers
    io::forward({.id              = req.chunk_id(),
                 .length          = req.length(),
                 .filename        = req.filename(),
                 .data            = std::move(buf),
                 .forwarding_list = forwarding_list(req)},
                serverid_);
    return true;
}

} // namespace sadfs
