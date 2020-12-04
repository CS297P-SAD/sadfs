/* verifies the correctness of uuid io into/from file_chunks */
#include <sadfs/sadmd/util.hpp>

#include <iostream>

int
main()
{
	using namespace sadfs;
	auto id = uuid::generate();
	auto fc = util::file_chunks{};

	fc.add_chunk(id);
	auto ret_id = fc[0];

	std::cout << "generated uuid: " << to_string(id) << "\n"
	          << "retrieved uuid: " << to_string(ret_id) << "\n";
}
