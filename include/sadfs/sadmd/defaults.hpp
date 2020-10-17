#ifndef SADFS_SADMD_DEFAULTS_HPP
#define SADFS_SADMD_DEFAULTS_HPP

#include <string>

namespace sadfs { namespace sadmd { namespace defaults {

	// default configuration file path.
	// since boost::program_options expects a const& to a
	// std::string while setting an option's default value,
	// this cannot be a std::string_view. if we were to use a
	// std::string_view, we would have to construct a std::string
	// at runtime, which isn't appropriate for this use-case
	std::string const config_path("/etc/sadmd.conf");

} // defaults
} // sadmd
} // sadfs

#endif // SADFS_SADMD_DEFAULTS_HPP
