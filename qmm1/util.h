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

/* $Id: util.h,v 1.7 2005/10/12 22:57:57 cybermind Exp $ */

#ifndef __UTIL_H__
#define __UTIL_H__

const char* get_modulename();
const char* get_modulepath();
void* get_modulehandle();
//int ismoduleloaded(void*);
int ismoduleloaded(void*);
void setmoduleloaded(void*);
void setmoduleunloaded(void*);
int byteswap(int);
short byteswap(short);
char* vaf(char*, ...);
//const char* opcodename(int);
extern const char* opcodename[];
int get_int_cvar(const char*);
const char* get_str_cvar(const char*);
void log_set(int);
int log_get();
int log_write(const char*, int = -1);
const char* my_strcasestr(const char*, const char*);
int isnumber(const char*);
int write_file(const char*, const char* = NULL);

#endif //__UTIL_H__
