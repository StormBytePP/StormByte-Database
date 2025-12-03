option(ENABLE_TEST "Enable Unit Tests" OFF)
if(ENABLE_TEST AND NOT STORMBYTE_AS_DEPENDENCY)
	enable_testing()

	add_executable(DummyTests dummy_test.cxx)
	target_link_libraries(DummyTests StormByte::Database)
	add_test(NAME DummyTests COMMAND DummyTests)
	
	if (ENABLE_SQLITE)
		add_executable(SQLiteTests sqlite_test.cxx)
		target_link_libraries(SQLiteTests StormByte::Database)
		add_test(NAME SQLiteTests COMMAND SQLiteTests)
	endif()
endif()