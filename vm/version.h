/*

QMMVM - VM Wrapper Tool
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
    VM code from Q3Fusion (http://www.sourceforge.net/projects/q3fusion/)
    VM info from Phaethon (http://www.icculus.org/~phaethon/q3mc/q3vm_specs.html)
    Code3Arena (http://www.planetquake.com/code3arena/)

*/

/* $Id: version.h,v 1.14 2004/08/30 00:45:36 cybermind Exp $ */

#ifndef __VERSION_H__
#define __VERSION_H__

#define QMMVM_VERSION		"0.4.0b"
#define QMMVM_VERSION_DWORD	0,4,0,0
#define QMMVM_COMPILE		__TIME__ " " __DATE__
#define QMMVM_BUILDER		"CyberMind"

//#define GAME_Q3A
//#define GAME_JK2

//set default mod dir
#if defined(GAME_JK2)
 #define QMMVM_DEFAULT_MODDIR	"base"
 #define QMMVM_DEFAULT_VMPATH	"vm/jk2mpgame.qvm"
 #define QMMVM_GAME		"JK2"
 #undef GAME_Q3A
#elif defined(GAME_Q3A)
 #define QMMVM_DEFAULT_MODDIR	"baseq3"
 #define QMMVM_DEFAULT_VMPATH	"vm/qagame.qvm"
 #define QMMVM_GAME		"Q3A"
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

#define QMMVM_G_MALLOC		1000
#define QMMVM_G_REALLOC		1001
#define QMMVM_G_FREE		1002

#ifdef WIN32
 #define QMMVM_OS		"Win32"
#else
 #define QMMVM_OS		"Linux"
#endif

#define PARAM_OPCODE 2
#define DEBUG_OPCODE 3

#endif //__VERSION_H__
