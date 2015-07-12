/*
QMM - Q3 MultiMod
Copyright QMM Team 2005
http://www.q3mm.org/

Licensing:
    QMM is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    QMM is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QMM; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Created By:
    Kevin Masterson a.k.a. CyberMind <kevinm@planetquake.com>

*/

/* $Id: osdef.h,v 1.6 2005/09/21 02:05:03 cybermind Exp $ */

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
 #define DLL_SUF		"x86"
 #define PDB_LIB		"pdb.dll"
 #define DLL_EXT		".dll"
 #define QVM_EXT		".qvm"
 #define vsnprintf		_vsnprintf
 #define strcasecmp		stricmp
 #define dlopen(file, x)	(void*)LoadLibrary(file)
 #define dlsym(dll, func)	(void*)GetProcAddress((HMODULE)(dll), (func))
 #define dlclose(dll)		FreeLibrary((HMODULE)(dll))
 char* dlerror();		//this returns the last error from anything, not just library functions

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
