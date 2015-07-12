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

/* $Id: main.cpp,v 1.7 2006/03/08 08:25:01 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_local.h>
#include <qmmapi.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "vote.h"
#include "util.h"

pluginres_t* g_result = NULL;		//QMM result pointer
plugininfo_t g_plugininfo = {
	"QAdmin",			//name of plugin
	QADMIN_QMM_VERSION,		//version of plugin
	"Administration Plugin",	//description of plugin
	"CyberMind",			//author of plugin
	"http://www.quake3mm.net/",	//website of plugin
	0,				//can this plugin be paused?
	0,				//can this plugin be loaded via cmd
	1,				//can this plugin be unloaded via cmd
	QMM_PIFV_MAJOR,
	QMM_PIFV_MINOR
};
eng_syscall_t g_syscall = NULL;		//engine's syscall pointer
mod_vmMain_t g_vmMain = NULL;		//mod's vmMain pointer
int g_vmbase = 0;			//VM base address, used with GETPTR() macro
pluginfuncs_t* g_pluginfuncs = NULL;

playerinfo_t g_playerinfo[MAX_CLIENTS];	//store qadmin-specific user info

userinfo_t g_userinfo[MAX_USER_ENTRIES];	//store user/pass data
int g_maxuserinfo = -1;

int g_defaultAccess = 1;

time_t g_mapstart;
int g_levelTime;

char** g_gaggedCmds = NULL;

//first function called in plugin, give QMM the plugin info
C_DLLEXPORT void QMM_Query(plugininfo_t** pinfo) {
	QMM_GIVE_PINFO();
}

//second function called, save pointers
C_DLLEXPORT int QMM_Attach(eng_syscall_t engfunc, mod_vmMain_t modfunc, pluginres_t* presult, pluginfuncs_t* pluginfuncs, int vmbase, int iscmd) {
	QMM_SAVE_VARS();
	iscmd = 0;			//ignore, but satisfy gcc -Wall

	g_syscall(G_PRINT, QMM_VARARGS("[QADMIN] %s v%s by %s (%s)\n", g_plugininfo.name, g_plugininfo.version, g_plugininfo.author, g_plugininfo.url));

	//make version cvar
	g_syscall(G_CVAR_REGISTER, NULL, "admin_version", QADMIN_QMM_VERSION, CVAR_SERVERINFO | CVAR_ROM | CVAR_ARCHIVE);
	g_syscall(G_CVAR_SET, "admin_version", QADMIN_QMM_VERSION);

	//other cvars
	g_syscall(G_CVAR_REGISTER, NULL, "admin_default_access", "1", CVAR_ARCHIVE);
	g_syscall(G_CVAR_REGISTER, NULL, "admin_vote_kick_time", "60", CVAR_ARCHIVE);
	g_syscall(G_CVAR_REGISTER, NULL, "admin_vote_map_time", "60", CVAR_ARCHIVE);
	g_syscall(G_CVAR_REGISTER, NULL, "admin_config_file", "qmmaddons/qadmin/config/qadmin.cfg", CVAR_ARCHIVE);
	g_syscall(G_CVAR_REGISTER, NULL, "admin_gagged_cmds", "say_team,tell,vsay,vsay_team,vtell,vosay,vosay_team,votell,vtaunt", CVAR_ARCHIVE);

	memset(g_userinfo, 0, sizeof(g_userinfo));
	memset(g_playerinfo, 0, sizeof(g_playerinfo));

	return 1;
}

//last function called, clean up stuff allocated in QMM_Attach
C_DLLEXPORT void QMM_Detach(int iscmd) {
	tok_free(g_gaggedCmds);

	iscmd = 0;			//ignore, but satisfy gcc -Wall
}

//called before mod's vmMain (engine->mod)
C_DLLEXPORT int QMM_vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	//get client info on connection (moved to Post)
	//clear client info on disconnection
	if (cmd == GAME_CLIENT_DISCONNECT) {
		memset(&g_playerinfo[arg0], 0, sizeof(g_playerinfo[arg0]));
		
	//handle client commands
	} else if (cmd == GAME_CLIENT_COMMAND) {
		return handlecommand(arg0, 0);

	//allow admin commands from console with "admin_cmd" or "a_c" commands
	} else if (cmd == GAME_CONSOLE_COMMAND) {
		char command[MAX_COMMAND_LENGTH];
		g_syscall(G_ARGV, 0, command, sizeof(command));
		if (!strcasecmp(command, "admin_cmd") || !strcasecmp(command, "a_c"))
			return handlecommand(SERVER_CONSOLE, 1);
		else if (!strcasecmp(command, "admin_adduser_ip"))
			return admin_adduser(au_ip);
		else if (!strcasecmp(command, "admin_adduser_name"))
			return admin_adduser(au_name);
		else if (!strcasecmp(command, "admin_adduser_id"))
			return admin_adduser(au_id);

	//handle the game initialization (independent of mod being loaded)
	} else if (cmd == GAME_INIT) {
		g_levelTime = arg0;

	//handle the game shutdown
	} else if (cmd == GAME_SHUTDOWN) {
		//do shutdown stuff
	
	} else if (cmd == GAME_RUN_FRAME) {
		g_levelTime = arg0;

		if (g_vote.inuse && arg0 >= g_vote.finishtime)
			vote_finish();
	}

	QMM_RET_IGNORED(1);
}

//called after mod's vmMain (engine->mod)
C_DLLEXPORT int QMM_vmMain_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	//save client data on connection
	//(this is here so that the game has a chance to do various info checking before
	//we get the values)
	if (cmd == GAME_CLIENT_CONNECT || cmd == GAME_CLIENT_USERINFO_CHANGED) {
		char userinfo[MAX_INFO_STRING];
		g_syscall(G_GET_USERINFO, arg0, userinfo, sizeof(userinfo));

		if (!Info_Validate(userinfo))
			QMM_RET_IGNORED(1);

		if (cmd == GAME_CLIENT_CONNECT) {
			memset(&g_playerinfo[arg0], 0, sizeof(g_playerinfo[arg0]));
			g_playerinfo[arg0].connected = 1;
			g_playerinfo[arg0].access = g_defaultAccess;
		}

		strncpy(g_playerinfo[arg0].ip, Info_ValueForKey(userinfo, "ip"), sizeof(g_playerinfo[arg0].ip));
		//if a situation arises where the ip is exactly 15 bytes long, the 16th byte
		//in the buffer will be ':', so this will terminate the string anyway
		char* temp = strstr(g_playerinfo[arg0].ip, ":");
		if (temp) *temp = '\0';
		strncpy(g_playerinfo[arg0].guid, Info_ValueForKey(userinfo, "cl_guid"), sizeof(g_playerinfo[arg0].guid));
		strncpy(g_playerinfo[arg0].name, Info_ValueForKey(userinfo, "name"), sizeof(g_playerinfo[arg0].name));
		strncpy(g_playerinfo[arg0].stripname, StripCodes(g_playerinfo[arg0].name), sizeof(g_playerinfo[arg0].stripname));

	//handle the game initialization (dependent on mod being loaded)
	} else if (cmd == GAME_INIT) {
		g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("exec %s.cfg\n", QMM_GETSTRCVAR("mapname")));
		//g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, "exec banned_guids.cfg\n");
		
		time(&g_mapstart);

		reload();
	}

	QMM_RET_IGNORED(1);
}

//called before engine's syscall (mod->engine)
C_DLLEXPORT int QMM_syscall(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {

	QMM_RET_IGNORED(1);
}

//called after engine's syscall (mod->engine)
C_DLLEXPORT int QMM_syscall_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {
	QMM_RET_IGNORED(1);
}
