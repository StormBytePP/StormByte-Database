option(ENABLE_ASAN "Enable AddressSanitizer (and UBSan) for debug builds" OFF)

if(ENABLE_ASAN AND NOT STORMBYTE_AS_DEPENDENCY AND NOT WIN32)
	# Recommend Debug builds for ASan
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

	# GCC/Clang sanitizers
	set(SANITIZER_FLAGS "-fsanitize=address,undefined")

	# Set global environment for all ctest runs.
	set_property(GLOBAL PROPERTY ENVIRONMENT 
		"ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:strict_string_checks=1:alloc_dealloc_mismatch=1;UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1"
	)

	# Inform about ASan being enabled
	message(STATUS "AddressSanitizer and UndefinedBehaviorSanitizer enabled")
endif()