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

namespace sadfs {

sadmd::
sadmd(char const* ip, int port) : service_(ip, port)
{
	sqlite3* db;

	// open the sadmd database
	auto chk = sqlite3_open("sadmd.db", &db);
	if (chk)
	{
		std::cerr << "Error: Couldn't open database: ";
		std::cerr << sqlite3_errmsg(db) << '\n';
	}
	files_db_ = db;

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
		// create_file(result);
	}
}

void sadmd::
create_file(std::string filename)
{
	auto info = file_info{false, time(0), };
	if (!files_.count(filename))
	{
		files_.emplace(filename, info);
	}
	// Do nothing if file already exists
}

std::string sadmd::
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

void sadmd::
db_command(std::string stmt)
{
	auto chk = sqlite3_exec(files_db_, stmt.c_str(), NULL, NULL, NULL);
	if (chk)
	{
		std::cerr << "Error running {" << stmt << "}: ";
		std::cerr << sqlite3_errmsg(files_db_) << '\n';
	}
}

void sadmd::
load_files()
{
	std::cout << "Files loaded:\n";
	auto res = 0;
	sqlite3_stmt* stmt;
	auto chk = sqlite3_prepare_v2(files_db_, "SELECT * FROM files;", -1, &stmt, NULL);
	if (chk)
	{
			std::cerr << sqlite3_errmsg(files_db_) << '\n';
	}

	while ((res = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		auto filename = std::string{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))};
		const unsigned char* cids = sqlite3_column_text(stmt, 1);
		std::cout << filename << ": " << cids << '\n';
		create_file(filename);
		//TODO: convert cids into a vector of ints and add to file_info object
	}
	sqlite3_finalize(stmt);
}

} // sadfs namespace
