/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: setup.h

  Link class declarations & system-specific definitions
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 02 Jan 2018 16:51:09 +0000
  Touch : Tue, 02 Jan 2018 18:31:24 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_SETUP_H_
#define _FALCON_SETUP_H_

#include <falcon/config.h>
#include <falcon/types.h>


#ifdef FALCON_SYSTEM_WIN

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
      #undef CDECL
      #define CDECL __cdecl
      #define FALCON_FUNC \
         void CDECL

		#ifndef FALCON_ENGINE_STATIC
			#if defined(FALCON_ENGINE_EXPORTS)
				#define FALCON_DYN_CLASS __declspec(dllexport)
				#define FALCON_DYN_SYM __declspec(dllexport)
				#define EXTERN_TEMPLATE

            // Falcon export service is optional, but mandatory with engine exports.
            #ifndef FALCON_EXPORT_SERVICE
               #define FALCON_EXPORT_SERVICE
            #endif
			#else
				#define FALCON_DYN_CLASS __declspec(dllimport)
				#define FALCON_DYN_SYM __declspec(dllimport)
				#define EXTERN_TEMPLATE export
			#endif
		#else
			#define FALCON_DYN_CLASS
			#define FALCON_DYN_SYM
		#endif

      #ifdef FALCON_EXPORT_SERVICE
         #define FALCON_SERVICE __declspec(dllexport)
      #else
         #define FALCON_SERVICE __declspec(dllimport)
      #endif

      #define FALCON_FUNC_DYN_SYM \
		   FALCON_DYN_SYM void CDECL

      #define FALCON_MODULE_TYPE \
		   extern "C" __declspec(dllexport) ::Falcon::Module * CDECL


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
   	#endif

      #define atoll _atoi64
      #define snprintf _snprintf
      #define LLFMT    "I64"
      #define I64LIT(x) (x ## i64)
      #define UI64LIT(x) (x ## ui64)
   #endif

   /* Specifics for Gcc/Mingw */
   #ifdef __GNUC__
	   #ifndef CDECL
		#define CDECL
	   #endif
	   #define FALCON_FUNC \
	      void

	   #ifdef FALCON_ENGINE_EXPORTS
			#define FALCON_DYN_CLASS __declspec(dllexport)
			#define FALCON_DYN_SYM __declspec(dllexport)
			#define EXTERN_TEMPLATE
		#else
			#define FALCON_DYN_CLASS __declspec(dllimport)
			#define FALCON_DYN_SYM __declspec(dllimport)
			#define EXTERN_TEMPLATE export
		#endif

      #ifdef FALCON_EXPORT_SERVICE
         #define FALCON_SERVICE __declspec(dllexport)
      #else
         #define FALCON_SERVICE __declspec(dllimport)
      #endif

      #define FALCON_FUNC_DYN_SYM \
		   FALCON_DYN_SYM void CDECL

	   #define FALCON_MODULE_TYPE \
	      extern "C" __declspec(dllexport) ::Falcon::Module *
      #define LLFMT    "ll"
      #define I64LIT(x) (x ## LL)
      #define UI64LIT(x) (x ## ULL)
	#endif

   /* Other Windonws specific system defines */

   #define DIR_SEP_STR   "\\"
   #define DIR_SEP_CHR   '\\'
   // paths are always indicated in falcon convention.
   #define DEFAULT_TEMP_DIR "C:/TEMP"
   #define FALCON_SYS_EOL "\r\n"

//=================================
// Unix specific defines
//
#else
   #define CDECL

   #define FALCON_DYN_CLASS
   #define FALCON_DYN_SYM
   #define EXTERN_TEMPLATE
   #define FALCON_SERVICE

   #define FALCON_MODULE_TYPE \
      extern "C" ::Falcon::Module *

   #define FALCON_FUNC_DYN_SYM   FALCON_FUNC

   #define DIR_SEP_STR   "/"
   #define DIR_SEP_CHR   '/'
   #define DEFAULT_TEMP_DIR "/tmp"
   #define FALCON_SYS_EOL "\n"
   #define LLFMT "ll"
   #define I64LIT(x) (x ## LL)
   #define UI64LIT(x) (x ## ULL)

#endif

}

#endif /* _FALCON_SETUP_H_ */

/* end of setup.h */

