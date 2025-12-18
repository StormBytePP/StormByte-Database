IF(WIN32)
	set(WITH_POSTGRES "BUNDLED" CACHE STRING "Postgres mode: OFF, SYSTEM, BUNDLED" FORCE)
else()
	set(WITH_POSTGRES "SYSTEM" CACHE STRING "Postgres mode: OFF, SYSTEM, BUNDLED")
endif()
if (NOT WITH_POSTGRES STREQUAL "OFF")
	if (WITH_POSTGRES STREQUAL "SYSTEM")
		find_package(PostgreSQL REQUIRED)
		message(STATUS "Found PostgreSQL: ${PostgreSQL_VERSION}")
	else()
		# Build postgres from source (meson + ninja) and expose libpq as an imported target
		find_program(MESON_EXECUTABLE meson)
		find_program(NINJA_EXECUTABLE ninja)
		if (NOT MESON_EXECUTABLE)
			message(FATAL_ERROR "Meson not found: required to build bundled PostgreSQL. Install 'meson'.")
		endif()
		if (NOT NINJA_EXECUTABLE)
			message(FATAL_ERROR "Ninja not found: required to build bundled PostgreSQL. Install 'ninja'.")
		endif()

		set(POSTGRES_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/postgres")
		set(POSTGRES_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/build")

		# Run meson setup during CMake configure so build files exist early.
		# Only run if the build dir doesn't already contain Ninja build files.
		if (NOT EXISTS "${POSTGRES_BUILD_DIR}/build.ninja")
			message(STATUS "Configuring bundled PostgreSQL with Meson")
			file(MAKE_DIRECTORY "${POSTGRES_BUILD_DIR}")

			# Start meson setup in background and arrange for its exit code to be written to a file.
			# The shell command prints the started PID so CMake can poll the log while Meson runs.
			execute_process(
				COMMAND ${MESON_EXECUTABLE} setup ${POSTGRES_BUILD_DIR} ${POSTGRES_SRC_DIR} --buildtype=release
				RESULT_VARIABLE MESON_SETUP_RESULT
				OUTPUT_VARIABLE MESON_SETUP_OUT
				ERROR_VARIABLE MESON_SETUP_ERR
			)
			if(MESON_SETUP_OUT)
				message(STATUS "${MESON_SETUP_OUT}")
			endif()
			if(MESON_SETUP_ERR)
				message(STATUS "${MESON_SETUP_ERR}")
			endif()
			if(NOT MESON_SETUP_RESULT EQUAL 0)
				message(FATAL_ERROR "Meson setup failed (exit ${MESON_SETUP_RESULT}). See meson output above for details.")
			endif()
		else()
			message(STATUS "Skipping meson setup; build files already present")
		endif()

		# Compute expected libpq path based on the build directory so the
		# imported target exists at configure time. The actual library will
		# be produced during the ExternalProject build step.
		set(POSTGRES_LIBPQ_DIR "${POSTGRES_BUILD_DIR}/src/interfaces/libpq")
		set(POSTGRES_LIBPQ "${POSTGRES_LIBPQ_DIR}/libpq${CMAKE_SHARED_LIBRARY_SUFFIX}")

		if (WIN32)
			set(POSTGRES_LIBPQ_LIB "${POSTGRES_LIBPQ_DIR}/libpq.lib")
			add_custom_command(
				OUTPUT "${POSTGRES_LIBPQ}" "${POSTGRES_LIBPQ_LIB}"
				COMMAND ${NINJA_EXECUTABLE} backend
				WORKING_DIRECTORY ${POSTGRES_BUILD_DIR}
				COMMENT "Building PostgreSQL backend with Ninja"
				USES_TERMINAL
			)
			add_custom_target(postgres_bundled
				DEPENDS "${POSTGRES_LIBPQ}" "${POSTGRES_LIBPQ_LIB}"
			)
			add_library(libpq SHARED IMPORTED)
			set_target_properties(libpq PROPERTIES
				IMPORTED_LOCATION "${POSTGRES_LIBPQ}"
				IMPORTED_IMPLIB   "${POSTGRES_LIBPQ_LIB}"
				IMPORTED_LOCATION_DEBUG "${POSTGRES_LIBPQ}"
				IMPORTED_LOCATION_RELEASE "${POSTGRES_LIBPQ}"
				IMPORTED_IMPLIB_DEBUG "${POSTGRES_LIBPQ_LIB}"
				IMPORTED_IMPLIB_RELEASE "${POSTGRES_LIBPQ_LIB}"
				IMPORTED_GLOBAL TRUE
			)
		else()
			message(STATUS "Postgres build dir: ${POSTGRES_BUILD_DIR}")
			add_custom_command(
				OUTPUT "${POSTGRES_LIBPQ}"
				COMMAND ${NINJA_EXECUTABLE} backend
				WORKING_DIRECTORY ${POSTGRES_BUILD_DIR}
				COMMENT "Building PostgreSQL backend with Ninja"
				USES_TERMINAL
			)
			add_custom_target(postgres_bundled
				DEPENDS "${POSTGRES_LIBPQ}"
			)
			add_library(libpq SHARED IMPORTED GLOBAL)
			set_target_properties(libpq PROPERTIES
				IMPORTED_LOCATION "${POSTGRES_LIBPQ}"
				IMPORTED_LOCATION_DEBUG "${POSTGRES_LIBPQ}"
				IMPORTED_LOCATION_RELEASE "${POSTGRES_LIBPQ}"
				IMPORTED_GLOBAL TRUE
			)
		endif()
		
		add_dependencies(libpq postgres_bundled)
		# Export include dirs from the Postgres source tree so libpq and common headers are found
		target_include_directories(libpq
			SYSTEM INTERFACE
			"${POSTGRES_SRC_DIR}/src/include"
			"${POSTGRES_SRC_DIR}/src/interfaces/libpq"
			"${POSTGRES_LIBPQ_DIR}"
		)
		add_library(PostgreSQL::PostgreSQL ALIAS libpq)

		message(STATUS "Using PostgreSQL: bundled ${POSTGRES_VERSION}")
	endif()

	cmake_policy(SET CMP0079 NEW)
	target_link_libraries(StormByte-Database PRIVATE PostgreSQL::PostgreSQL)
endif()