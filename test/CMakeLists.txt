cmake_minimum_required(VERSION 3.18.0)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})

add_executable(appTest ${TEST_SOURCES})


target_include_directories(
    appTest
    PUBLIC
    ${PROJECT_SOURCE_DIR}/external/googletest
    ${PROJECT_SOURCE_DIR}/external/sqlite3
    # ${PROJECT_SOURCE_DIR}/external/rapidjson
)

target_link_libraries(
    appTest 
    PUBLIC 
    gtest_main 
    MySQLite
)

add_test(
    NAME MySQLiteTest
    COMMAND appTest
)

gtest_discover_tests(appTest)