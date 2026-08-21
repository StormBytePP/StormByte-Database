if(WIN32)
	set(WITH_MARIADB "BUNDLED" CACHE STRING "MariaDB mode: OFF, SYSTEM, BUNDLED" FORCE)
else()
	set(WITH_MARIADB "BUNDLED" CACHE STRING "MariaDB mode: OFF, SYSTEM, BUNDLED")
endif()

if(NOT WITH_MARIADB STREQUAL "OFF")
	if(WITH_MARIADB STREQUAL "SYSTEM")
		list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
		find_package(MariaDBConnector REQUIRED QUIET)
		message(STATUS "\tUsing system MariaDB Connector C")
	else()
		set(MARIADB_C_SRCDIR "${CMAKE_CURRENT_LIST_DIR}/mariadb-connector-c")
		ensure_build_dir(MARIADB_C_BUILDDIR)
		set(MARIADB_C_COMPONENT "MariaDB Connector C")

		set(MARIADB_C_OPTIONS
			"-DWITH_MYSQLCOMPAT=OFF"
			"-DWITH_UNIT_TESTS=OFF"
			"-DZLIB_BUILD_EXAMPLES=OFF"
			"-DINSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}"
			"-DINSTALL_PLUGINDIR=${CMAKE_INSTALL_LIBDIR}/mariadb/plugins"
			"-DINSTALL_PCDIR=${CMAKE_INSTALL_LIBDIR}/pkgconfig"
		)

		create_cmake_component(
			MARIADB_C_LIBRARY_CREATE_FILE
			"MariaDB-ConnectorC"
			"${MARIADB_C_COMPONENT}"
			"${MARIADB_C_SRCDIR}"
			"${MARIADB_C_BUILDDIR}"
			"${MARIADB_C_OPTIONS}"
			"static"
			"mariadbclient"
			1
		)
		include("${MARIADB_C_LIBRARY_CREATE_FILE}")

		add_dependencies(StormByte-Database MariaDB-ConnectorC_install)

		if(NOT TARGET MariaDB::ConnectorC)
			add_library(MariaDB::ConnectorC ALIAS MariaDB-ConnectorC)
		endif()

		target_include_directories(MariaDB-ConnectorC
			SYSTEM INTERFACE
			"${BUILDMASTER_INSTALL_INCLUDEDIR}/mariadb"
		)

		if(MSVC)
			target_link_libraries(StormByte-Database PRIVATE
				ws2_32
				secur32
				bcrypt
				ncrypt
				crypt32
			)
		else()
			target_link_libraries(StormByte-Database PRIVATE ssl crypto)
		endif()
	endif()

	target_link_libraries(StormByte-Database PRIVATE MariaDB::ConnectorC)
endif()
