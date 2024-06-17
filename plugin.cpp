/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

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
