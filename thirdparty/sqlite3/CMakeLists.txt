if(WIN32)
	set(WITH_SQLITE "BUNDLED" CACHE STRING "SQLite mode: OFF, SYSTEM, BUNDLED" FORCE)
else()
	set(WITH_SQLITE "BUNDLED" CACHE STRING "SQLite mode: OFF, SYSTEM, BUNDLED")
endif()

if(NOT WITH_SQLITE STREQUAL "OFF")
	if(WITH_SQLITE STREQUAL "SYSTEM")
		find_package(SQLite3 REQUIRED QUIET)
		message(STATUS "Using system SQLite3: ${SQLite3_VERSION}")
		target_link_libraries(StormByte-Database PRIVATE SQLite3::SQLite3)
	else()
		set(SQLITE_AMALGAMATION_URL
			"https://sqlite.org/2026/sqlite-amalgamation-3530400.zip")
		set(SQLITE_AMALGAMATION_HASH
			"SHA3_256=628a44cfe82c66aed1ccbbe85a562d2e33ebe64b3288981ed76285612227934e")

		buildmaster_component(
			sqlite3
			"SQLite3 amalgamation"
			"${CMAKE_CURRENT_LIST_DIR}/cmake-port"
			""
			static
			"sqlite3"
			"ALIAS=SQLite3::SQLite3;FILES={URL=${SQLITE_AMALGAMATION_URL};NAME=sqlite3;UNPACK;TITLE=SQLite3 amalgamation;EXPECTED_HASH=${SQLITE_AMALGAMATION_HASH}}"
		)

		buildmaster_link(StormByte-Database sqlite3)
	endif()
endif()
