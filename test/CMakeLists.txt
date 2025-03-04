option(ENABLE_TEST "Enable Unit Tests" OFF)
if(ENABLE_TEST)
	enable_testing()

	add_executable(SQLiteTests sqlite_test.cxx)
	target_link_libraries(SQLiteTests StormByte::Database)
	add_test(NAME SQLiteTests COMMAND SQLiteTests)
endif()