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
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_1 (resource, value) values ('AI01', 2.3)";
	EXPECT_TRUE (db.exec (query));
}

TEST_F (MySqliteTest, Throw_Exception_at_insert_data_in_wrong_table) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_3 (resource, value) values ('AI01', 2.3)";
	EXPECT_THROW (db.exec (query), std::runtime_error);
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Select_statement) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_1 (resource, value) values ('AI01', 2.3)";
	EXPECT_TRUE (db.exec (query));
	query = "SELECT * FROM data_1;";
	std::vector<jlu::sqlRow> data;
	EXPECT_TRUE (db.exec (query, data));
	for (long unsigned int i = 0; i < data.size (); i++) {
		EXPECT_EQ (std::get<int> (data[i]["id"]), 1);
		EXPECT_EQ (std::get<std::string> (data[i]["resource"]), "AI01");
		EXPECT_EQ (std::get<double> (data[i]["value"]), 2.3);
	}
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Select_a_big_set_of_data) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_1 (resource, value) values ";
	std::vector<jlu::sqlRow> inputData;
	double t = 0.0;
	std::string coma ("");

	for (int i = 1; i <= 10000; i++) {
		t = i * 0.3;
		query += coma + "('AI0" + std::to_string (i) + "', " + std::to_string (t) + ")";
		coma = ", ";
		inputData.push_back ({{"id", i}, {"resource", "AI0" + std::to_string (i)}, {"value", t}});
	}

	EXPECT_TRUE (db.exec (query));
	query = "SELECT * FROM data_1;";
	std::vector<jlu::sqlRow> data;
	EXPECT_TRUE (db.exec (query, data));
	for (long unsigned int i = 0; i < data.size (); i++) {
		EXPECT_EQ (std::get<int> (data[i]["id"]), std::get<int> (inputData[i]["id"]));
		EXPECT_EQ (std::get<std::string> (data[i]["resource"]),
				   std::get<std::string> (inputData[i]["resource"]));
		EXPECT_DOUBLE_EQ (std::get<double> (data[i]["value"]),
						  std::get<double> (inputData[i]["value"]));
	}
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Wrong_Select_statement) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_1 (resource, value) values ('AI01', 2.3)";
	EXPECT_TRUE (db.exec (query));
	query = "SELECT * FROM data_3;";
	std::vector<jlu::sqlRow> data;
	EXPECT_THROW (db.exec (query, data), std::runtime_error);
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Empty_result_of_select) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "SELECT * FROM data_1;";
	std::vector<jlu::sqlRow> data;
	EXPECT_TRUE (db.exec (query, data));
	EXPECT_NO_THROW (db.exec (query, data));
	EXPECT_TRUE (data.empty ());
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Rename_column) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "ALTER TABLE data_1 RENAME COLUMN resource TO res;";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_1 (res, value) values('AI01', 3.1);";
	EXPECT_TRUE (db.exec (query));
	query = "SELECT * FROM data_1;";
	std::vector<jlu::sqlRow> data;
	EXPECT_TRUE (db.exec (query, data));
	for (long unsigned int i = 0; i < data.size (); i++) {
		EXPECT_EQ (std::get<int> (data[i]["id"]), 1);
		EXPECT_EQ (std::get<std::string> (data[i]["res"]), "AI01");
		EXPECT_EQ (std::get<double> (data[i]["value"]), 3.1);
	}
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Add_and_drop_column) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "ALTER TABLE data_1 ADD COLUMN mTest INTEGER;";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_1 (resource, value, mTest) values('AI01', 3.1, 1);";
	EXPECT_TRUE (db.exec (query));
	query = "SELECT * FROM data_1;";
	std::vector<jlu::sqlRow> data;
	EXPECT_TRUE (db.exec (query, data));
	for (long unsigned int i = 0; i < data.size (); i++) {
		EXPECT_EQ (std::get<int> (data[i]["id"]), 1);
		EXPECT_EQ (std::get<std::string> (data[i]["resource"]), "AI01");
		EXPECT_EQ (std::get<double> (data[i]["value"]), 3.1);
		EXPECT_EQ (std::get<int> (data[i]["mTest"]), 1);
	}
	query = "ALTER TABLE data_1 DROP COLUMN mTest;";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_1 (resource, value, mTest) values('AI01', 3.2, 1);";
	EXPECT_THROW (db.exec (query), std::runtime_error);
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Rename_table) {
	jlu::MySQLite db (fileName);
	std::string query =
		"CREATE TABLE IF NOT EXISTS data_1 (id INTEGER PRIMARY KEY ASC NOT NULL, "
		"resource TEXT NOT NULL, value REAL NOT NULL)";
	EXPECT_TRUE (db.exec (query));
	query = "ALTER TABLE data_1 RENAME TO data_A;";
	EXPECT_TRUE (db.exec (query));
	query = "INSERT INTO data_A (resource, value) values('AI01', 3.1);";
	EXPECT_TRUE (db.exec (query));
	query = "SELECT * FROM data_A;";
	std::vector<jlu::sqlRow> data;
	EXPECT_TRUE (db.exec (query, data));
	for (long unsigned int i = 0; i < data.size (); i++) {
		EXPECT_EQ (std::get<int> (data[i]["id"]), 1);
		EXPECT_EQ (std::get<std::string> (data[i]["resource"]), "AI01");
		EXPECT_EQ (std::get<double> (data[i]["value"]), 3.1);
	}
	EXPECT_TRUE (db.close ());
}