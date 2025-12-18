# Install (on Windows only install DLLs)
if (WIN32)
	install(TARGETS StormByte StormByte-Logger StormByte-Database libmariadb sqlite3
		RUNTIME 		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(IMPORTED_RUNTIME_ARTIFACTS libpq DESTINATION ${CMAKE_INSTALL_BINDIR})
elseif (NOT STORMBYTE_AS_DEPENDENCY)
	install(TARGETS StormByte-Database
		ARCHIVE 		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY 		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME 		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/public/StormByte/"
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte"
		FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	)
	if (WITH_SQLITE STREQUAL "BUNDLED")
		install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/optional/StormByte/database/sqlite"
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte/database"
			FILES_MATCHING
			PATTERN "*.h"
			PATTERN "*.hxx"
		)
	endif()
	if (WITH_POSTGRES STREQUAL "BUNDLED")
		install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/optional/StormByte/database/postgres"
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte/database"
			FILES_MATCHING
			PATTERN "*.h"
			PATTERN "*.hxx"
		)
	endif()
	if (WITH_MARIADB STREQUAL "BUNDLED")
		install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/optional/StormByte/database/mariadb"
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StormByte/database"
			FILES_MATCHING
			PATTERN "*.h"
			PATTERN "*.hxx"
		)
	endif()
endif()