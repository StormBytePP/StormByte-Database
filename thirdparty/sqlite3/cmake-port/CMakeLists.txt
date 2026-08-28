cmake_minimum_required(VERSION 3.12)
project(sqlite3 LANGUAGES C)

include(GNUInstallDirs)

set(_amalg "${BUILDMASTER_BINDIR}/files/sqlite3/sqlite-amalgamation-3530400")
if(NOT EXISTS "${_amalg}/sqlite3.c")
	message(FATAL_ERROR "sqlite3 amalgamation missing under ${_amalg} (FILES unpack)")
endif()

add_library(sqlite3 STATIC "${_amalg}/sqlite3.c")
target_include_directories(sqlite3
	PUBLIC
		$<BUILD_INTERFACE:${_amalg}>
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
install(FILES "${_amalg}/sqlite3.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
