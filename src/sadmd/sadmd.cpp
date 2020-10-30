/* main process of the Master server */

// sadfs-related includes

// standard includes
#include <iostream>

int
main(int argc, char** argv)
{
	std::cout << argc << " arguments passed\n";
	for (auto i = 0; i < argc; i++)
	{
		std::cout << argv[i] << "\n";
	}
}
