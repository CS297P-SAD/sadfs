#ifndef SADFS_LOGGER_HPP
#define SADFS_LOGGER_HPP

#include <iostream>

namespace sadfs
{
using namespace std::string_literals;
using namespace std::string_view_literals;
namespace logger
{

inline void
info(std::string_view msg)
{
    std::cout << "[INFO]: " << msg << "\n";
}

inline void
debug(std::string_view msg)
{
#ifndef NDEBUG
    std::cout << "[DEBUG]: " << msg << "\n";
#endif // NDEBUG
}

inline void
error(std::string_view msg)
{
    std::cout << "[ERROR]: " << msg << "\n";
}

inline void
info(std::string const &msg)
{
    info(std::string_view{msg.data(), msg.size()});
}

inline void
debug(std::string const &msg)
{
#ifndef NDEBUG
    debug(std::string_view{msg.data(), msg.size()});
#endif // NDEBUG
}

inline void
error(std::string const &msg)
{
    error(std::string_view{msg.data(), msg.size()});
}

} // namespace logger
} // namespace sadfs

#endif // SADFS_LOGGER_HPP
