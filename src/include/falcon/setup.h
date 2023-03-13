/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: setup.h

  Link class declarations & system-specific definitions
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 02 Jan 2018 16:51:09 +0000
  Touch : Tue, 09 Jan 2018 23:27:02 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_SETUP_H_
#define _FALCON_SETUP_H_

#include <falcon/config.h>
#include <falcon/types.h>


#ifdef FALCON_SYSTEM_WIN

	#if defined(FALCON_ENGINE_EXPORTS)
	   #define FALCON_API_ __declspec(dllexport)
	#else
	   #define FALCON_API_ __declspec(dllimport)
	#endif

   // Minimal specific.
   #if ! defined(_WIN32_WINNT)
      #define _WIN32_WINNT 0x0501 //0x0403
   #endif

   #ifndef NOMINMAX
      #define NOMINMAX
   #endif

   //===============================
   // Compiler specific defines
   //


   /* Specifigs for MSVC */
   #ifdef _MSC_VER
      #ifdef FALCON_EXPORT_SERVICE
         #define FALCON_SERVICE __declspec(dllexport)
      #else
         #define FALCON_SERVICE __declspec(dllimport)
      #endif

      #pragma warning (disable: 4786 )
      #pragma warning (disable: 4291 )
      #pragma warning (disable: 579 )
      #pragma warning (disable: 4290 )
      #pragma warning (disable: 4231 )
      #pragma warning (disable: 4355)
      #pragma warning (disable: 4996)

   #if _MSC_VER <= 1400
   		#ifndef _WCHAR_T_DEFINED
   			typedef unsigned short wchar_t;
   		#endif
		#define snprintf _snprintf
   #endif

      #define atoll _atoi64
      #define LLFMT    "I64"
      #define I64LIT(x) (x ## i64)
      #define UI64LIT(x) (x ## ui64)
   #endif

   /* Specifics for Gcc/Mingw */
   #ifdef __GNUC__

	   #define FALCON_MODULE_TYPE \
	      extern "C" __declspec(dllexport) ::falcon::Module *
      #define LLFMT    "ll"
      #define I64LIT(x) (x ## LL)
      #define UI64LIT(x) (x ## ULL)
	#endif

   /* Other Windonws specific system defines */

   #define FALCON_DIR_SEP_STR   "\\"
   #define FALCON_DIR_SEP_CHR   '\\'
   // paths are always indicated in falcon convention.
   #define FALCON_DEFAULT_TEMP_DIR "C:/TEMP"
   #define FALCON_SYS_EOL "\r\n"

//=================================
// Unix specific defines
//
#else
   #define FALCON_API_

   #define FALCON_DIR_SEP_STR   "/"
   #define FALCON_DIR_SEP_CHR   '/'
   #define FALCON_DEFAULT_TEMP_DIR "/tmp"
   #define FALCON_SYS_EOL "\n"
   #define LLFMT "ll"
   #define I64LIT(x) (x ## LL)
   #define UI64LIT(x) (x ## ULL)

#endif

#endif /* _FALCON_SETUP_H_ */

/* end of setup.h */

