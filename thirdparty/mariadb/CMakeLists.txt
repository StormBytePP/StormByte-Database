set(WITH_MARIADB "BUNDLED" CACHE STRING "MariaDB mode: OFF, SYSTEM, BUNDLED")

if (NOT WITH_MARIADB STREQUAL "OFF")
	if (WITH_MARIADB STREQUAL "SYSTEM")
		list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
		find_package(MariaDBConnector REQUIRED)
	else()
		set(WITH_MYSQLCOMPAT OFF CACHE STRING "Enable MySQL compatibility mode (requires MariaDB Connector-C)")
		set(WITH_UNIT_TESTS OFF CACHE STRING "Disable building MariaDB Connector-C unit tests")
		set(ZLIB_BUILD_EXAMPLES OFF CACHE STRING "Disable building zlib examples")
		set(INSTALL_PLUGINDIR "lib/mariadb/plugins" CACHE PATH "MariaDB connector plugin install directory")
		add_definitions(-DWIN32_LEAN_AND_MEAN -DNOGDI)
		add_subdirectory(mariadb-connector-c)
		add_library(MariaDB::ConnectorC ALIAS libmariadb)
		target_include_directories(libmariadb
			SYSTEM INTERFACE
			"${CMAKE_CURRENT_LIST_DIR}/mariadb-connector-c/include"
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c/include>
		)
	endif()

	cmake_policy(SET CMP0079 NEW)
	target_link_libraries(StormByte-Database PRIVATE MariaDB::ConnectorC)
endif()