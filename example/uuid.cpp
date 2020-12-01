#include <sadfs/common/uuid.hpp>

#include <functional>
#include <iostream>
#include <unordered_map>
#include <string>

int
main()
{
	using namespace sadfs;
	auto uuid = common::uuid::generate();
	auto uuid_str = to_string(uuid);

	std::cout << "generated: " << uuid_str << "\n";

	std::cout << "\n";
	std::unordered_map<common::uuid, std::string> map{};
	map.emplace(common::uuid::generate(), "generated first");
	map.emplace(common::uuid::from_string(uuid_str), "parsed");
	map.emplace(common::uuid::generate(), "generated again");

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
	uuid.serialize(std::back_inserter(bytes));
	std::cout << uuid_str << " serialized to: \n";
	for (auto byte : bytes)
	{ std::cout << static_cast<int>(byte) << "\n"; }
	std::cout << "\n";

	auto new_uuid = common::uuid{};
	new_uuid.deserialize(bytes.begin());
	std::cout << "deserialized back to: " << to_string(new_uuid) << "\n";
}
