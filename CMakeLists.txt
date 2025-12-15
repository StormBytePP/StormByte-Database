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

include(cmake/asan.cmake)

add_subdirectory(doc)
add_subdirectory(lib)
## Workaround: if the user set CMAKE_SKIP_INSTALL_RULES=ON, some bundled
## third-party CMake scripts still call `install()` and CMake emits the
## warning "CMAKE_SKIP_INSTALL_RULES was enabled even though installation
## rules have been specified". Temporarily clear the flag while configuring
## `thirdparty` to avoid the noisy warning, then restore the original value.
set(_cmake_skip_install_rules_was_defined FALSE)
if(DEFINED CMAKE_SKIP_INSTALL_RULES)
	set(_cmake_skip_install_rules_was_defined TRUE)
	set(_cmake_skip_install_rules_orig ${CMAKE_SKIP_INSTALL_RULES})
	if(${_cmake_skip_install_rules_orig})
		message(STATUS "Temporarily disabling CMAKE_SKIP_INSTALL_RULES while configuring thirdparty to avoid spurious install warnings")
		set(CMAKE_SKIP_INSTALL_RULES OFF CACHE BOOL "Temporarily cleared to avoid warnings" FORCE)
	endif()
endif()

add_subdirectory(thirdparty)

if(_cmake_skip_install_rules_was_defined)
	if(${_cmake_skip_install_rules_orig})
		set(CMAKE_SKIP_INSTALL_RULES ${_cmake_skip_install_rules_orig} CACHE BOOL "Restore original value" FORCE)
	else()
		unset(CMAKE_SKIP_INSTALL_RULES CACHE)
	endif()
endif()
add_subdirectory(test)

include(cmake/outputflags.cmake)
