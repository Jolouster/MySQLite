#include <gtest/gtest.h>
#include <filesystem>
#include "../../external/sqlite3/sqlite3.h"
#include "../src/mySQLite/include/mysqlite.h"

// Initial setup
void db_initial_setup () {
	if (0 != remove ("test.db")) {
		std::cerr << "Error deleting test.db" << std::endl;
	}

	// we create using c library so not using any of the code to exercise
	sqlite3* db;
	char* err_msg = 0;

	int rc = sqlite3_open ("test.db", &db);

	if (rc != SQLITE_OK) {
		std::cerr << "Unable open database for testing" << std::endl;
		sqlite3_close (db);
		std::exit (EXIT_FAILURE);

		return;
	}

	const char* sql[] = {
		"DROP TABLE IF EXISTS contacts;"
		"CREATE TABLE contacts (name TEXT, company TEXT, mobile TEXT, ddi TEXT, switchboard "
		"TEXT, address1 TEXT, address2 TEXT, address3 TEXT, address4 TEXT, postcode TEXT, "
		"email TEXT, url TEXT, category TEXT, notes TEXT);"
		"CREATE INDEX idx_mobile ON contacts (mobile);"
		"CREATE INDEX idx_switchboard ON contacts (switchboard);"
		"CREATE INDEX idx_ddi ON contacts (ddi);",
		"CREATE TABLE calls(timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, callerid TEXT, "
		"contactid INTEGER);",
		"INSERT INTO contacts (name, mobile, switchboard, address1, address2, address3, "
		"postcode, email, url, category) VALUES(\"Test Person\", \"07788111222\", "
		"\"02088884444\", \"House of Commons\", \"Westminster\", \"London\", \"SW1A 0AA\", "
		"\"test@house.co.uk\", \"www.house.com\", \"Supplier\");",
		"INSERT INTO calls (callerid, contactid) VALUES(\"07788111222\", 1);"};

	size_t num_commands = sizeof (sql) / sizeof (char*);

	for (size_t i = 0; i < num_commands; ++i) {
		rc = sqlite3_exec (db, sql[i], 0, 0, &err_msg);

		if (rc != SQLITE_OK) {
			fprintf (stderr, "SQL error: %s\n", err_msg);

			sqlite3_free (err_msg);
			sqlite3_close (db);
		}
	}
	sqlite3_close (db);
}

const std::string filename ("test.db");
std::vector<std::string> tables{"contacts", "calls"};

class MySqliteTest : public ::testing::Test {
   public:
	void SetUp () { db_initial_setup (); }
};

TEST_F (MySqliteTest, Create_db_object_and_delete) {
	jlu::MySQLite db;
	EXPECT_TRUE (db.open ("test.db"));
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Drop_table_if_exists) {
	jlu::MySQLite db ("test.db");
	std::string query = "DROP TABLE IF EXISTS contacts;";
	EXPECT_TRUE (db.exec (query));
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Create_Table) {
	jlu::MySQLite db ("test.db");
	std::string query =
		"CREATE TABLE contacts (name TEXT, company TEXT, mobile TEXT, ddi TEXT, switchboard "
		"TEXT, address1 TEXT, address2 TEXT, address3 TEXT, address4 TEXT, postcode TEXT, "
		"email TEXT, url TEXT, category TEXT, notes TEXT);";
	EXPECT_TRUE (db.exec (query));
	EXPECT_TRUE (db.close ());
}

TEST_F (MySqliteTest, Create_index) {
	jlu::MySQLite db ("terst.db");
	std::string query = "CREATE INDEX idx_mobile ON contacts (mobile);";
	EXPECT_TRUE (db.exec (query));
	EXPECT_TRUE (db.close ());
}