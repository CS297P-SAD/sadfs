#ifndef SADFS_SADCD_SADCD_HPP
#define SADFS_SADCD_SADCD_HPP

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/msgs/channel.hpp>
#include <sadfs/msgs/chunk/messages.hpp>
#include <sadfs/msgs/common.hpp>
#include <sadfs/proto/internal.pb.h>
#include <sadfs/types.hpp>

// standard includes
#include <future>
#include <string>
#include <unordered_map>

namespace sadfs
{

// forward declaration
class request_handler;
class sadcd
{
public:
    sadcd(char const* ip, int port, char const* master_ip, int master_port,
          char const* server_id);

    // starts server by opening a listener
    void start();

    // handles a read_request
    bool handle(msgs::chunk::read_request const&, msgs::message_header const&,
    		msgs::channel const&);

private:
    // runs the chunk service
    void run();

    // starts main server thread
    // stops when asked to, indicates death via std::promise
    void start_main(std::promise<void>, std::shared_future<void>);

    // serve a single client's request(s)
    void serve_client(comm::listener const&, request_handler&);

    bool join_network();

    bool notify_master_of_write(chunkid, version, std::string const&,
                                uint32_t);

    comm::service const service_;
    comm::service const master_;
    serverid const      serverid_;
};

class request_handler
{
    using metadata = proto::internal::chunk_metadata;
    std::unordered_map<chunkid, metadata> chunk_metadata_;
    serverid const                        serverid_;

public:
    request_handler(serverid id);

    bool handle(msgs::chunk::read_request const&, msgs::message_header const&,
                msgs::channel const&);
};

} // namespace sadfs

#endif // SADFS_SADCD_SADCD_HPP
