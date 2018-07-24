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

/* $Id: plugin.cpp,v 1.9 2006/01/29 22:45:37 cybermind Exp $ */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "CModMgr.h"
#include "qmm.h"
#include "qmmapi.h"
#include "util.h"
#include "CLinkList.h"

static int plugin_WriteGameLog(const char* text, int len = -1) {
	return log_write(text, len);
}

static char* plugin_VarArgs(char* format, ...) {
	va_list	argptr;
	static char str[8][1024];
	static int index = 0;
	int i = index;

	va_start(argptr, format);
	vsnprintf(str[i], sizeof(str[i]), format, argptr);
	va_end(argptr);

	index = (index + 1) & 7;
	return str[i];
}

static int plugin_IsVM() {
	return g_ModMgr->Mod()->IsVM();
}

static const char* plugin_EngMsgName(int x) {
	return ENG_MSGNAME(x);
}

static const char* plugin_ModMsgName(int x) {
	return MOD_MSGNAME(x);
}

static int plugin_GetIntCvar(const char* cvar) {
	return get_int_cvar(cvar);
}

static const char* plugin_GetStrCvar(const char* cvar) {
	return get_str_cvar(cvar);
}

static pluginfuncs_t s_pluginfuncs = {
	&plugin_WriteGameLog,
	&plugin_VarArgs,
	&plugin_IsVM,
	&plugin_EngMsgName,
	&plugin_ModMsgName,
	&plugin_GetIntCvar,
	&plugin_GetStrCvar,
};

pluginfuncs_t* get_pluginfuncs() {
	return &s_pluginfuncs;
}
