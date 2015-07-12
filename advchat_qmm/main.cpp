/*

ADVCHAT_QMM - Advanced Chat Plugin
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

/* $Id: main.cpp,v 1.3 2006/01/29 23:08:01 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_local.h>
#include <qmmapi.h>
#include <string.h>

pluginres_t* g_result = NULL;
plugininfo_t g_plugininfo = {
	"AdvChat",				//name of plugin
	ADVCHAT_QMM_VERSION,			//version of plugin
	"Advanced Chat Features",		//description of plugin
	ADVCHAT_QMM_BUILDER,			//author of plugin
	"http://www.q3mm.org/",			//website of plugin
	0,					//can this plugin be paused?
	0,					//can this plugin be loaded via cmd
	1,					//can this plugin be unloaded via cmd
	QMM_PIFV_MAJOR,				//plugin interface version major
	QMM_PIFV_MINOR				//plugin interface version minor
};
eng_syscall_t g_syscall = NULL;
mod_vmMain_t g_vmMain = NULL;
pluginfuncs_t* g_pluginfuncs = NULL;
int g_vmbase = 0;

gentity_t* g_gents = NULL;
int g_gentsize = sizeof(gentity_t);
gclient_t* g_clients = NULL;
int g_clientsize = sizeof(gclient_t);

int g_sayflag = 0;
int g_sayclient = 0;

C_DLLEXPORT void QMM_Query(plugininfo_t** pinfo) {
	QMM_GIVE_PINFO();
}

C_DLLEXPORT int QMM_Attach(eng_syscall_t engfunc, mod_vmMain_t modfunc, pluginres_t* presult, pluginfuncs_t* pluginfuncs, int vmbase, int iscmd) {
	QMM_SAVE_VARS();

	iscmd = 0;

	return 1;
}

C_DLLEXPORT void QMM_Detach(int iscmd) {
	iscmd = 0;
}

C_DLLEXPORT int QMM_vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {

	//cancel arg checking if a new command is entered before this plugin thinks it
	//has all the "say" args (sanity checking)
	if ((cmd == GAME_CONSOLE_COMMAND || cmd == GAME_CLIENT_COMMAND) && g_sayflag) {
		g_sayflag = 0;
		QMM_RET_IGNORED(1);
	}

	//hook client console commands
	if (cmd == GAME_CLIENT_COMMAND) {
		//check what command it is
		char buf[16];
		g_syscall(G_ARGV, 0, buf, sizeof(buf));

		//person is using a say command, set global flags
		if (!strncmp(buf, "say", 3)) {
			g_sayflag = g_syscall(G_ARGC);
			g_sayclient = arg0;
		}
	}

	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_syscall(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {
	//store player/entity information
	if (cmd == G_LOCATE_GAME_DATA) {
		g_gents = (gentity_t*)arg0;
		g_gentsize = arg2;
		g_clients = (gclient_t*)arg3;
		g_clientsize = arg4;
	}

	//this is the main workhorse
	//whenever the mod calls trap_Argv for a say command, this checks each argument for the
	//existence of a replacable token (like "$h") and then replaces all instances of them
	else if (cmd == G_ARGV) {
		//if this is an argument for a say command
		if (g_sayflag) {
			//run the function normally, using the mod-given buffer/size
			int argnum = arg0;
			char* buf = (char*)arg1;
			int buflen = arg2;
			g_syscall(G_ARGV, argnum, buf, buflen);

			int health = ENT_FROM_NUM(g_sayclient)->health;
			//check for "$h" tokens
			char* replace = strstr(buf, "$h");
			//this argument contains at least one "$h" token
			while (replace) {
				//stop 'buf' from being read past the token
				*replace = '\0';

				//form new string with the token replaced
				strncpy(buf, QMM_VARARGS("%s%d%s", buf, health, replace + 2), buflen);
				buf[buflen - 1] = '\0';

				//look for another token
				replace = strstr(buf, "$h");
			}

//RTCW: Enemy Territory doesn't have armor
#ifndef GAME_RTCWET
			int armor = CLIENT_FROM_NUM(g_sayclient)->ps.stats[STAT_ARMOR];
			replace = strstr(buf, "$a");
			while (replace) {
				*replace = '\0';
				strncpy(buf, QMM_VARARGS("%s%d%s", buf, armor, replace + 2), buflen);
				buf[buflen - 1] = '\0';
				replace = strstr(buf, "$a");
			}
#endif

			//stop checking future args if this is the last arg
			if (argnum == g_sayflag - 1)
				g_sayflag = 0;

			//NEVER let this go through for say command args
			QMM_RET_SUPERCEDE(1);
		}
	}

	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_vmMain_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_syscall_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {
	QMM_RET_IGNORED(1);
}
