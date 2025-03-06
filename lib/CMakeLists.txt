include(GNUInstallDirs)

# Sources
set(STORMBYTE_DATABASE_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/database/exception.cxx"
)
set(STORMBYTE_DATABASE_HEADERS
	"${CMAKE_CURRENT_LIST_DIR}/database/database.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/exception.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/preparedSTMT.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/query.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/row.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/visibility.h"
)
set(STORMBYTE_DATABASE_SQLITE3_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/exception.cxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/preparedSTMT.cxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/query.cxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/sqlite3.cxx"
)
set(STORMBYTE_DATABASE_SQLITE3_HEADERS
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/alias.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/exception.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/preparedSTMT.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/query.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/sqlite3.hxx"
	"${CMAKE_CURRENT_LIST_DIR}/database/sqlite/type.hxx"
)

# Library
add_library(StormByte-Database SHARED ${STORMBYTE_DATABASE_SOURCES})
add_library(StormByte::Database ALIAS StormByte-Database)
target_link_libraries(StormByte-Database PUBLIC StormByte)
set_target_properties(StormByte-Database PROPERTIES
	LINKER_LANGUAGE CXX
	SOVERSION		${CMAKE_PROJECT_VERSION}
	VERSION 		${CMAKE_PROJECT_VERSION}
)
if (ENABLE_SQLITE)
	target_sources(StormByte-Database PRIVATE ${STORMBYTE_DATABASE_SQLITE3_SOURCES})
	target_link_libraries(StormByte-Database PRIVATE sqlite3)
endif()

# Compile options
if(MSVC)
	add_compile_options(/wd4251 /EHsc)
	add_compile_definitions(UNICODE) # Create problems if disabled
else()
	set(CMAKE_CXX_FLAGS_DEBUG "-pipe -g -ggdb -Wall -Wextra -Wnon-virtual-dtor -pedantic -pedantic-errors -O0")
	target_compile_options(StormByte-Database PRIVATE -fvisibility=hidden $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden>)
endif()

# Include directories
target_include_directories(StormByte-Database
	SYSTEM BEFORE PUBLIC "${CMAKE_CURRENT_LIST_DIR}"
)

# Install
if (NOT STORMBYTE_AS_DEPENDENCY)
	install(TARGETS StormByte-Database
		ARCHIVE 		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY 		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME 		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES ${STORMBYTE_DATABASE_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte/database")
	if (ENABLE_SQLITE)
		install(FILES ${STORMBYTE_DATABASE_SQLITE3_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte/database/sqlite")
	endif()
endif()
