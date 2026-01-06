if(WIN32)
	set(WITH_MARIADB "BUNDLED" CACHE STRING "MariaDB mode: OFF, SYSTEM, BUNDLED" FORCE)
else()
	set(WITH_MARIADB "BUNDLED" CACHE STRING "MariaDB mode: OFF, SYSTEM, BUNDLED")
endif()
if (NOT WITH_MARIADB STREQUAL "OFF")
	if (WITH_MARIADB STREQUAL "SYSTEM")
		list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
		find_package(MariaDBConnector REQUIRED)
	else()
		# set(WITH_MYSQLCOMPAT ON CACHE STRING "Enable MySQL compatibility mode")
		# set(WITH_UNIT_TESTS OFF CACHE STRING "Disable building MariaDB Connector-C unit tests")
		# set(ZLIB_BUILD_EXAMPLES OFF CACHE STRING "Disable building zlib examples")
		# set(INSTALL_PLUGINDIR "lib/mariadb/plugins" CACHE PATH "MariaDB connector plugin install directory")
		# add_definitions(-DWIN32_LEAN_AND_MEAN -DNOGDI)
		# add_subdirectory(mariadb-connector-c)
		# add_library(MariaDB::ConnectorC ALIAS libmariadb)
		# target_include_directories(libmariadb
		# 	SYSTEM INTERFACE
		# 	"${CMAKE_CURRENT_LIST_DIR}/mariadb-connector-c/include"
		# 	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c/include>
		# )

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
			"mariadbclient" # This is the static library name
			1
		)
		include("${MARIADB_C_LIBRARY_CREATE_FILE}")
		add_dependencies(StormByte-Database MariaDB-ConnectorC_install)
		add_library(MariaDB::ConnectorC ALIAS MariaDB-ConnectorC)
		target_include_directories(MariaDB-ConnectorC SYSTEM INTERFACE ${BUILDMASTER_INSTALL_INCLUDEDIR}/mariadb)

		# Link Windows libraries
		if(MSVC)
			target_link_libraries(StormByte-Database PRIVATE
				ws2_32     # Windows Sockets API: socket, bind, connect, recv, send, shutdown, WSAStartup, etc.
				secur32    # Security Support Provider Interface (SSPI): TLS handshake, credentials, EncryptMessage, etc.
				bcrypt     # Windows CNG (Cryptography API: Next Generation): random generation, algorithm providers
				ncrypt     # Windows CNG key storage: persisted keys, self-signed certs, key export/import
				crypt32    # Windows CryptoAPI: certificate store, encoding, self-signing, PEM/DER conversion
			)
		else()
			target_link_libraries(StormByte-Database PRIVATE
				ssl
				crypto
			)
		endif()
	endif()

	target_link_libraries(StormByte-Database PRIVATE MariaDB::ConnectorC)
endif()