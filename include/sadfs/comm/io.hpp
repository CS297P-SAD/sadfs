#ifndef SADFS_COMM_IO_IMPL_HPP
#define SADFS_COMM_IO_IMPL_HPP

// sadfs-specific includes
#include <sadfs/comm/socket.hpp>

// standard includes
#include <memory> // std::unique_ptr

// external includes
#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace sadfs { namespace comm { namespace io {

namespace gpio = google::protobuf::io;

// protobuf-based implementation of connection
class connection
{
public:
	connection(socket&& sock);

	gpio::ZeroCopyInputStream*  istream() const noexcept;
	gpio::ZeroCopyOutputStream* ostream() const noexcept;
	void                        flush()   const noexcept;
private:
	socket                                  sock_;
	std::unique_ptr<gpio::FileInputStream>  istream_;
	std::unique_ptr<gpio::FileOutputStream> ostream_;
};

} // io namespace
} // comm namespace
} // sadfs namespace

#endif // SADFS_COMM_IO_IMPL_HPP
