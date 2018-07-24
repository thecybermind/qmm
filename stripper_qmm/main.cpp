/*

Stripper - Dynamic Map Entity Modification
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
    Kevin Masterson a.k.a. CyberMind <cybermind@users.sourceforge.net>

*/

/* $Id: main.cpp,v 1.4 2006/02/13 20:34:24 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_local.h>
#include <qmmapi.h>
#include <stdio.h>
#include <string.h>
#include "ent.h"

pluginres_t* g_result = NULL;
plugininfo_t g_plugininfo = {
	"Stripper",				//name of plugin
	STRIPPER_QMM_VERSION,			//version of plugin
	"Add/delete entities in the map",	//description of plugin
	STRIPPER_QMM_BUILDER,			//author of plugin
	"http://www.q3mm.org/",			//website of plugin
	1,					//can this plugin be paused?
	0,					//can this plugin be loaded via cmd
	1,					//can this plugin be unloaded via cmd
	QMM_PIFV_MAJOR,				//plugin interface version major
	QMM_PIFV_MINOR				//plugin interface version minor
};
eng_syscall_t g_syscall = NULL;
mod_vmMain_t g_vmMain = NULL;
pluginfuncs_t* g_pluginfuncs = NULL;
int g_vmbase = 0;

C_DLLEXPORT void QMM_Query(plugininfo_t** pinfo) {
	QMM_GIVE_PINFO();
}

C_DLLEXPORT int QMM_Attach(eng_syscall_t engfunc, mod_vmMain_t modfunc, pluginres_t* presult, pluginfuncs_t* pluginfuncs, int vmbase, int iscmd) {
	QMM_SAVE_VARS();

	iscmd = 0;

	return 1;
}

C_DLLEXPORT void QMM_Detach(int iscmd) {
	g_mapents.empty();
	iscmd = 0;
}

C_DLLEXPORT int QMM_vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	if (cmd == GAME_INIT) {
		//init msg
		g_syscall(G_PRINT, "[STRIPPER] Stripper v" STRIPPER_QMM_VERSION " by " STRIPPER_QMM_BUILDER " is loaded\n");

		//register cvar
		g_syscall(G_CVAR_REGISTER, NULL, "stripper_version", STRIPPER_QMM_VERSION, CVAR_ROM | CVAR_SERVERINFO | CVAR_NORESTART);
		g_syscall(G_CVAR_SET, "stripper_version", STRIPPER_QMM_VERSION);

		//get all the entity tokens from the engine and save to lists
		get_entity_tokens();

		//load global config
		load_config("qmmaddons/stripper/global.ini");

		//load map-specific config
		load_config(QMM_VARARGS("qmmaddons/stripper/maps/%s.ini", QMM_GETSTRCVAR("mapname")));

	} else if (cmd == GAME_CONSOLE_COMMAND) {
		char buf[20];
		g_syscall(G_ARGV, 0, buf, sizeof(buf));
		if (!strcmp(buf, "stripper_dump")) {
			dump_ents();
			QMM_RET_SUPERCEDE(1);
		}
	} //cmd == GAME_CONSOLE_COMMAND

	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_syscall(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {

	//loop through the ent list and return a single token
	if (cmd == G_GET_ENTITY_TOKEN) {
		QMM_RET_SUPERCEDE(get_next_entity_token((char*)arg0, arg1));
	}

	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_vmMain_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_syscall_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {
	QMM_RET_IGNORED(1);
}
