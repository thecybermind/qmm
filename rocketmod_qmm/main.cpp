/*

RocketMod - Rocketlaunchers-Only Plugin
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

/* $Id: main.cpp,v 1.4 2006/01/30 00:41:42 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_local.h>
#include <qmmapi.h>
#include <stdio.h>
#include <string.h>

pluginres_t* g_result = NULL;
plugininfo_t g_plugininfo = {
	"RocketMod",				//name of plugin
	ROCKETMOD_QMM_VERSION,			//version of plugin
	"Rockets everywhere!",			//description of plugin
	ROCKETMOD_QMM_BUILDER,			//author of plugin
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

gclient_t* g_clients = NULL;
int g_clientsize = sizeof(gclient_t);

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

int s_enabled = 0;
C_DLLEXPORT int QMM_vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	if (cmd == GAME_INIT) {
		//init msg
		g_syscall(G_PRINT, "[ROCKET] RocketMod v" ROCKETMOD_QMM_VERSION " by " ROCKETMOD_QMM_BUILDER " is loaded\n");

		//register cvars
		g_syscall(G_CVAR_REGISTER, NULL, "rocketmod_version", ROCKETMOD_QMM_VERSION, CVAR_ROM | CVAR_SERVERINFO);
		g_syscall(G_CVAR_SET, "rocketmod_version", ROCKETMOD_QMM_VERSION);
		g_syscall(G_CVAR_REGISTER, NULL, "rocketmod_enabled", "1", CVAR_SERVERINFO | CVAR_ARCHIVE);
		g_syscall(G_CVAR_REGISTER, NULL, "rocketmod_gauntlet", "1", CVAR_ARCHIVE);
		g_syscall(G_CVAR_REGISTER, NULL, "rocketmod_ammo", "10", CVAR_ARCHIVE);

		//cache this in an int so we don't have to check it every time
		//G_GET_ENTITY_TOKEN comes around. player spawning still checks the cvar
		s_enabled = QMM_GETINTCVAR("rocketmod_enabled");
	}
	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_syscall(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {
	
	//store player/entity information
	if (cmd == G_LOCATE_GAME_DATA) {
		g_clients = (gclient_t*)arg3;
		g_clientsize = arg4;
	}

	//this is a good place to hook when a player respawns.
	//weird, i know, but if you look through ClientSpawn you'll
	//see this is called after the starting machine gun is set
	//also, don't do this if rocketmod_enabled is 0
	else if ((cmd == G_GET_USERCMD) && QMM_GETINTCVAR("rocketmod_enabled")) {
		gclient_t* client = CLIENT_FROM_NUM(arg0);

		//if the user just respawned, and he has a machine gun, we need to
		//remove it and give him a rocket launcher
		if ( ((client->ps.pm_flags & PMF_RESPAWNED) == PMF_RESPAWNED) && ((client->ps.stats[STAT_WEAPONS] & (1 << WP_MACHINEGUN)) == (1 << WP_MACHINEGUN)) ) {

			//give user rocket launcher and gauntlet only
			client->ps.stats[STAT_WEAPONS] = 1 << WP_ROCKET_LAUNCHER;

			if (QMM_GETINTCVAR("rocketmod_gauntlet"))
				client->ps.stats[STAT_WEAPONS] |= 1 << WP_GAUNTLET;

			//give rockets and clear machinegun ammo
			client->ps.ammo[WP_ROCKET_LAUNCHER] = QMM_GETINTCVAR("rocketmod_ammo");
			if (client->ps.ammo[WP_ROCKET_LAUNCHER] <= 0)
				client->ps.ammo[WP_ROCKET_LAUNCHER] = 10;
			client->ps.ammo[WP_MACHINEGUN] = 0;
			client->ps.ammo[WP_GAUNTLET] = -1;
			client->ps.ammo[WP_GRAPPLING_HOOK] = -1;

			//set rocket launcher as the default weapon and set to ready
			client->ps.weapon = WP_ROCKET_LAUNCHER;
			client->ps.weaponstate = WEAPON_READY;
		}
	}

	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_vmMain_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	QMM_RET_IGNORED(1);
}

C_DLLEXPORT int QMM_syscall_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {
	static int is_classname = 0;
	
	//this is called to get level-placed entity info at the start of the map
	//moved to syscall_Post to not interfere with stripper_qmm
	//also, don't do this if rocketmod_enabled is 0
	if ((cmd == G_GET_ENTITY_TOKEN) && s_enabled) {
		//weapon_* -> weapon_rocketlauncher
		//ammo_* -> ammo_rockets

		char* buf = (char*)arg0;

		//if this is the value string for a "classname" key, check it
		if (is_classname) {
			is_classname = 0;

			//if its a weapon entity, make it a rocket launcher
			if (!strncmp(buf, "weapon_", 7))
				strncpy(buf, "weapon_rocketlauncher", arg1);
			//if its an ammo entity, make it a rocket ammo pack
			else if (!strncmp(buf, "ammo_", 5))
				strncpy(buf, "ammo_rockets", arg1);

		} else if (!strcmp(buf, "classname")) {
			is_classname = 1;
		}
	}
	QMM_RET_IGNORED(1);
}
