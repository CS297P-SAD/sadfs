#ifndef SADFS_EXCEPTIONS_HPP
#define SADFS_EXCEPTIONS_HPP

// standard includes
#include <exception>

class invalid_operation : public std::runtime_error
{
public:
	invalid_operation(char const* err) : std::runtime_error(err) {}
	invalid_operation(std::string const& err) : std::runtime_error(err) {}
};

#endif // SADFS_EXCEPTIONS_HPP
