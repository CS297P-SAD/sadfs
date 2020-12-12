#ifndef SADFS_SOCKET_HPP
#define SADFS_SOCKET_HPP

namespace sadfs
{
namespace comm
{

class socket
{
public:
    enum class domain
    {
        local,
        inet
    };
    enum class type
    {
        stream,
        datagram
    };

    // creates a new socket
    socket(domain const, type const) noexcept;

    // creates a representation of an existing socket
    // does not create a new socket
    socket(domain const, type const, int const) noexcept;

    // constructs a socket that owns no file descriptor
    // calling comm_domain() and socket_type() is undefined
    socket() noexcept;

    // support move operations
    socket(socket&&) noexcept;
    socket& operator=(socket&&) noexcept;

    // sockets must not be copied
    socket(socket const&) = delete;
    socket& operator=(socket const&) = delete;

    ~socket() noexcept;

    // accessors
    domain comm_domain() const noexcept;
    type   socket_type() const noexcept;
    int    descriptor() const noexcept;

    // returns true if socket owns a valid descriptor
    bool valid() const noexcept;

private:
    domain domain_;
    type   type_;
    int    descriptor_;
};

// inline functions
inline socket::socket() noexcept
    : domain_{domain::inet}, type_{type::stream}, descriptor_{-1}
{
}

inline bool
socket::valid() const noexcept
{
    return descriptor_ != -1;
}

} // namespace comm
} // namespace sadfs

#endif // SADFS_SOCKET_HPP
