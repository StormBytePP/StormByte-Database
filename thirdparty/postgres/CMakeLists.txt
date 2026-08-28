if(WIN32)
	set(WITH_POSTGRES "BUNDLED" CACHE STRING "Postgres mode: OFF, SYSTEM, BUNDLED" FORCE)
else()
	set(WITH_POSTGRES "BUNDLED" CACHE STRING "Postgres mode: OFF, SYSTEM, BUNDLED")
endif()

if(NOT WITH_POSTGRES STREQUAL "OFF")
	if(WITH_POSTGRES STREQUAL "SYSTEM")
		find_package(PostgreSQL REQUIRED QUIET)
		message(STATUS "Using system PostgreSQL: ${PostgreSQL_VERSION}")
		target_link_libraries(StormByte-Database PRIVATE PostgreSQL::PostgreSQL)
	else()
		set(POSTGRES_OPTIONS
			docs=disabled
			docs_pdf=disabled
			llvm=disabled
			nls=disabled
			readline=disabled
			libxml=disabled
			libxslt=disabled
			icu=disabled
			zlib=disabled
			lz4=disabled
			zstd=disabled
			bonjour=disabled
			pam=disabled
			gssapi=disabled
			bsd_auth=disabled
			selinux=disabled
			systemd=disabled
			libnuma=disabled
			liburing=disabled
			plperl=disabled
			plpython=disabled
			pltcl=disabled
			uuid=none
			ssl=none
			ldap=disabled
			libcurl=disabled
			dtrace=disabled
			tap_tests=disabled
			cassert=false
		)

		if(WIN32)
			set(_pg_optstr "TOOLCHAIN=msvc;ALIAS=PostgreSQL::PostgreSQL")
		else()
			set(_pg_optstr "ALIAS=PostgreSQL::PostgreSQL")
		endif()

		buildmaster_component(
			PostgreSQL
			"PostgreSQL"
			"${CMAKE_CURRENT_LIST_DIR}/src"
			"${POSTGRES_OPTIONS}"
			static
			"pq;pgport;pgport_shlib;pgcommon;pgcommon_shlib"
			"${_pg_optstr}"
		)

		buildmaster_link(StormByte-Database PostgreSQL)
	endif()
endif()
