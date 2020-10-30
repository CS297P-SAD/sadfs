#ifndef SADFS_BOOTSTRAP_BOOTSTRAP_HPP
#define SADFS_BOOTSTRAP_BOOTSTRAP_HPP
/* declares bootstrapping utility functions */

#include <boost/program_options.hpp>

namespace sadfs { namespace bootstrap {
namespace po = boost::program_options;

// parses command-line args and populates variables
void parse_args(po::variables_map&, int argc, char const** argv,
                po::options_description const&);

// parses config file and populates variables
void parse_config_file(po::variables_map&, po::options_description const&);

// verifies that all mandatory options are configured
// option handlers are notified as well
void verify(po::variables_map&);

} // bootstrap namespace
} // sadfs namespace

#endif // SADFS_BOOTSTRAP_BOOTSTRAP_HPP
