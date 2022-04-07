add_library(MySQLite STATIC 
	src/sqlite3.c
	src/mysqlite.cpp 
)

target_include_directories(MySQLite PUBLIC 
	include
)

# target_link_libraries(MySQLite sqlite3 stdc++fs)

set(CMAKE_C_COMPILE_OBJECT MSVC)
# target_link_libraries(MySQLite stdc++fs)
