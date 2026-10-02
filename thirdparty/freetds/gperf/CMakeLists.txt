buildmaster_component(
	gperf
	"GNU gperf code generator"
	"${CMAKE_CURRENT_LIST_DIR}/src"
	"BUILD_TESTS=OFF;CMAKE_CXX_STANDARD=14;CMAKE_INSTALL_INCLUDEDIR=include/gperf"
	executable
	gperf
)
