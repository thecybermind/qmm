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

/* $Id: version.h,v 1.9 2006/03/04 22:33:29 cybermind Exp $ */

#ifndef __VERSION_H__
#define __VERSION_H__

#define QMM_VERSION_MAJOR	1
#define QMM_VERSION_MINOR	1
#define QMM_VERSION_REV		3

#define QMM_VERSION		"1.1.3"

#define QMM_COMPILE		__TIME__ " " __DATE__
#define QMM_BUILDER		"CyberMind"

#ifdef WIN32
 #define QMM_OS			"Win32"
#elif defined(linux)
 #define QMM_OS			"Linux"
#endif

//this gets updated automatically, we don't care about this
#define QMM_VERSION_DWORD	QMM_VERSION_MAJOR , QMM_VERSION_MINOR , QMM_VERSION_REV , 0

#endif //__VERSION_H__
