/*

QADMIN_QMM - QMM Administration Plugin
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

*/

/* $Id: util.h,v 1.4 2006/03/08 08:25:01 cybermind Exp $ */

#ifndef __UTIL_H__
#define __UTIL_H__

bool has_access(int, int);
int user_with_ip(const char*, int = -1);
int arrsize(admincmd_t*);
void ClientPrint(int, const char*, bool = 0);
char* concatargs(int);
char** tok_parse(const char*, char = ' ');
void tok_free(char**);
void setcvar(char*, int);
const char* StripCodes(const char*);
const char* lcase(const char*);
int namematch(const char*, bool = 0, int = -1);
bool is_valid_map(const char*);

char* Info_ValueForKey(const char*, const char*);
qboolean Info_Validate(const char*);

#endif //__UTIL_H__
