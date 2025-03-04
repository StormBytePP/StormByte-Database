# Look for the necessary header
find_path(StormByte_INCLUDE_DIR NAMES "StormByte/visibility.h")
mark_as_advanced(StormByte_INCLUDE_DIR)

# Look for the necessary library
find_library(StormByte_LIBRARY NAMES StormByte)
mark_as_advanced(StormByte_LIBRARY)

# Extract version information from the header file
if(StormByte_INCLUDE_DIR)
	set(StormByte_FOUND TRUE)
	set(StormByte_INCLUDE_DIRS "${StormByte_INCLUDE_DIR}")
	set(StormByte_LIBRARIES "${StormByte_LIBRARY}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(StormByte
    REQUIRED_VARS StormByte_INCLUDE_DIR StormByte_LIBRARY StormByte_LIBRARIES
    HANDLE_COMPONENTS
)

# Create the imported target
if(StormByte_FOUND)
    if(NOT TARGET StormByte)
        add_library(StormByte UNKNOWN IMPORTED GLOBAL)
        set_target_properties(StormByte PROPERTIES
			IMPORTED_LOCATION 				"${StormByte_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES 	"${StormByte_INCLUDE_DIRS}"
			INTERFACE_LINK_LIBRARIES      	"${StormByte_LIBRARIES}"
		)
    endif()
endif()
