#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "mysqlite.h"

int main() {
    try {
        jlu::MySQLite* db = new jlu::MySQLite("test.db");

        std::cout << "Using database..." << std::endl;
        db->exec(
            "CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, name "
            "TEXT, value REAL);");
        db->exec("INSERT INTO test VALUES (4, 'one', 4.3)");
        db->exec("INSERT INTO test VALUES (5, 'two', 2.8)");
        db->exec("INSERT INTO test VALUES (6, 'three', 5.1)");
        db->exec(
            "INSERT INTO test VALUES (12, 'three', 5.1), (13, 'four', 8.1), (14, 'six', 1.0), (15, "
            "'seven', 0), (16, 'eight', 0.1);");
        std::vector<jlu::sqlRow> result;
        db->exec("SELECT * FROM test;", result);

        if (result.empty()) {
            std::cout << "result is empty!" << std::endl;
        } else {
            for (long unsigned int i = 0; i < result.size(); i++) {
                std::cout << std::get<int>(result[i]["id"]) << " - "
                          << std::get<std::string>(result[i]["name"]) << " - "
                          << std::get<double>(result[i]["value"]) << std::endl;
            }
        }

        delete db;
    } catch (const std::exception& exc) {
        std::cerr << exc.what();
        std::exit(EXIT_FAILURE);
    }

    return 0;
}
