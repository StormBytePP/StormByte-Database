include(GNUInstallDirs)

# Sources
file(GLOB_RECURSE STORMBYTE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cxx")

# Library
add_library(StormByteDatabase SHARED ${STORMBYTE_SOURCES})
add_library(StormByte::Database ALIAS StormByteDatabase)
target_link_libraries(StormByteDatabase PUBLIC StormByte)
target_link_libraries(StormByteDatabase PRIVATE sqlite3)
set_target_properties(StormByteDatabase PROPERTIES
	LINKER_LANGUAGE CXX
	SOVERSION		${CMAKE_PROJECT_VERSION}
	VERSION 		${CMAKE_PROJECT_VERSION}
)

# Compile options
if(MSVC)
	add_compile_options(/wd4251 /EHsc)
	add_compile_definitions(UNICODE) # Create problems if disabled
else()
	set(CMAKE_CXX_FLAGS_DEBUG "-pipe -g -ggdb -Wall -Wextra -Wnon-virtual-dtor -pedantic -pedantic-errors -O0")
	target_compile_options(StormByteDatabase PRIVATE -fvisibility=hidden $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden>)
endif()

# Include directories
target_include_directories(StormByteDatabase
	SYSTEM BEFORE PUBLIC "${CMAKE_CURRENT_LIST_DIR}"
)

# Install
if (NOT STORMBYTE_AS_DEPENDENCY)
	install(TARGETS StormByteDatabase
		ARCHIVE 		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY 		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME 		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/"
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte"
		FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	)
endif()
