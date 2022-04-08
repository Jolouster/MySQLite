add_library(MySQLite STATIC 
	src/sqlite3.c
	src/mysqlite.cpp 
)

add_compile_options(-O2 -DSQLITE_ENABLE_JSON1)
target_include_directories(MySQLite PUBLIC include)
# set (CMAKE_C_COMPILE_OBJECT ${CMAKE_C_COMPILER})
# set (CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS}"-pthread -DSQLITE_ENABLE_JSON1 -O2")


target_link_libraries(MySQLite dl pthread)

# target_link_libraries(MySQLite stdc++fs)
