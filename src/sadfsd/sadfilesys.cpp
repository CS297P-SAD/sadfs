/* Definitions for the sadfilesys class */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/constants.hpp>
#include <sadfs/logger.hpp>
#include <sadfs/msgs/chunk/messages.hpp>
#include <sadfs/msgs/chunk/serializer.hpp>
#include <sadfs/msgs/client/deserializer.hpp>
#include <sadfs/msgs/client/messages.hpp>
#include <sadfs/msgs/master/messages.hpp>
#include <sadfs/msgs/master/serializer.hpp>
#include <sadfs/sadfsd/sadfilesys.hpp>

// external includes
#include <cerrno>	// error codes
#include <cstring> 	// strerror() and memcpy()

namespace sadfs
{

sadfilesys::sadfilesys(char const* ip, int port) : master_service_(ip, port)
{
    load_operations();
}

int
sadfilesys::bootstrap(int argc, char** argv)
{
    return ::fuse_main(argc, argv, &operations_, this);
}

sadfilesys*
sadfilesys::this_()
{
    return static_cast<sadfilesys*>(fuse_get_context()->private_data);
}

void
sadfilesys::load_operations()
{
    operations_.getattr = [](char const* path, struct stat* stbuf) -> int {
        return this_()->getattr(path, stbuf);
    };
    operations_.open = [](char const* path, fuse_file_info* fi) -> int {
        return this_()->open(path, fi);
    };
    operations_.read = [](char const* path, char* buf, size_t size,
                          off_t offset, fuse_file_info* fi) -> int {
        return this_()->read(path, buf, size, offset, fi);
    };
    operations_.readdir = [](char const* path, void* buf,
                             fuse_fill_dir_t filler, off_t off,
                             fuse_file_info* fi) -> int {
        return this_()->readdir(path, buf, filler, off, fi);
    };
}

int
sadfilesys::getattr(char const* path, struct stat* stbuf)
{
    logger::debug("gettattr() called at " + std::string(path));
    auto result{0};

    if ((strlen(path) >= 2 && path[0] == '/' && path[1] == '.') ||
    	strcmp(path, "/autorun.inf") == 0)
    {
        result = -ENOENT; 	// No such file or directory
        logger::debug("gettattr() failed with error " +
                      std::string(strerror(-result)));
        return result;
    }
    
    // base directory where filesystem is mounted
    if (strcmp(path, "/") == 0)
    {
        logger::debug("path identified as base directory"sv);
        stbuf->st_mode = S_IFDIR | 0755;
	stbuf->st_size = 0;
        return result;
    }

    auto filename     = std::string{path};
    auto request      = msgs::master::file_metadata_request{filename};
    auto response     = msgs::client::file_metadata_response{};
    auto serializer   = msgs::master::serializer{};
    auto deserializer = msgs::client::deserializer{};
    auto sock         = master_service_.connect();
    if (!sock.valid())
    {
        result = -ENETUNREACH;	// Network is unreachable
        logger::debug("gettattr() failed with error " +
                      std::string(strerror(-result)));
        return result;
    }
    auto ch = msgs::channel{std::move(sock)};

    auto flush = [](auto const& ch) {
        ch.flush();
        return true;
    };

    if (!(serializer.serialize(request, ch) && flush(ch) &&
          deserializer.deserialize(response, ch).first))
    {
        result = -EPROTONOSUPPORT;	// Protocol not supported
        logger::debug("gettattr() failed with error " +
                      std::string(strerror(-result)));
        return result;
    }
    if (!response.ok())
    {
        result = -ENOENT; 	// No such file or directory
        logger::debug("gettattr() failed with error " +
                      std::string(strerror(-result)));
        return result;
    }


    logger::debug("path identified as valid filename"sv);

    stbuf->st_mode = S_IFREG | 0666;
    stbuf->st_size = response.size();
    return result;
}

int
sadfilesys::readdir(char const* path, void* buf, fuse_fill_dir_t filler,
                    off_t off, fuse_file_info* fi)
{
    // TODO
    return -ENOENT;
}

int
sadfilesys::open(char const* path, fuse_file_info* fi)
{
    // TODO
    return 0;
}

int
sadfilesys::read(char const* path, char* buf, size_t size, off_t offset,
                 fuse_file_info* fi)
{
    logger::debug("read() called at " + std::string(path) + " to read " +
    	          std::to_string(size) + " bytes at offset " +
		  std::to_string(offset));

    auto result{0};
    auto location_request = msgs::master::chunk_location_request
    {
    	msgs::io_type::read,
	std::string{path},
	static_cast<uint32_t> (offset / constants::bytes_per_chunk)
    };
    auto location_response	= msgs::client::chunk_location_response{};
    auto master_serializer 	= msgs::master::serializer{};
    auto client_deserializer	= msgs::client::deserializer{};
    auto master_sock		= master_service_.connect();
    if (!master_sock.valid())
    {
        result = -ENETUNREACH;	// Network is unreachable
        logger::debug("read() failed with error " +
                      std::string(strerror(-result)));
        return result;
    }
    auto master_ch = msgs::channel{std::move(master_sock)};

    auto flush = [](auto const& ch) {
    	ch.flush();
	return true;
    };

    if (!(master_serializer.serialize(location_request, master_ch) &&
        flush(master_ch) &&
	client_deserializer.deserialize(location_response, master_ch).first))
    {
        result = -EPROTONOSUPPORT;	// Communication error on send
        logger::debug("read() failed to get chunk location " +
                      std::string(strerror(-result)));
        return result;
    }
    if (!location_response.ok())
    {
        result = -EBADMSG;	// Bad message
        logger::debug("read() chunk location not found " +
                      std::string(strerror(-result)));
        return result;
    }
    if (size == 0) // Not sure if we ever get such a request
    {
        logger::debug("read() of size 0 requested"sv);
    	return result;
    }
    if (location_response.locations_size() == 0)
    {
    	result =  -EPROTO;	// Protocol error
        logger::debug("read() empty chunk location returned " +
                      std::string(strerror(-result)));
        return result;
    }
    
    
    auto file_size	= location_response.file_size();
    logger::debug("read() got file size " + std::to_string(file_size));
    if (static_cast<uint32_t>(offset) >= file_size)
    {
    	return result;
    }

    auto chunk_offset	= static_cast<uint32_t>(
    				offset % constants::bytes_per_chunk);
    auto chunk_read_size= std::min<uint32_t>({
					static_cast<uint32_t>(size),
				    	constants::bytes_per_chunk -
						chunk_offset,
				    	file_size - 
						static_cast<uint32_t>(offset)
				});
    auto chunk_request	= msgs::chunk::read_request
    {
	location_response.chunk_id(),
	chunk_offset,
	chunk_read_size
    };
    auto chunk_response	= msgs::client::read_response{};
    auto chunk_serializer = msgs::chunk::serializer{};

    for (auto i = 0; i < location_response.locations_size(); ++i)
    {
    	auto chunk_sock = location_response.service(i).connect();
	if (!chunk_sock.valid())
	{
		result = -ENETUNREACH; // Network is unreachable
		continue;
	}
	auto chunk_ch = msgs::channel{std::move(chunk_sock)};

	if (!(chunk_serializer.serialize(chunk_request, chunk_ch) &&
	    flush(chunk_ch) &&
	    client_deserializer.deserialize(chunk_response, chunk_ch).first))
	{
        	result = -EPROTONOSUPPORT;	// Protocol not supported
		continue;
	}
	if (!chunk_response.ok())
	{
		result	= -EPROTO;	// Protocol error
		continue;
	}
	auto const& chunk_data = chunk_response.data();
	memcpy(buf, chunk_data.data(), chunk_data.size());
     	logger::debug("read() got " + std::to_string(chunk_data.size()) +
		      " from chunk server");
	// result returns the number of bytes read
	result = chunk_data.size();
	break;
     }
    
     if (result < 0)
     {
        logger::debug("read() failed with error " +
                      std::string(strerror(-result)));
	return result;
     }

     if (result == chunk_read_size &&
     	 offset + chunk_read_size < offset + size &&
	 offset + chunk_read_size < file_size)
     {
     	auto ret = read(path, buf + chunk_read_size, size - chunk_read_size,
		        offset + chunk_read_size, fi);
	if (ret < 0)
	{
		return result;
	}
	return result + ret;
     }
     
     logger::debug("read() finished"sv);
     return result;
}

} // namespace sadfs
