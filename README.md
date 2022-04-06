# MySQLite a simple wrapper for SQLite3

A simple and efficient wrapper for SQLite3 for C++.

This class is very simple and easy to use. You can use SQLite3 like an object in your C++ applications.

## Use

You have these methods:

- Open:

```cpp
jlu::MySQLite::MySQLite("dbFileName");
	jlu::MySQLite::open("dbFileName");
```

- Close database:

```cpp
jlu::MySQLite::~MySQLite();
jlu::MySQLite::close();
```

- Run SQL statements, if you don't expect any data:

```cpp
jlu::MySQLite::exec(const std::string& query);
```

- Run SQL statements, if you expect data:

```cpp
jlu::MySQLite::exec(const std::string& query, 
	std::vector<std::map<std::string, std::variant<int, double, std::string, std::vector<uint8_t>>>>);
```

## Example


