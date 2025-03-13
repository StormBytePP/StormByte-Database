#pragma once

#include <StormByte/platform.h>

#ifdef WINDOWS
	#ifdef StormByte_Database_EXPORTS
		#define STORMBYTE_DATABASE_PUBLIC	__declspec(dllexport)
  	#else
      	#define STORMBYTE_DATABASE_PUBLIC	__declspec(dllimport)
  	#endif
  	#define STORMBYTE_DATABASE_PRIVATE
#else
    #define STORMBYTE_DATABASE_PUBLIC		__attribute__ ((visibility ("default")))
    #define STORMBYTE_DATABASE_PRIVATE		__attribute__ ((visibility ("hidden")))
#endif
