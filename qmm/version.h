/*

QMM - Q3 MultiMod
Copyright QMM Team 2004
http://www.quake3mm.net/

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
    Kevin Masterson a.k.a. CyberMind <cybermind@users.sourceforge.net>

Special Thanks:
    Code3Arena (http://www.planetquake.com/code3arena/)

*/

/* $Id: version.h,v 1.13 2004/08/25 21:08:20 cybermind Exp $ */

#ifndef __VERSION_H__
#define __VERSION_H__

#define QMM_VERSION		"0.4.0b"
#define QMM_VERSION_DWORD	0,4,0,0
#define QMM_COMPILE		__TIME__ " " __DATE__
#define QMM_BUILDER		"CyberMind"

//#define GAME_Q3A
//#define GAME_COD
//#define GAME_RTCW
//#define GAME_JKA
//#define GAME_JK2

//set default mod dir
#if defined(GAME_COD)
 #define QMM_DEFAULT_MODDIR	"main"
 #define QMM_GAME		"COD"
 #undef GAME_Q3A
 #undef GAME_RTCW
 #undef GAME_JKA
 #undef GAME_JK2
#elif defined(GAME_RTCW)
 #ifdef WIN32
  #define QMM_DEFAULT_MODDIR	"Main"
 #else
  #define QMM_DEFAULT_MODDIR	"main"
 #endif
 #define QMM_GAME		"RTCW"
 #undef GAME_Q3A
 #undef GAME_JKA
 #undef GAME_JK2
#elif defined(GAME_JKA)
 #define QMM_DEFAULT_MODDIR	"base"
 #define QMM_GAME		"JKA"
 #undef GAME_Q3A
 #undef GAME_JK2
#elif defined(GAME_JK2)
 #define QMM_DEFAULT_MODDIR	"base"
 #define QMM_GAME		"JK2"
 #undef GAME_Q3A
#elif defined(GAME_Q3A)
 #define QMM_DEFAULT_MODDIR	"baseq3"
 #define QMM_GAME		"Q3A"
 #define G_MEMSET		100
 #define G_MEMCPY		101
 #define G_STRNCPY		102
 #define G_SIN			103
 #define G_COS			104
 #define G_ATAN2		105
 #define G_SQRT			106
 #define G_FLOOR		110
 #define G_CEIL			111
 #define G_TESTPRINTINT		112
 #define G_TESTPRINTFLOAT	113
#else
 #ifndef MSVC_RC
  #error You must define a GAME_ value in version.h or on compiler command line
 #endif
#endif

#ifdef WIN32
 #define QMM_OS			"Win32"
 #define QMM_DEFAULT_MODFILE	"qmmvm.dll"
#else
 #define QMM_OS			"Linux"
 #define QMM_DEFAULT_MODFILE	"qmmvmi386.so"
#endif

#endif //__VERSION_H__
