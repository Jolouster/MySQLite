#ifndef MYSQLITE_H
#define MYSQLITE_H

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include "../../../external/sqlite3/sqlite3.h"

namespace jlu {
	typedef std::map<std::string, std::variant<int, double, std::string, std::vector<uint8_t>>>
		sqlRow;
	class MySQLite {
	   public:
		MySQLite ();
		MySQLite (const std::string& dbFileName);
		~MySQLite ();
		bool exec (const std::string& query);
		bool exec (const std::string& query, std::vector<sqlRow>& result);
		bool open (const std::string& dbName);
		bool close ();
		bool isOpen ();

	   private:
		bool returnData (std::vector<sqlRow>& result, sqlite3_stmt* stmt, const int& numCols);
		sqlite3* db;
		std::string dbName;
	};
}	// namespace jlu

#endif	 // MYSQLITE_H
