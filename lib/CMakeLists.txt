include(GNUInstallDirs)

# Sources
file(GLOB_RECURSE STORMBYTE_DATABASE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/public/*.cxx")
file(GLOB_RECURSE STORMBYTE_DATABASE_SQLITE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/optional/StormByte/database/sqlite/*.cxx")
file(GLOB_RECURSE STORMBYTE_DATABASE_SQLITE_SOURCES_PRIVATE CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/private/StormByte/database/sqlite/*.cxx")
file(GLOB_RECURSE STORMBYTE_DATABASE_POSTGRES_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/optional/StormByte/database/postgres/*.cxx")
file(GLOB_RECURSE STORMBYTE_DATABASE_POSTGRES_SOURCES_PRIVATE CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/private/StormByte/database/postgres/*.cxx")
file(GLOB_RECURSE STORMBYTE_DATABASE_MARIADB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/optional/StormByte/database/mariadb/*.cxx")
file(GLOB_RECURSE STORMBYTE_DATABASE_MARIADB_SOURCES_PRIVATE CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/private/StormByte/database/mariadb/*.cxx")

# Remove redundant sources


# Library
add_library(StormByte-Database SHARED ${STORMBYTE_DATABASE_SOURCES})
add_library(StormByte::Database ALIAS StormByte-Database)

if (NOT WIN32 AND ENABLE_ASAN)
	target_compile_options(StormByte-Database PUBLIC ${SANITIZER_FLAGS} -g -fno-omit-frame-pointer)
	target_link_options(StormByte-Database PUBLIC ${SANITIZER_FLAGS})
endif()

set_target_properties(StormByte-Database PROPERTIES
	LINKER_LANGUAGE CXX
	SOVERSION		${CMAKE_PROJECT_VERSION}
	VERSION 		${CMAKE_PROJECT_VERSION}
)
set_target_properties(StormByte-Database PROPERTIES
	POSITION_INDEPENDENT_CODE        ON
	CXX_VISIBILITY_PRESET            hidden
	VISIBILITY_INLINES_HIDDEN        ON
)

# SQLite optional
if (NOT WITH_SQLITE STREQUAL "OFF")
	target_sources(StormByte-Database
		PRIVATE
		${STORMBYTE_DATABASE_SQLITE_SOURCES}
		${STORMBYTE_DATABASE_SQLITE_SOURCES_PRIVATE}
	)
endif()

if (NOT WITH_POSTGRES STREQUAL "OFF")
	target_sources(StormByte-Database
		PRIVATE
		${STORMBYTE_DATABASE_POSTGRES_SOURCES}
		${STORMBYTE_DATABASE_POSTGRES_SOURCES_PRIVATE}
	)
endif()

if (NOT WITH_MARIADB STREQUAL "OFF")
	target_sources(StormByte-Database
		PRIVATE
		${STORMBYTE_DATABASE_MARIADB_SOURCES}
		${STORMBYTE_DATABASE_MARIADB_SOURCES_PRIVATE}
	)
endif()

# Compile options
if(MSVC)
	target_compile_options(StormByte-Database PRIVATE /EHsc)
	target_compile_options(StormByte-Database PRIVATE $<$<CONFIG:Release>:/O2>)
	target_compile_options(StormByte-Database PRIVATE $<$<CONFIG:Debug>:/Od>)
	target_link_options(StormByte-Database PRIVATE $<$<CONFIG:Release>:/LTCG /GL>)
else()
	set(CMAKE_CXX_FLAGS_DEBUG "-pipe -g -ggdb -Wall -Wextra -Wnon-virtual-dtor -pedantic -pedantic-errors -O0")
endif()

# Include directories
target_include_directories(StormByte-Database
	SYSTEM BEFORE PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public" "${CMAKE_CURRENT_LIST_DIR}/private" "${CMAKE_CURRENT_LIST_DIR}/optional"
)

# Install
if (NOT STORMBYTE_AS_DEPENDENCY)
	install(TARGETS StormByte-Database
		ARCHIVE 		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY 		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME 		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/public/StormByte/"
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte"
		FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	)
	if (WITH_SQLITE STREQUAL "BUNDLED")
		install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/optional/StormByte/database/sqlite"
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte/database"
			FILES_MATCHING
			PATTERN "*.h"
			PATTERN "*.hxx"
		)
	endif()
	if (WITH_POSTGRES STREQUAL "BUNDLED")
		install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/optional/StormByte/database/postgres"
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte/database"
			FILES_MATCHING
			PATTERN "*.h"
			PATTERN "*.hxx"
		)
	endif()
	if (WITH_MARIADB STREQUAL "BUNDLED")
		install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/optional/StormByte/database/mariadb"
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte/database"
			FILES_MATCHING
			PATTERN "*.h"
			PATTERN "*.hxx"
		)
	endif()
endif()
