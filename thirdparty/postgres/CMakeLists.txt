if(WIN32)
	set(WITH_POSTGRES "BUNDLED" CACHE STRING "Postgres mode: OFF, SYSTEM, BUNDLED" FORCE)
else()
	set(WITH_POSTGRES "BUNDLED" CACHE STRING "Postgres mode: OFF, SYSTEM, BUNDLED")
endif()

if(NOT WITH_POSTGRES STREQUAL "OFF")
	if(WITH_POSTGRES STREQUAL "SYSTEM")
		find_package(PostgreSQL REQUIRED QUIET)
		message(STATUS "Using system PostgreSQL: ${PostgreSQL_VERSION}")
	else()
		set(POSTGRES_SRCDIR "${CMAKE_CURRENT_LIST_DIR}/src")
		ensure_build_dir(POSTGRES_BUILDDIR "postgres")
		set(POSTGRES_COMPONENT "PostgreSQL")

		set(POSTGRES_OPTIONS
			"-Ddocs=disabled"
			"-Ddocs_pdf=disabled"
			"-Dllvm=disabled"
			"-Dnls=disabled"
			"-Dreadline=disabled"
			"-Dlibxml=disabled"
			"-Dlibxslt=disabled"
			"-Dicu=disabled"
			"-Dzlib=disabled"
			"-Dlz4=disabled"
			"-Dzstd=disabled"
			"-Dbonjour=disabled"
			"-Dpam=disabled"
			"-Dgssapi=disabled"
			"-Dbsd_auth=disabled"
			"-Dselinux=disabled"
			"-Dsystemd=disabled"
			"-Dlibnuma=disabled"
			"-Dliburing=disabled"
			"-Dplperl=disabled"
			"-Dplpython=disabled"
			"-Dpltcl=disabled"
			"-Duuid=none"
			"-Dssl=none"
			"-Dldap=disabled"
			"-Dlibcurl=disabled"
			"-Ddtrace=disabled"
			"-Dtap_tests=disabled"
			"-Dcassert=false"
		)

		# On Windows, force TOOLCHAIN msvc for this component only.
		# Postgres Meson runs a C99 probe that includes <complex.h> / <tgmath.h>.
		# Under clang-cl those headers mix Clang _Complex with UCRT _Fcomplex/_Dcomplex
		# and the probe fails (C compiler does not support C99). Real MSVC (cl) passes.
		# Parent may still use clang-cl; only PostgreSQL configure/build/install use msvc.
		if(WIN32)
			create_meson_component(
				POSTGRES_LIBRARY_CREATE_FILE
				"PostgreSQL"
				"${POSTGRES_COMPONENT}"
				"${POSTGRES_SRCDIR}"
				"${POSTGRES_BUILDDIR}"
				"${POSTGRES_OPTIONS}"
				"static"
				"pq;pgport;pgport_shlib;pgcommon;pgcommon_shlib"
				1
				"msvc"
			)
		else()
			create_meson_component(
				POSTGRES_LIBRARY_CREATE_FILE
				"PostgreSQL"
				"${POSTGRES_COMPONENT}"
				"${POSTGRES_SRCDIR}"
				"${POSTGRES_BUILDDIR}"
				"${POSTGRES_OPTIONS}"
				"static"
				"pq;pgport;pgport_shlib;pgcommon;pgcommon_shlib"
				1
			)
		endif()
		include("${POSTGRES_LIBRARY_CREATE_FILE}")

		if(NOT TARGET PostgreSQL::PostgreSQL)
			add_library(PostgreSQL::PostgreSQL ALIAS PostgreSQL)
		endif()

		add_dependencies(StormByte-Database PostgreSQL_install)

		if(MSVC)
			# Upstream installs lib*.a names; rename to MSVC-style .lib and drop the DLL implib
			add_custom_command(TARGET PostgreSQL_install POST_BUILD
				COMMAND ${ENV_CMAKE_SILENT_COMMAND} -E remove "${BUILDMASTER_INSTALL_LIBDIR}/pq.lib"
				COMMAND ${ENV_CMAKE_SILENT_COMMAND} -E rename "${BUILDMASTER_INSTALL_LIBDIR}/libpq.a" "${BUILDMASTER_INSTALL_LIBDIR}/pq.lib"
				COMMAND ${ENV_CMAKE_SILENT_COMMAND} -E rename "${BUILDMASTER_INSTALL_LIBDIR}/libpgcommon.a" "${BUILDMASTER_INSTALL_LIBDIR}/pgcommon.lib"
				COMMAND ${ENV_CMAKE_SILENT_COMMAND} -E rename "${BUILDMASTER_INSTALL_LIBDIR}/libpgcommon_shlib.a" "${BUILDMASTER_INSTALL_LIBDIR}/pgcommon_shlib.lib"
				COMMAND ${ENV_CMAKE_SILENT_COMMAND} -E rename "${BUILDMASTER_INSTALL_LIBDIR}/libpgport.a" "${BUILDMASTER_INSTALL_LIBDIR}/pgport.lib"
				COMMAND ${ENV_CMAKE_SILENT_COMMAND} -E rename "${BUILDMASTER_INSTALL_LIBDIR}/libpgport_shlib.a" "${BUILDMASTER_INSTALL_LIBDIR}/pgport_shlib.lib"
				COMMENT "Renaming PostgreSQL static libraries to .lib"
			)
			target_link_libraries(StormByte-Database PRIVATE shlwapi wldap32)
		endif()
	endif()

	target_link_libraries(StormByte-Database PRIVATE PostgreSQL::PostgreSQL)
endif()
