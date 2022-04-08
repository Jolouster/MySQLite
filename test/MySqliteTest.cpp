#include <gtest/gtest.h>
#include <filesystem>
#include "../src/MySQLite/include/mysqlite.h"
#include "../src/MySQLite/include/sqlite3.h"

// Initial setup
void db_initial_setup (const std::string& fname, const std::string& bfname) {
	std::string fileNames[] = {fname, bfname};
	for (std::string name : fileNames) {
		std::filesystem::path f (name.c_str ());
		if (true == std::filesystem::exists (f)) {
			int r = remove (name.c_str ());
			if (0 != r) {
				std::cerr << "Error deleting test.db. Error code: " << r << std::endl;
			}
		}
	}
}

const std::string fileName ("test.db");
const std::string badFileName ("terst.db");
std::vector<std::string> tables{"contacts", "calls"};

class MySqliteTest : public ::testing::Test {
   public:
	void SetUp () { db_initial_setup (fileName, badFileName); }
};

TEST_F (MySqliteTest, Create_db_object_and_delete) {
	jlu::MySQLite db;
	EXPECT_TRUE (db.open (fileName));
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Drop_table_if_exists) {
	jlu::MySQLite db (fileName);
	std::string query = "DROP TABLE IF EXISTS contacts;";
	EXPECT_NO_THROW (db.exec (query));
	EXPECT_TRUE (db.exec (query));
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Create_Table) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC, resource TEXT NOT NULL, "
		"value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Create_index_throw_exception_badFileName) {
	jlu::MySQLite db (badFileName);
	std::string query = "CREATE INDEX idx_mobile ON contacts (mobile);";
	EXPECT_THROW (db.exec (query), std::runtime_error);
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Insert_data_in_table) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER AUTOINCREMENT PRIMARY KEY NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_1 (resource, value) values ('AI01', 2.3)";
	EXPECT_TRUE (db.exec (query));
}