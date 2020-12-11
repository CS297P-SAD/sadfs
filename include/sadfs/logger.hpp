#ifndef SADFS_LOGGER_HPP
#define SADFS_LOGGER_HPP

#include <chrono>
#include <iomanip>
#include <iostream>

namespace sadfs
{
using namespace std::string_literals;
using namespace std::string_view_literals;
namespace logger
{

namespace
{
auto timestamp = []() {
    using std::chrono::system_clock;
    auto now = system_clock::to_time_t(system_clock::now());
    std::cout << std::put_time(std::gmtime(&now), "%c %Z");
};
}

inline void
info(std::string_view msg)
{
    timestamp();
    std::cout << " [INFO]: " << msg << "\n";
}

inline void
debug(std::string_view msg)
{
#ifndef NDEBUG
    timestamp();
    std::cout << " [DEBUG]: " << msg << "\n";
#endif // NDEBUG
}

inline void
error(std::string_view msg)
{
    timestamp();
    std::cerr << " [ERROR]: " << msg << "\n";
}

inline void
info(std::string msg)
{
	info(std::string_view{msg});
}

inline void
debug(std::string msg)
{
	debug(std::string_view{msg});
}

inline void
error(std::string msg)
{
	error(std::string_view{msg});
}

} // namespace logger
} // namespace sadfs

#endif // SADFS_LOGGER_HPP
