# Initialize StormByte BuildMaster
add_subdirectory(buildmaster)
include(buildmaster/helpers.cmake)

# Add rest of dependencies
add_subdirectory(StormByte)
add_subdirectory(sqlite3)
add_subdirectory(postgres)
add_subdirectory(mariadb)