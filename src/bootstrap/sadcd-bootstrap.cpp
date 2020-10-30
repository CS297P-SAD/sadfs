/* This gets compiled to the binary sadcd-bootstrap that,
 * as the name suggests, performs bootstrapping tasks required
 * to run sadcd -- the Chunk server daemon. As a last step,
 * it calls execvp(3) to execute sadcd.
 */

// sadfs-specific includes
#include <sadfs/sadcd/defaults.hpp>
#include <sadfs/bootstrap/util.hpp>

// standard includes
#include <array>
#include <cerrno>     // errno
#include <cstdint>    // std::uint16_t
#include <cstdlib>    // std::exit
#include <cstring>    // std::strerror
#include <iostream>
#include <string>
#include <unistd.h>   // execvp(3)

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
		("port,p", po::value<std::uint16_t>()->required(),
			"override configured port number on which "
			"the server listens for incoming connections")
		;

	// adds options that can be passed only via the CLI
	desc.add_options()
		("help,h", "display this message and exit")
		("config,c", po::value<std::string>()->default_value(
		                 sadfs::sadcd::defaults::config_path),
			"configuration file to use")
		;

	return desc;
}

void
display_help(po::options_description const& options)
{
	std::cout <<
		"Usage: sadcd-bootstrap OPTIONS\n\n" <<
		"Options:\n" <<
		options << '\n';
}

// Populates command-line args for sadcd and starts it
[[noreturn]] void
start_server(po::variables_map const& variables)
{
	using namespace std::string_literals;
	// construct arguments to pass
	auto args = std::array
	{
		"sadcd"s,
		"--port"s,
		std::to_string(variables["port"].as<std::uint16_t>())
	};

	// allocate space for pointers, including terminating nullptr
	auto argv = std::array<char*, args.size()+1>{nullptr};

	// construct argv from args
	auto get_ptr = [](auto& str) { return str.data(); };
	std::transform(args.begin(), args.end(), argv.begin(), get_ptr);

	execvp(argv.at(0), argv.data());

	// execvp returned -- something went wrong
	std::cerr << "Error: failed to start server: "
	          << std::strerror(errno) << "\n";
	std::exit(errno);
}

} // unnamed namespace


int
main(int argc, char const** argv)
{
	auto options   = config_options();
	auto variables = po::variables_map();

	// boost doesn't overwrite parsed options. since we want command-line
	// args to override options read from the config file, parse them first.
	sadfs::bootstrap::parse_args(variables, argc, argv, options);

	// at this point, we can determine if we need to display the help/usage
	// message. there's no need to read the config file from disk if so.
	if (variables.count("help"))
	{
		display_help(options);
		return 0;
	}

	// read options not specified via the CLI
	sadfs::bootstrap::parse_config_file(variables, options);

	// verify that mandatory options have been configured
	// po::notify throws if they are not
	sadfs::bootstrap::verify(variables);

	// perform bootstrapping and start sadcd
	start_server(variables);

	// start_server will not return since it is supposed to
	// replace the program image with execvp(3), or die trying
}
