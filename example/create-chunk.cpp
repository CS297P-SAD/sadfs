/* creates chunks for testing */
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

#include <sadfs/uuid.hpp>

int
main(int argc, char** argv)
{
	auto uuid = sadfs::uuid::generate();
	auto filename = (argc>1) ? std::string{argv[1]} : to_string(uuid)+".dat";
	std::cout << "creating " << filename << "\n";
	auto file = std::ofstream{filename};
	for (auto i = 0; i < 1024*1024; i++)
	{
		// [0, 36)
		file << std::string_view{to_string(uuid)};

		// [36, 63)
		for (auto j = 0; j < 27; j++)
		{
			file << static_cast<char>('0' + (i%10));
		}

		// [63, 64)
		file << '\n';
	}
}
