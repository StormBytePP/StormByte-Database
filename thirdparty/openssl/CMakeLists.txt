if(WIN32)
	set(_openssl_link "LINK={ws2_32;crypt32}")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(_openssl_link "LINK={pthread;dl}")
endif()

buildmaster_component(
	BundledOpenSSL
	"OpenSSL 3.5.9"
	"${CMAKE_CURRENT_LIST_DIR}/cmake-port"
	"OPENSSL_SOURCE=${CMAKE_CURRENT_LIST_DIR}/src;OPENSSL_TARGET_VERSION=3.5.9;OPENSSL_INSTALL=ON;OPENSSL_LIBDIR=${CMAKE_INSTALL_LIBDIR}"
	static
	"ssl;crypto"
	"ALIAS=OpenSSL;${_openssl_link}"
)