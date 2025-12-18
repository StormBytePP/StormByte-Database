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
		include(FetchContent)

		#Lowercase "sqlite3" prefixes needed by CMake
		set(sqlite3_VERSION 3.51.1)

		FetchContent_Declare(sqlite3
			URL      https://sqlite.org/2025/sqlite-amalgamation-3510100.zip
			URL_HASH SHA3_256=856b52ffe7383d779bb86a0ed1ddc19c41b0e5751fa14ce6312f27534e629b64
			DOWNLOAD_EXTRACT_TIMESTAMP false
		)
		
		if(NOT sqlite3_POPULATED)
			message(STATUS "Fetching SQLite3 ${sqlite3_VERSION}")
			FetchContent_MakeAvailable(sqlite3)
		endif()
		
		if (WIN32)
			set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
			add_library(sqlite3 SHARED "${sqlite3_SOURCE_DIR}/sqlite3.c")
			set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
			install(TARGETS sqlite3
				ARCHIVE 		DESTINATION ${CMAKE_INSTALL_LIBDIR}
				LIBRARY 		DESTINATION ${CMAKE_INSTALL_LIBDIR}
				RUNTIME 		DESTINATION ${CMAKE_INSTALL_BINDIR}
			)
		else()
			add_library(sqlite3 STATIC "${sqlite3_SOURCE_DIR}/sqlite3.c")
		endif()
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
		message(STATUS "Found SQLite3: bundled ${sqlite3_VERSION}")
	endif()

	cmake_policy(SET CMP0079 NEW)
	target_link_libraries(StormByte-Database PRIVATE SQLite::SQLite3)
endif()
