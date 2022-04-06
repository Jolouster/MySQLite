add_library(MySQLite STATIC 
	src/mysqlite.cpp 
)

target_include_directories(MySQLite PUBLIC 
	include
	${PROJECT_SOURCE_DIR}/external/sqlite3
)

target_link_libraries(MySQLite sqlite3 stdc++fs)
