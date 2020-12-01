#ifndef SADFS_COMMON_UUID_HPP
#define SADFS_COMMON_UUID_HPP

// standard includes
#include <algorithm>

// external includes
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>

namespace sadfs { namespace common {

// defines a UUID class that can be (de)serialized
// from/to 16-element conainers
struct uuid
{
	boost::uuids::uuid value;

	// generates a new UUID
	static uuid generate()
	{
		return uuid{boost::uuids::random_generator_pure{}()};
	}

	// parses a UUID from a std::string
	static uuid from_string(std::string const& str)
	{
		return uuid{boost::uuids::string_generator{}(str)};
	}

	// serializes to a 16-element container
	template <typename OutputIt>
	void serialize(OutputIt it)
	{
		std::copy(value.begin(), value.end(), it);
	}

	// deserializes from a 16-element conatiner
	template <typename InputIt>
	void deserialize(InputIt it)
	{
		std::copy(it, it+value.size(), value.begin());
	}
};

// required by STL
bool operator==(uuid const& lhs, uuid const& rhs)
{
	return lhs.value == rhs.value;
}

// generates a std::string in human-readable format
inline std::string to_string(uuid const& u)
{
	return boost::uuids::to_string(u.value);
}


} // common namespace
} // sadfs namespace

// define std::hash for sadfs::common::uuid
// for use with std::unordered_{map|set}
namespace std {

template <>
struct hash<sadfs::common::uuid>
{
	std::size_t operator()(sadfs::common::uuid const& u) const
	{
		return boost::uuids::hash_value(u.value);
	}
};

} // std namespace

#endif // SADFS_COMMON_UUID_HPP
