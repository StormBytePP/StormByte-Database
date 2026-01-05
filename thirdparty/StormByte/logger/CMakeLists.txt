set(STORMBYTE_LOGGER_SRCDIR "${CMAKE_CURRENT_LIST_DIR}/src")
ensure_build_dir(STORMBYTE_LOGGER_BUILDDIR)
set(STORMBYTE_LOGGER_COMPONENT "StormByte Suite - Logger")
set(STORMBYTE_LOGGER_OPTIONS
	"-DWITH_STORMBYTE=BUNDLED"
)
create_cmake_component(
	STORMBYTE_LOGGER_LIBRARY_CREATE_FILE
	"StormByte-Logger"
	"${STORMBYTE_LOGGER_COMPONENT}"
	"${STORMBYTE_LOGGER_SRCDIR}"
	"${STORMBYTE_LOGGER_BUILDDIR}"
	"${STORMBYTE_LOGGER_OPTIONS}"
	"shared"
	"StormByte-Logger;StormByte"
	1
)
include("${STORMBYTE_LOGGER_LIBRARY_CREATE_FILE}")
add_dependencies(StormByte-Database StormByte-Logger_install)
add_library(StormByte::Logger ALIAS StormByte-Logger)