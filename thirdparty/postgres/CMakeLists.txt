IF(WIN32)
	set(WITH_POSTGRES "BUNDLED" CACHE STRING "Postgres mode: OFF, SYSTEM, BUNDLED" FORCE)
else()
	set(WITH_POSTGRES "BUNDLED" CACHE STRING "Postgres mode: OFF, SYSTEM, BUNDLED")
endif()
if (NOT WITH_POSTGRES STREQUAL "OFF")
	cmake_policy(SET CMP0079 NEW)
	if (WITH_POSTGRES STREQUAL "SYSTEM")
		find_package(PostgreSQL REQUIRED)
		message(STATUS "Found PostgreSQL: ${PostgreSQL_VERSION}")
	else()
		# # Build postgres from source (meson + ninja) and expose libpq as an imported target
		# find_program(MESON_EXECUTABLE meson)
		# find_program(NINJA_EXECUTABLE ninja)
		# if (NOT MESON_EXECUTABLE)
		# 	message(FATAL_ERROR "Meson not found: required to build bundled PostgreSQL. Install 'meson'.")
		# endif()
		# if (NOT NINJA_EXECUTABLE)
		# 	message(FATAL_ERROR "Ninja not found: required to build bundled PostgreSQL. Install 'ninja'.")
		# endif()

		# set(POSTGRES_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/postgres")
		# set(POSTGRES_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/build")

		# # Run meson setup during CMake configure so build files exist early.
		# # Only run if the build dir doesn't already contain Ninja build files.
		# if (NOT EXISTS "${POSTGRES_BUILD_DIR}/build.ninja")
		# 	message(STATUS "Configuring bundled PostgreSQL with Meson")
		# 	file(MAKE_DIRECTORY "${POSTGRES_BUILD_DIR}")

		# 	# Start meson setup in background and arrange for its exit code to be written to a file.
		# 	# The shell command prints the started PID so CMake can poll the log while Meson runs.
		# 	execute_process(
		# 		COMMAND ${MESON_EXECUTABLE} setup ${POSTGRES_BUILD_DIR} ${POSTGRES_SRC_DIR} --buildtype=release
		# 		RESULT_VARIABLE MESON_SETUP_RESULT
		# 		OUTPUT_VARIABLE MESON_SETUP_OUT
		# 		ERROR_VARIABLE MESON_SETUP_ERR
		# 	)
		# 	if(MESON_SETUP_OUT)
		# 		message(STATUS "${MESON_SETUP_OUT}")
		# 	endif()
		# 	if(MESON_SETUP_ERR)
		# 		message(STATUS "${MESON_SETUP_ERR}")
		# 	endif()
		# 	if(NOT MESON_SETUP_RESULT EQUAL 0)
		# 		message(FATAL_ERROR "Meson setup failed (exit ${MESON_SETUP_RESULT}). See meson output above for details.")
		# 	endif()
		# else()
		# 	message(STATUS "Skipping meson setup; build files already present")
		# endif()

		# # Compute expected libpq path based on the build directory so the
		# # imported target exists at configure time. The actual library will
		# # be produced during the ExternalProject build step.
		# set(POSTGRES_LIBPQ_DIR "${POSTGRES_BUILD_DIR}/src/interfaces/libpq")
		# set(POSTGRES_LIBPQ "${POSTGRES_LIBPQ_DIR}/libpq${CMAKE_SHARED_LIBRARY_SUFFIX}")

		# if (WIN32)
		# 	set(POSTGRES_LIBPQ_LIB "${POSTGRES_LIBPQ_DIR}/libpq.lib")
		# 	add_custom_command(
		# 		OUTPUT "${POSTGRES_LIBPQ}" "${POSTGRES_LIBPQ_LIB}"
		# 		COMMAND ${NINJA_EXECUTABLE} backend
		# 		WORKING_DIRECTORY ${POSTGRES_BUILD_DIR}
		# 		COMMENT "Building PostgreSQL backend with Ninja"
		# 		USES_TERMINAL
		# 	)
		# 	add_custom_target(postgres_bundled
		# 		DEPENDS "${POSTGRES_LIBPQ}" "${POSTGRES_LIBPQ_LIB}"
		# 	)
		# 	add_library(libpq SHARED IMPORTED)
		# 	set_target_properties(libpq PROPERTIES
		# 		IMPORTED_LOCATION "${POSTGRES_LIBPQ}"
		# 		IMPORTED_IMPLIB   "${POSTGRES_LIBPQ_LIB}"
		# 		IMPORTED_GLOBAL TRUE
		# 	)
		# else()
		# 	message(STATUS "Postgres build dir: ${POSTGRES_BUILD_DIR}")
		# 	add_custom_command(
		# 		OUTPUT "${POSTGRES_LIBPQ}"
		# 		COMMAND ${NINJA_EXECUTABLE} backend
		# 		WORKING_DIRECTORY ${POSTGRES_BUILD_DIR}
		# 		COMMENT "Building PostgreSQL backend with Ninja"
		# 		USES_TERMINAL
		# 	)
		# 	add_custom_target(postgres_bundled
		# 		DEPENDS "${POSTGRES_LIBPQ}"
		# 	)
		# 	add_library(libpq SHARED IMPORTED GLOBAL)
		# 	set_target_properties(libpq PROPERTIES
		# 		IMPORTED_LOCATION "${POSTGRES_LIBPQ}"
		# 		IMPORTED_GLOBAL TRUE
		# 	)
		# endif()
		
		# add_dependencies(libpq postgres_bundled)
		# # Export include dirs from the Postgres source tree so libpq and common headers are found
		# target_include_directories(libpq
		# 	SYSTEM INTERFACE
		# 	"${POSTGRES_SRC_DIR}/src/include"
		# 	"${POSTGRES_SRC_DIR}/src/interfaces/libpq"
		# 	"${POSTGRES_LIBPQ_DIR}"
		# )
		# add_library(PostgreSQL::PostgreSQL ALIAS libpq)

		# message(STATUS "Using PostgreSQL: bundled ${POSTGRES_VERSION}")

		set(POSTGRES_SRCDIR "${CMAKE_CURRENT_LIST_DIR}/src")
		ensure_build_dir(POSTGRES_BUILDDIR)
		set(POSTGRES_COMPONENT "PostgreSQL")
		# Fetch and checkout REL_18_STABLE
		create_git_fetch(POSTGRES_FETCH_SCRIPT "${POSTGRES_COMPONENT}" "${POSTGRES_SRCDIR}")
		create_git_switch_branch(POSTGRES_REL_18_STABLE_SWITCH "${POSTGRES_COMPONENT}" "${POSTGRES_SRCDIR}" "REL_18_STABLE")
		include(${POSTGRES_FETCH_SCRIPT})
		include(${POSTGRES_REL_18_STABLE_SWITCH})

		# Now compile
		set(POSTGRES_OPTIONS
			"-Ddocs=disabled"
		)
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
		include("${POSTGRES_LIBRARY_CREATE_FILE}")
		add_dependencies(StormByte-Database PostgreSQL_install)
		add_library(PostgreSQL::PostgreSQL ALIAS PostgreSQL)
		if(MSVC)
			# On Windows, libraries are named libpq.a, etc, we need to rename them
			# Also, remove pq.lib as it is the IMPLIB for the DLL that is unwanted
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
		else()
			target_link_libraries(StormByte-Database PRIVATE ssl crypto)
		endif()
	endif()

	target_link_libraries(StormByte-Database PRIVATE PostgreSQL::PostgreSQL)
endif()