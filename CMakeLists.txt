cmake_minimum_required(VERSION 3.12.0)
set (VERSION 1.0.0)

project(sqliteMod
	DESCRIPTION "Aprendiendo a usar sqlite3 con c++"
    VERSION ${VERSION}
	LANGUAGES CXX C)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_C_STANDARD_REQUIRED true)
set (CMAKE_CXX_STANDARD_REQUIRED true)
set (BUILD_SHARED_LIBS NO) # We want static libraries
set (CMAKE_VERBOSE_MAKEFILE OFF)

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
	message("Esto es un sistema operativo Linux")
else ()
	message("Esto es un sistema operativo Windows")
endif()

option(DEBUG_TYPE "Build in debug mode" ON)
option(INCLUDE_GOOGLE_TEST "Add Google Test framework to project" ON)

if (DEBUG_TYPE)
	set (CMAKE_BUILD_TYPE Debug)
    set (DEBUG_MODE true)
else()
    set (CMAKE_BUILD_TYPE Release)
    set (DEBUG_MODE false)
endif()

set (DEST_LIB_FOLDER "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib")
set (DEST_APP_FOLDER "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin")

if (NOT EXISTS ${DEST_LIB_FOLDER})
	file(MAKE_DIRECTORY ${DEST_LIB_FOLDER})
endif()

if (NOT EXISTS ${DEST_APP_FOLDER})
	file(MAKE_DIRECTORY ${DEST_APP_FOLDER})
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DEST_LIB_FOLDER})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DEST_LIB_FOLDER})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DEST_APP_FOLDER})

message("----------------------------------------")
message("system name: ${CMAKE_SYSTEM_NAME}")
message("system processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("compiler: " ${CMAKE_CXX_COMPILER})
message("----------------------------------------")

# Add third party source code
# ----------------------------------------------
include(FetchContent)
# - Google Test
if (INCLUDE_GOOGLE_TEST)
        FetchContent_Declare(googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG main
                SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/googletest
                BINARY_DIR ${PROJECT_BINARY_DIR}/external/googletest
                INSTALL_DIR ${DEST_LIB_FOLDER}
        )

        FetchContent_GetProperties(googletest)

        if(NOT googletest_POPULATED)
			FetchContent_Populate(googletest)
			add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
        endif()
endif()
# - end Google Test

# Warnings level ---> Important: only after third party code
# Add platform dependent options
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    add_compile_options(/W3 /WX)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add subdirectories of project -- Our modules!
#-----------------------------------------------
add_subdirectory(${PROJECT_SOURCE_DIR}/src/MySQLite)

# Create App
#-----------------------------------------------
add_executable(app main.cpp)

target_link_libraries(
	app
	MySQLite
)

# - At the en of CMakeLists.txt I add test files
if (INCLUDE_GOOGLE_TEST)
	enable_testing()
	add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()
