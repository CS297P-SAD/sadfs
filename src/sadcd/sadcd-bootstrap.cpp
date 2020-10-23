/* This gets compiled to the binary sadmd-bootstrap that,
 * as the name suggests, performs bootstrapping tasks required
 * to run sadmd -- the Master server daemon. As a last step,
 * it calls execve(2) to execute sadmd.
 */

// sadfs-specific includes
#include <sadfs/sadmd/defaults.hpp>

// standard includes
#include <cstdint>    // std::uint16_t
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

// external includes
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace {

// returns a boost::options_description containing
// all configuration options
po::options_description
config_options()
{
	po::options_description desc;
	// adds options that are configured in the config file,
	// but can be overridden from the command-line
	desc.add_options()
		("port,p", po::value<std::uint16_t>(),
			"override configured port number on which "
			"the server listens for incoming connections")
		;

	// adds options that can be passed only via the CLI
	desc.add_options()
		("help,h", "display this message and exit")
		("config,c", po::value<std::string>()->default_value(
		                 sadfs::sadmd::defaults::config_path),
			"configuration file to use")
		;

	return desc;
}

// parses command-line args and populates variables
void
parse_args(po::variables_map& variables,
           int argc, char const** argv,
           po::options_description const& options)
{
	po::store(po::parse_command_line(argc, argv, options),
	          variables);
	po::notify(variables);
}

// parses config file and populates variables
void
parse_config_file(po::variables_map& variables,
                 po::options_description const& options)
{
	auto const filename = std::string_view(
	                          variables["config"].as<std::string>());

	auto file     = std::ifstream(filename.data());
	if (!file.is_open())
	{
		std::cerr << "Error: " << filename << " does not exist\n";
		std::exit(1);
	}

	try
	{
		po::store(po::parse_config_file(file, options), variables);
	}
	catch (std::exception const& ex)
	{
		std::cerr << "Error: failed to parse " << filename << '\n'
			      << ex.what() << "\n";
		std::exit(1);
	}

	po::notify(variables);
}

void
display_help(po::options_description const& options)
{
	std::cout <<
		"Usage: sadmd-bootstrap OPTIONS\n\n" <<
		"Options:\n" <<
		options << '\n';
}

void
bootstrap(po::variables_map const& config)
{
	std::cerr << "Error: bootstrap() unimplemented\n";
	std::exit(1);
}

void
start_server()
{
	std::cerr << "Error: start_server() unimplemented\n";
	std::exit(1);
}

} // unnamed namespace


int
main(int argc, char const** argv)
{
	auto options   = config_options();
	auto variables = po::variables_map();

	// boost doesn't overwrite parsed options. since we want command-line
	// args to override options read from the config file, parse them first.
	parse_args(variables, argc, argv, options);

	// at this point, we can determine if we need to display the help/usage
	// message. there's no need to read the config file from disk if so.
	if (variables.count("help"))
	{
		display_help(options);
		return 0;
	}

	// read options not specified via the CLI
	parse_config_file(variables, options);

	// perform bootstrapping and start sadmd
	bootstrap(variables);
	start_server();

	// start_server must not return since it is supposed to
	// replace the program image with execve(2)
	std::cerr << "Error: could not start server\n";
	return 1;
}
