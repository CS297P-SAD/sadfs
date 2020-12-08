#ifndef SADFS_EXCEPTIONS_HPP
#define SADFS_EXCEPTIONS_HPP

// standard includes
#include <stdexcept>

namespace sadfs {

// indicates that a programmer has done 
// something that shouldn't have been done
struct logic_error : std::logic_error
{
	logic_error(std::string const& what) : std::logic_error{what} { }
};

// indicates that an operation has failed
struct operation_failure : std::runtime_error
{
	operation_failure(std::string const& what) : std::runtime_error{what} { }
};

} // sadfs namespace

#endif // SADFS_EXCEPTIONS_HPP
