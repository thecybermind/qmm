/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __OSDEF_H__
#define __OSDEF_H__

#ifdef _WIN32
 #ifndef WIN32
  #define WIN32
 #endif
#endif

#ifdef WIN32
 #ifdef linux
  #undef linux
 #endif
#else
 #ifndef linux
  #define linux
 #endif
#endif

#ifdef WIN32
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
 #define DLL_SUF			"x86"
 #define PDB_LIB			"pdb.dll"
 #define DLL_EXT			".dll"
 #define QVM_EXT			".qvm"
 #define vsnprintf			_vsnprintf
 #define strcasecmp			stricmp
 #define dlopen(file, x)	(void*)LoadLibrary(file)
 #define dlsym(dll, func)	(void*)GetProcAddress((HMODULE)(dll), (func))
 #define dlclose(dll)		FreeLibrary((HMODULE)(dll))
 char* dlerror();			//this returns the last error from anything, not just library functions

 #pragma warning(disable:4710)	//"function * not inlined"

#else

 #include <dlfcn.h>
 #include <limits.h>
 #include <ctype.h>
 #define DLL_SUF		"i386"
 #define PDB_LIB		"./pdb.so"
 #define DLL_EXT		".so"
 #define QVM_EXT		".qvm"
 #define MAX_PATH		PATH_MAX
#endif

#endif //__OSDEF_H__
