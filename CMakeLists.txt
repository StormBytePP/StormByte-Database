cmake_minimum_required(VERSION 3.12)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project("StormByte C++ Library: Database module"
	VERSION 0.0.9999
	DESCRIPTION "StormByte-Database is a StormByte library module for handling database connections"
	HOMEPAGE_URL "https://dev.stormbyte.org/StormByte-Database"
)

enable_testing()
add_subdirectory(thirdparty)
add_subdirectory(doc)
add_subdirectory(lib)
add_subdirectory(test)
