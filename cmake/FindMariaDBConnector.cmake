# ---------------------------------------------------------------------------
# FindMariaDBConnector.cmake
#
# This CMake "Find" module locates MariaDB Connector-C (libmariadb / libmysql)
# When the connector is found this module sets the following variables:
#  - MARIADB_CONNECTOR_FOUND
#  - MARIADB_CONNECTOR_STATIC_FOUND
#  - MARIADB_CONNECTOR_LIBRARIES
#  - MARIADB_CONNECTOR_STATIC_LIBRARIES
#  - MARIADB_CONNECTOR_INCLUDE_DIR
#
# Additionally, if the connector is available an IMPORTED target is created:
#  - MariaDB::ConnectorC
#
# The imported target has these properties (when available):
#  - IMPORTED_LOCATION -> points to the dynamic or static library
#  - INTERFACE_INCLUDE_DIRECTORIES -> include dir to use when consuming target
#
# The finder supports the standard `find_package(... REQUIRED)` behavior via
# `FindPackageHandleStandardArgs` so callers can request REQUIRED.
# ---------------------------------------------------------------------------
# This CMake file tries to find the the MariaDB Connector-C
# The following variables are set:
# MARIADB_CONNECTOR_FOUND - System has the connector
# MARIADB_CONNECTOR_STATIC_FOUND - System has static version of the connector library
# MARIADB_CONNECTOR_LIBRARIES - The dynamic connector libraries
# MARIADB_CONNECTOR_STATIC_LIBRARIES - The static connector libraries
# MARIADB_CONNECTOR_INCLUDE_DIR - The connector headers

find_library(MARIADB_CONNECTOR_LIBRARIES NAMES mariadbclient mariadb mysqlclient PATH_SUFFIXES mariadb mysql)
if(${MARIADB_CONNECTOR_LIBRARIES} MATCHES "NOTFOUND")
  set(MARIADB_CONNECTOR_FOUND FALSE CACHE INTERNAL "")
  message(STATUS "Dynamic MySQL client library not found.")
  unset(MARIADB_CONNECTOR_LIBRARIES)
else()
  set(MARIADB_CONNECTOR_FOUND TRUE CACHE INTERNAL "")
  message(STATUS "Found dynamic MySQL client library: ${MARIADB_CONNECTOR_LIBRARIES}")
endif()

set(OLD_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
find_library(MARIADB_CONNECTOR_STATIC_LIBRARIES NAMES mariadbclient mariadb mysqlclient PATH_SUFFIXES mariadb mysql)
set(CMAKE_FIND_LIBRARY_SUFFIXES ${OLD_SUFFIXES})

if(${MARIADB_CONNECTOR_STATIC_LIBRARIES} MATCHES "NOTFOUND")
  set(MARIADB_CONNECTOR_STATIC_FOUND FALSE CACHE INTERNAL "")
  message(STATUS "Static MySQL client library not found.")
  unset(MARIADB_CONNECTOR_STATIC_LIBRARIES)
else()
  set(MARIADB_CONNECTOR_STATIC_FOUND TRUE CACHE INTERNAL "")
  message(STATUS "Found statc MySQL client library: ${MARIADB_CONNECTOR_STATIC_LIBRARIES}")
endif()

find_path(MARIADB_CONNECTOR_INCLUDE_DIR mysql.h PATH_SUFFIXES mariadb mysql)

if(NOT (${MARIADB_CONNECTOR_INCLUDE_DIR}  MATCHES "NOTFOUND"))
  include(CheckSymbolExists)
  set(CMAKE_REQUIRED_INCLUDES ${MARIADB_CONNECTOR_INCLUDE_DIR})
  check_symbol_exists(LIBMARIADB mysql.h HAVE_MARIADB_CONNECTOR)
endif()

if(HAVE_MARIADB_CONNECTOR)
  message(STATUS "Found MariaDB Connector-C")
  set(MARIADB_CONNECTOR_FOUND TRUE CACHE INTERNAL "" FORCE)
else()
  set(MARIADB_CONNECTOR_FOUND FALSE CACHE INTERNAL "")
  unset(MARIADB_CONNECTOR_STATIC_FOUND)
  unset(MARIADB_CONNECTOR_LIBRARIES)
  unset(MARIADB_CONNECTOR_STATIC_LIBRARIES)
  unset(MARIADB_CONNECTOR_INCLUDE_DIR)
endif()

# Use FindPackageHandleStandardArgs so callers can use `find_package(... REQUIRED)`
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MariaDBConnector DEFAULT_MSG MARIADB_CONNECTOR_LIBRARIES MARIADB_CONNECTOR_INCLUDE_DIR)

# Mirror the canonical Find results into the module-style variable used elsewhere
if(DEFINED MariaDBConnector_FOUND)
  set(MARIADB_CONNECTOR_FOUND ${MariaDBConnector_FOUND} CACHE INTERNAL "" FORCE)
elseif(NOT DEFINED MARIADB_CONNECTOR_FOUND)
  set(MARIADB_CONNECTOR_FOUND FALSE CACHE INTERNAL "")
endif()

# If we successfully found connector artifacts, provide an imported target
if(MARIADB_CONNECTOR_FOUND)
  if(NOT TARGET MariaDB::ConnectorC)
    add_library(MariaDB::ConnectorC UNKNOWN IMPORTED GLOBAL)

    # Prefer the dynamic library if available, otherwise use static
    if(DEFINED MARIADB_CONNECTOR_LIBRARIES AND MARIADB_CONNECTOR_LIBRARIES)
      list(GET MARIADB_CONNECTOR_LIBRARIES 0 _mariadb_connector_lib)
      if(_mariadb_connector_lib)
        set_target_properties(MariaDB::ConnectorC PROPERTIES IMPORTED_LOCATION "${_mariadb_connector_lib}")
      endif()
    elseif(DEFINED MARIADB_CONNECTOR_STATIC_LIBRARIES AND MARIADB_CONNECTOR_STATIC_LIBRARIES)
      list(GET MARIADB_CONNECTOR_STATIC_LIBRARIES 0 _mariadb_connector_lib)
      if(_mariadb_connector_lib)
        set_target_properties(MariaDB::ConnectorC PROPERTIES IMPORTED_LOCATION "${_mariadb_connector_lib}")
      endif()
    endif()

    if(DEFINED MARIADB_CONNECTOR_INCLUDE_DIR AND MARIADB_CONNECTOR_INCLUDE_DIR)
      set_target_properties(MariaDB::ConnectorC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${MARIADB_CONNECTOR_INCLUDE_DIR}")
    endif()

    message(STATUS "Created imported target MariaDB::ConnectorC")
  endif()
endif()
