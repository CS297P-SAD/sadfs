#ifndef SADFS_COMMON_UUID_HPP
#define SADFS_COMMON_UUID_HPP

// standard includes
#include <algorithm>

// external includes
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>

namespace sadfs {

// defines a UUID class that can be (de)serialized
// from/to 16-element conainers
struct uuid
{
	boost::uuids::uuid value;

	// generates a new UUID
	static uuid generate()
	{
		static auto generate = boost::uuids::random_generator_pure{};
		return uuid{generate()};
	}

	// parses a UUID from a std::string
	static uuid from_string(std::string const& str)
	{
		static auto generate = boost::uuids::string_generator{};
		return uuid{generate(str)};
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
inline bool
operator==(uuid const& lhs, uuid const& rhs)
{
	return lhs.value == rhs.value;
}

// generates a std::string in human-readable format
inline
std::string to_string(uuid const& u)
{
	return boost::uuids::to_string(u.value);
}

} // sadfs namespace

// define std::hash for sadfs::uuid
// for use with std::unordered_{map|set}
namespace std {

template <>
struct hash<sadfs::uuid>
{
	std::size_t operator()(sadfs::uuid const& u) const
	{
		return boost::uuids::hash_value(u.value);
	}
};

} // std namespace

#endif // SADFS_COMMON_UUID_HPP
