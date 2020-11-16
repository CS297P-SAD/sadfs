/* Code for the sadmd class,  */

// sadfs-specific includes
#include <sadfs/comm/inet.hpp>
#include <sadfs/sadmd/sadmd.hpp>

// standard includes
#include <array>
#include <cstring>  // std::strerror
#include <iostream>
#include <string>
#include <sqlite3.h>
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
create_file(std::string const& filename)
{
	if (!files_.count(filename))
	{
		files_.emplace(filename, file_info{0, });
	}
	else
	{
		std::cerr << "Error: " << filename << ": file already exists\n";
	}
	
}

void sadmd::
db_command(std::string const& stmt) const noexcept
{
	if (sqlite3_exec(files_db_, stmt.c_str(), NULL, NULL, NULL) != 0)
	{
		std::cerr << "Error running {" << stmt << "}: ";
		std::cerr << sqlite3_errmsg(files_db_) << '\n';
		std::exit(1);
	}
}

void sadmd::
load_files()
{
	std::cout << "Files loaded:\n";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(files_db_, "SELECT * FROM files;", -1, &stmt, NULL) != 0)
	{
		std::cerr << sqlite3_errmsg(files_db_) << '\n';
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		auto filename = std::string{reinterpret_cast<const char*>(
			sqlite3_column_text(stmt, 0))};
		const unsigned char* cids = sqlite3_column_text(stmt, 1);
		std::cout << filename << ": " << cids << '\n';
		create_file(filename);
		//TODO: convert cids into a vector of ints and add to file_info object
	}
	sqlite3_finalize(stmt);
}

} // sadfs namespace
