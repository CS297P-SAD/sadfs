#include <sadfs/uuid.hpp>

#include <functional>
#include <iostream>
#include <unordered_map>
#include <string>

int
main()
{
	using namespace sadfs;
	auto id = uuid::generate();
	auto id_str = to_string(id);

	std::cout << "generated: " << id_str << "\n";

	std::cout << "\n";
	std::unordered_map<uuid, std::string> map{};
	map.emplace(uuid::generate(), "generated first");
	map.emplace(uuid::from_string(id_str), "parsed");
	map.emplace(uuid::generate(), "generated again");

	std::cout << "printing std::unordered_map of uuids\n"
	          << "==================================================\n";
	for (auto kvp : map)
	{
		std::cout << to_string(kvp.first) << ": " << kvp.second << "\n";
	}

	std::cout << "==================================================\n";

	// simulate (de)serialization to/from protobufs
	std::cout << "\n";
	auto bytes = std::string{};
	id.serialize(std::back_inserter(bytes));
	std::cout << id_str << " serialized to: \n";
	for (auto byte : bytes)
	{ std::cout << static_cast<int>(byte) << "\n"; }
	std::cout << "\n";

	auto new_id = uuid{};
	new_id.deserialize(bytes.begin());
	std::cout << "deserialized back to: " << to_string(new_id) << "\n";
}
