if(WIN32)
	set(WITH_SQLITE "BUNDLED" CACHE STRING "SQLite mode: OFF, SYSTEM, BUNDLED" FORCE)
else()
	set(WITH_SQLITE "BUNDLED" CACHE STRING "SQLite mode: OFF, SYSTEM, BUNDLED")
endif()

if(NOT WITH_SQLITE STREQUAL "OFF")
	if(WITH_SQLITE STREQUAL "SYSTEM")
		find_package(SQLite3 REQUIRED)
		message(STATUS "Found SQLite3: ${SQLite3_VERSION}")
	else()
		message(STATUS "\tSetting up bundled SQLite3")

		set(SQLITE3_VERSION "3.53.4")
		set(SQLITE3_AMALGAMATION_URL
			"https://sqlite.org/2026/sqlite-amalgamation-3530400.zip")
		set(SQLITE3_AMALGAMATION_HASH
			"SHA3_256=628a44cfe82c66aed1ccbbe85a562d2e33ebe64b3288981ed76285612227934e")

		# ------------------------------------------------------------------
		# 1. Download (cached)
		# ------------------------------------------------------------------
		file_download_cached(SQLITE3_DOWNLOAD_SCRIPT
			"${SQLITE3_AMALGAMATION_URL}"
			TITLE "SQLite3 ${SQLITE3_VERSION} amalgamation"
			EXPECTED_HASH "${SQLITE3_AMALGAMATION_HASH}"
			2
		)
		include("${SQLITE3_DOWNLOAD_SCRIPT}")

		# ------------------------------------------------------------------
		# 2. Extract into a stable source directory (only once)
		# ------------------------------------------------------------------
		set(SQLITE3_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/sqlite3-src")
		set(SQLITE3_AMALGAMATION_DIR
			"${SQLITE3_SRC_DIR}/sqlite-amalgamation-3530400")

		if(NOT EXISTS "${SQLITE3_AMALGAMATION_DIR}/sqlite3.c")
			file_decompress(SQLITE3_DECOMPRESS_SCRIPT
				"sqlite-amalgamation-3530400.zip"
				"${SQLITE3_SRC_DIR}"
				TITLE "SQLite3 ${SQLITE3_VERSION}"
				2
			)
			include("${SQLITE3_DECOMPRESS_SCRIPT}")
		endif()

		# ------------------------------------------------------------------
		# 3. Minimal CMakeLists (write only once)
		# ------------------------------------------------------------------
		if(NOT EXISTS "${SQLITE3_AMALGAMATION_DIR}/CMakeLists.txt")
			file(WRITE "${SQLITE3_AMALGAMATION_DIR}/CMakeLists.txt" [=[
cmake_minimum_required(VERSION 3.12)
project(sqlite3 LANGUAGES C)

include(GNUInstallDirs)

add_library(sqlite3 STATIC sqlite3.c)
target_include_directories(sqlite3
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
)
set_target_properties(sqlite3 PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	OUTPUT_NAME sqlite3
)
install(TARGETS sqlite3
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES sqlite3.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
]=])
		endif()

		# ------------------------------------------------------------------
		# 4. BuildMaster component
		# ------------------------------------------------------------------
		ensure_build_dir(SQLITE3_BUILD_DIR "sqlite3")
		set(SQLITE3_COMPONENT "SQLite3 amalgamation")

		set(SQLITE3_OPTIONS
			-DCMAKE_POSITION_INDEPENDENT_CODE=ON
		)

		create_cmake_component(
			SQLITE3_LIBRARY_CREATE_FILE
			"sqlite3"
			"${SQLITE3_COMPONENT}"
			"${SQLITE3_AMALGAMATION_DIR}"
			"${SQLITE3_BUILD_DIR}"
			"${SQLITE3_OPTIONS}"
			"static"
			"sqlite3"
			1
		)
		include("${SQLITE3_LIBRARY_CREATE_FILE}")

		if(NOT TARGET SQLite3::SQLite3)
			add_library(SQLite3::SQLite3 ALIAS sqlite3)
		endif()
	endif()

	target_link_libraries(StormByte-Database PRIVATE SQLite3::SQLite3)
endif()
