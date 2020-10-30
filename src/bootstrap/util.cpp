// sadfs-specific includes
#include <sadfs/bootstrap/util.hpp>

// standard includes
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

// external includes
#include <boost/program_options.hpp>

namespace sadfs { namespace bootstrap {
namespace po = boost::program_options;

// parses command-line args and populates variables
void
parse_args(po::variables_map& variables,
           int argc, char const** argv,
           po::options_description const& options)
{
	po::store(po::parse_command_line(argc, argv, options),
	          variables);
}

// parses config file and populates variables
void
parse_config_file(po::variables_map& variables,
                 po::options_description const& options)
{
	auto const filename = variables["config"].as<std::string>();
	auto fatal_error = [](auto const& msg)
	{
		std::cerr << msg;
		std::exit(1);
	};

	auto file = std::ifstream(filename.c_str());
	if (!file.is_open())
	{
		fatal_error("Error: " + filename + " does not exist\n");
	}

	// po::store will throw an exception if the
	// config file's syntax is incorrect
	try
	{
		po::store(po::parse_config_file(file, options), variables);
	}
	catch (po::error const& ex)
	{
		fatal_error("Error: failed to parse " + filename + ": "
		            + ex.what() + "\n");
	}
}

// verifies that all mandatory options are configured
// option handlers are notified as well
void
verify(po::variables_map& variables)
{
	try
	{
		po::notify(variables);
	}
	catch (po::error const& ex)
	{
		// looks like a mandatory option is not configured
		std::cerr << "Error: " << ex.what() << "\n"
		          << "Alternatively, configure it via a config file. "
		          << "Use '--help' for more info.\n";
		std::exit(1);
	}
}

} // bootstrap namespace
} // sadfs namespace
