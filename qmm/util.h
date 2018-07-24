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

/* $Id: util.h,v 1.6 2004/08/23 07:32:27 cybermind Exp $ */

#ifndef __UTIL_H__
#define __UTIL_H__

//Format a string
char* vaf(char*, ...);
//find CPlugin with given id, return ptr
CPlugin* findplugin(int);

//return a string name for the int
const char* modfuncname(int);
//return a string name for the int
const char* engfuncname(int);

#endif //__UTIL_H__
