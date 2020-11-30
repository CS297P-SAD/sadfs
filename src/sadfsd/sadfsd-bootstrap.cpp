/* This processes the input parameters to establish connection with
 * master and mount the filesystem.
 */

// sadfs-specific includes
#include <sadfs/bootstrap/util.hpp>
#include <sadfs/sadfsd/defaults.hpp>
#include <sadfs/sadfsd/sadfilesys.hpp>

// standard includes
#include <array>
#include <cstdint>    // std::uint16_t
#include <iostream>
#include <string>

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
	desc.add_options()
		("ipaddress,i", po::value<std::string>()->required(),
			"override configured ip number on which "
			"the server listens for incoming connections")
		;
	desc.add_options()
		("mountpoint,m", po::value<std::string>()->required(),
			"override path to folder where the file system "
			"is to be mounted")
		;

	// adds options that can be passed only via the CLI
	desc.add_options()
		("help,h", "display this message and exit")
		("config,c", po::value<std::string>()->default_value(
		                 sadfs::sadfsd::defaults::config_path),
			"configuration file to use")
		;

	return desc;
}

void
display_help(po::options_description const& options)
{
	std::cout <<
		"Usage: sadfsd-bootstrap OPTIONS\n\n" <<
		"Options:\n" <<
		options << '\n';
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

	// create object of sadfilesys class
	auto sadfilesys = sadfs::sadfilesys
	{
		variables["ipaddress"].as<std::string>().c_str(),
		variables["port"].as<std::uint16_t>()
	};

	// since the first argument is skipped by fuse_main, it is initialized as
	// empty string
	auto filesys_args = std::array
	{
		std::string(),
		variables["mountpoint"].as<std::string>()
	};

	// allocate space for pointers, including terminating nullptr
	auto filesys_argv = std::array<char*, filesys_args.size() + 1>{nullptr};

	// construct filesys_argv from filesys_args
	auto get_ptr = [](auto& str) { return str.data(); };
	std::transform(filesys_args.begin(),
                   filesys_args.end(),
                   filesys_argv.begin(),
                   get_ptr);

	auto status = sadfilesys.bootstrap(filesys_args.size(),
	                                   filesys_argv.data());

	return status;
}
