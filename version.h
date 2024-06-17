/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __VERSION_H__
#define __VERSION_H__

#define QMM_VERSION_MAJOR	1
#define QMM_VERSION_MINOR	1
#define QMM_VERSION_REV		3

#define QMM_VERSION		"1.1.3"

#define QMM_COMPILE		__TIME__ " " __DATE__
#define QMM_BUILDER		"cybermind"

#ifdef WIN32
 #define QMM_OS			"Win32"
#elif defined(linux)
 #define QMM_OS			"Linux"
#endif

//this gets updated automatically, we don't care about this
#define QMM_VERSION_DWORD	QMM_VERSION_MAJOR , QMM_VERSION_MINOR , QMM_VERSION_REV , 0

#endif //__VERSION_H__
