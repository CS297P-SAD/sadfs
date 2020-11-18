/* Code for the sadmd class,  */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/sadmd/sadmd.hpp>

// standard includes
#include <array>
#include <cstring>  // std::strerror
#include <iostream>
#include <iterator>
#include <string>
#include <sqlite3.h>
#include <sstream>  // std::osstream
#include <unistd.h> // read/write

namespace {

sqlite3*
open_db()
{
	sqlite3* db;

	// open the sadmd database
	if (sqlite3_open("sadmd.db", &db) != 0)
	{
		std::cerr << "Error: Couldn't open database: ";
		std::cerr << sqlite3_errmsg(db) << '\n';
		std::exit(1);
	}
	return db;
}

std::string
chunkid_str(std::vector<uint64_t>& chunkids) noexcept
{
	std::ostringstream oss; 
	
	if (!chunkids.empty())
	{
		// copy vector with comma separators
		std::copy(chunkids.begin(), chunkids.end() - 1, 
			std::ostream_iterator<uint64_t>(oss, ", "));
		// add last separately to avoid trailing comma
		oss << chunkids.back();
	}

	return oss.str(); 
}

void
parse_chunkid_str(std::vector<uint64_t>& chunkids, 
	std::string const& existing_chunks) noexcept
{
	// index of the next (comma delimited) substring of existing_chunks
	auto front = 0;
	// length of the substring
	auto len = 0;
	
	while (front < existing_chunks.size())
	{
		len = existing_chunks.find(',', front) - front; 
		if (len > -1)
		{
			// add an integer representation of the substring
			chunkids.push_back(std::stoi(existing_chunks.substr(front, len)));
			front += len + 1;
		}
		else
		{
			// the rest of the string is the last chunkid
			chunkids.push_back(std::stoi(existing_chunks.substr(
				front, existing_chunks.size())));
			break;
		}
	}
}

std::string
process_message(sadfs::socket const& sock)
{
	auto buf = std::array<char, 512>{};
	auto len = 0;
	auto result = std::string{};
	while ((len = ::read(sock.descriptor(), buf.data(), buf.size())))
	{
		if (len == -1)
		{
			std::cerr << "read error\n";
			std::cerr << std::strerror(errno) << std::endl;
			std::exit(1);
		}
		if (::write(sock.descriptor(), buf.data(), len) == -1)
		{
			std::cerr << "write error\n";
			std::exit(1);
		}
		for (auto i = 0; i < len; i++)
		{
			result += buf[i];
		}
		buf.fill({});
	}

	return result;
}
} // unnamed namespace

namespace sadfs {

sadmd::
sadmd(char const* ip, int port) : service_(ip, port) , files_db_(open_db())
{
	// make sure the files table exists
	db_command("CREATE TABLE IF NOT EXISTS files (filename TEXT, chunkids TEXT);");
	// load files from the database to in-memory representation
	load_files();
}

void sadmd::
start()
{
	auto listener = inet::listener{service_};

	while (true)
	{
		auto sock = listener.accept();
		auto result = process_message(sock);

		std::cout << result << "\n";
		// perform some action based on result
	}
}

void sadmd::
create_file(std::string const& filename, std::string const& existing_chunks)
{
	if (!files_.count(filename))
	{
		auto info = file_info{0, };
		parse_chunkid_str(info.chunkids, existing_chunks);
		files_.emplace(filename, info);
	}
	else
	{
		// TODO: give a more meaningful error message to the user
		std::cerr << "Error: " << filename << ": file already exists\n";
		std::exit(1);
	}
	
}

void sadmd::
db_command(std::string const& stmt) const
{
	if (sqlite3_exec(files_db_, stmt.c_str(), NULL, NULL, NULL) != 0)
	{
		std::cerr << "Error running {" << stmt << "}: ";
		std::cerr << sqlite3_errmsg(files_db_) << '\n';
		std::exit(1);
	}
}

bool sadmd::
db_contains(std::string const& filename) const
{
	auto contains = false;
	sqlite3_stmt* stmt;
	auto cmd = std::string{"SELECT * FROM files WHERE filename='" + filename +
		"';"};

	if (sqlite3_prepare_v2(files_db_, cmd.c_str(), -1, &stmt, NULL) != 0)
	{
		std::cerr << sqlite3_errmsg(files_db_) << '\n';
		std::exit(1);
	}
	contains = (sqlite3_step(stmt) == SQLITE_ROW);
	sqlite3_finalize(stmt);

	return contains;
}

void sadmd::
load_files()
{
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(files_db_, "SELECT * FROM files;", -1, &stmt, NULL) != 0)
	{
		std::cerr << sqlite3_errmsg(files_db_) << '\n';
		std::exit(1);
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		auto filename = std::string{reinterpret_cast<const char*>(
			sqlite3_column_text(stmt, 0))};
		auto cids = std::string{reinterpret_cast<const char*>(
			sqlite3_column_text(stmt, 1))};
		create_file(filename, cids);
	}
	sqlite3_finalize(stmt);
}

void sadmd::
save_files() const noexcept
{
	for (auto file : files_)
	{
		auto sql_command = std::string{};
		auto filename = std::string{file.first};
		auto chunkids = chunkid_str(file.second.chunkids);

		if(db_contains(file.first))
		{
			sql_command += "UPDATE files SET chunkids='";
			sql_command += chunkids + "' WHERE filename='" + filename + "';";
		}
		else
		{
			sql_command += "INSERT INTO files VALUES('";
			sql_command += filename + "', '" +  chunkids + "');";
		}
		db_command(sql_command);
	}
}

} // sadfs namespace
