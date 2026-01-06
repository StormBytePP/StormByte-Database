if(WIN32)
	set(WITH_SQLITE "BUNDLED" CACHE STRING "SQLite mode: OFF, SYSTEM, BUNDLED" FORCE)
else()
	set(WITH_SQLITE "BUNDLED" CACHE STRING "SQLite mode: OFF, SYSTEM, BUNDLED")
endif()
if (NOT WITH_SQLITE STREQUAL "OFF")
	if (WITH_SQLITE STREQUAL "SYSTEM")
		find_package(SQLite3 REQUIRED)
		message(STATUS "Found SQLite3: ${SQLite3_VERSION}")
	else()
		message(STATUS "\tSetting up bundled SQLite3")
		include(FetchContent)

		#Lowercase "sqlite3" prefixes needed by CMake
		set(sqlite3_VERSION 3.51.1)

		FetchContent_Declare(sqlite3
			URL      https://sqlite.org/2025/sqlite-amalgamation-3510100.zip
			URL_HASH SHA3_256=856b52ffe7383d779bb86a0ed1ddc19c41b0e5751fa14ce6312f27534e629b64
			DOWNLOAD_EXTRACT_TIMESTAMP false
		)
		
		if(NOT sqlite3_POPULATED)
			message(STATUS "\t\tFetching SQLite3 ${sqlite3_VERSION}")
			FetchContent_MakeAvailable(sqlite3)
		endif()
		
		add_library(sqlite3 STATIC "${sqlite3_SOURCE_DIR}/sqlite3.c")
		set_target_properties(sqlite3 PROPERTIES
			POSITION_INDEPENDENT_CODE        ON
			CXX_VISIBILITY_PRESET            hidden
			VISIBILITY_INLINES_HIDDEN        ON
		)
		target_include_directories(sqlite3 SYSTEM INTERFACE ${sqlite3_SOURCE_DIR})
		target_include_directories(sqlite3 PRIVATE ${sqlite3_SOURCE_DIR})
		set_target_properties(sqlite3 PROPERTIES
			VERSION 	${sqlite3_VERSION}
			SOVERSION 	${sqlite3_VERSION}
		)
		add_library(SQLite::SQLite3 ALIAS sqlite3)
	endif()

	target_link_libraries(StormByte-Database PRIVATE SQLite::SQLite3)
endif()
