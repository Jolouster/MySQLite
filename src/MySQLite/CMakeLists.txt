add_library(MySQLite STATIC 
	src/sqlite3.c
	src/mysqlite.cpp 
)

add_compile_options(-O2 -DSQLITE_ENABLE_JSON1)
target_include_directories(MySQLite PUBLIC include)
# set (CMAKE_C_COMPILE_OBJECT ${CMAKE_C_COMPILER})
# set (CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS}"-pthread -DSQLITE_ENABLE_JSON1 -O2")

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(MySQLite pthread dl m)
endif()

# target_link_libraries(MySQLite stdc++fs)
