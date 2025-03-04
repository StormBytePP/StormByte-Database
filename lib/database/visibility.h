#pragma once

#include <definitions.h>

#ifdef WINDOWS
	#ifdef StormByteDatabase_EXPORTS
		#define STORMBYTE_DATABASE_PUBLIC	__declspec(dllexport)
  	#else
      	#define STORMBYTE_DATABASE_PUBLIC	__declspec(dllimport)
  	#endif
  	#define STORMBYTE_DATABASE_PRIVATE
#else
    #define STORMBYTE_DATABASE_PUBLIC		__attribute__ ((visibility ("default")))
    #define STORMBYTE_DATABASE_PRIVATE		__attribute__ ((visibility ("hidden")))
#endif
