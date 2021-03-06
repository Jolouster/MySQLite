#include "../include/mysqlite.h"

namespace jlu {
	MySQLite::MySQLite () {
		dbName = "";
		db = nullptr;
	}

	/**
	 * @brief Opens or creates a sqlite3 database.
	 *
	 * @param dbFileName Database name. If dbFileName is ":memory:" than the database will be
	 * an in memory database. \n If dbFileName has a size of 0 than the database will be private
	 * on disk database. \n Otherwise dbFileName will be interpreted as a file.
	 * @throw std::runtime_error if database can not be open
	 */
	MySQLite::MySQLite (const std::string& dbFileName) : db (nullptr), dbName ("") {
		try {
			if (open (dbFileName))
				dbName = dbFileName;   // It is a valid database name.
		} catch (std::exception& e) { throw e; }
	}

	/**
	 * @brief Close database if it is open and destroy the MySQLite::MySQLite object
	 *
	 * In case of error show a message in standard output.
	 */
	MySQLite::~MySQLite () {
		try {
			close ();
		} catch (std::exception& e) {
			std::cerr << "Error at try close database in destructor method. Desc.: " << e.what ()
					  << std::endl;
		}
	}

	/**
	 * @brief Execute a SQL statement and return true or false. It do not return data.
	 *
	 * @param query  The string to execute by sqlite3. It must contain all data and information.
	 * @return bool  True if the process was executed successfully or false in other case.
	 * @throw std::runtime_error if the SQL statement is wrong.
	 */
	bool MySQLite::exec (const std::string& query) {
		bool output = false;
		char* errmsg = 0;
		int result = sqlite3_exec (db, query.c_str (), 0, 0, &errmsg);
		if (SQLITE_OK == result || SQLITE_DONE == result) {
			output = true;
		} else {
			std::string errorMsg ("Error in sql statement. Desc: ");
			errorMsg += std::string (errmsg);
			sqlite3_free (errmsg);
			throw std::runtime_error (errorMsg);
		}

		return output;
	}

	/**
	 * @brief Execute a SQL statement and return true or false. The data is passed by reference.
	 *
	 * @param query The string to execute by sqlite3. It must contain all data an information.
	 * @param result The container where data will be stored. It is passed by reference. \n Its
	 * type: std::vector<std::map<std::string, std::variant<int, double, std::string,
	 * std::vector<uint8_t>>>>
	 *
	 * You can iterate over result like this way:
	 * @code .cpp
	 * for(long unsigned int i = 0; i < result.size(); i++) {
	 * 		std::cout << std::get<int>(result[i]["firstField"]) << " - "
	 * 		<< std::get<std::string>(result[i]["secondField"]) << ... << std::endl;
	 * }
	 * @endcode
	 *
	 * If the result of SQL query is empty the data container will be empty.
	 *
	 * @throw std::runtime_error if the SQL statement is wrong.
	 * @return bool  True if the process was executed successfully or false in other case.
	 */
	bool MySQLite::exec (const std::string& query, std::vector<sqlRow>& result) {
		bool output = false;
		sqlite3_stmt* stmt = NULL;
		int stmtResult = sqlite3_prepare_v2 (db, query.c_str (), -1, &stmt, NULL);

		if (SQLITE_OK != stmtResult) {
			std::string errorMsg ("Unable compile the SQL statement. Error code:" +
								  std::to_string (stmtResult) + "\n");
			throw std::runtime_error (errorMsg);
		}

		int numCols = sqlite3_column_count (stmt);

		if (!isOpen ()) {
			return output;
		}

		output = returnData (result, stmt, numCols);
		return output;
	}

	/**
	 * @brief Opens or creates a sqlite3 database.
	 *
	 * @param dbFileName Database name. If dbFileName is ":memory:" than the database will be
	 * an in memory database. \n If dbFileName has a size of 0 than the database will be private
	 * on disk database. \n Otherwise dbFileName will be interpreted as a file.
	 * @throw std::runtime_error if database can not be open
	 */
	bool MySQLite::open (const std::string& dbFileName) {
		int status = sqlite3_open (dbFileName.c_str (), &db);
		bool output = false;

		if (status != SQLITE_OK) {
			std::string error ("Unable to open DB. Error: ");
			error += sqlite3_errmsg (db);
			sqlite3_close (db);

			throw std::runtime_error (error.c_str ());
		} else {
			output = true;
		}
		return output;
	}

	/**
	 * @brief  Close database.
	 *
	 * @return bool True if the process is successfull, in other case throw an exception.
	 */
	bool MySQLite::close () {
		bool output = false;
		try {
			if (db != nullptr) {
				int result = sqlite3_close (db);
				if (SQLITE_BUSY == result) {
					sqlite3_busy_timeout (db, 2000);
				}

				if (SQLITE_OK != result) {
					std::cerr << "Resutl of close database: " << result << std::endl;
					throw std::runtime_error ("Error at close database file!");
				} else {
					output = true;
					db = nullptr;
					dbName = "";
				}
			}
		} catch (std::exception& e) { std::cerr << e.what (); }

		return output;
	}

	/**
	 * @brief Check if database connection is open
	 *
	 * @return true If database is ready to use
	 * @return false Cannot use database
	 */
	bool MySQLite::isOpen () { return (db != nullptr); }

	// Private methods >>

	bool MySQLite::returnData (std::vector<sqlRow>& result,
							   sqlite3_stmt* stmt,
							   const int& numCols) {
		bool output = false;
		int rc = 0;
		std::vector<std::string> column_names;
		result.clear ();

		if (0 < numCols) {
			for (int i = 0; i < numCols; i++) {
				column_names.push_back (sqlite3_column_name (stmt, i));
			}
		}

		// prepare data to send
		while ((rc = sqlite3_step (stmt)) != SQLITE_DONE) {
			std::map<std::string, std::variant<int, double, std::string, std::vector<uint8_t>>> row;

			for (int i = 0; i < numCols; i++) {
				int columnType = sqlite3_column_type (stmt, i);

				if (SQLITE3_TEXT == columnType) {
					const unsigned char* value = sqlite3_column_text (stmt, i);
					int len = sqlite3_column_bytes (stmt, i);
					row[column_names[i]] = std::string (value, value + len);
				} else if (SQLITE_INTEGER == columnType) {
					row[column_names[i]] = sqlite3_column_int (stmt, i);
				} else if (SQLITE_FLOAT == columnType) {
					row[column_names[i]] = sqlite3_column_double (stmt, i);
				} else if (SQLITE_BLOB == columnType) {
					const uint8_t* value =
						reinterpret_cast<const uint8_t*> (sqlite3_column_blob (stmt, i));
					int len = sqlite3_column_bytes (stmt, i);
					row[column_names[i]] = std::vector<uint8_t> (value, value + len);
				} else if (SQLITE_NULL == columnType) {
					row[column_names[i]] = "null";
				}
			}
			result.push_back (row);
		}
		sqlite3_finalize (stmt);
		output = true;
		return output;
	}
}	// namespace jlu