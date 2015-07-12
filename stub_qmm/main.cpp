/*

STUB_QMM - Example QMM Plugin
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

/* $Id: main.cpp,v 1.13 2005/04/04 03:18:38 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_local.h>
#include <qmmapi.h>

pluginres_t* g_result = NULL;
plugininfo_t g_plugininfo = {
	"STUB_QMM",				//name of plugin
	STUB_QMM_VERSION,			//version of plugin
	"Stub/test plugin",			//description of plugin
	STUB_QMM_BUILDER,			//author of plugin
	"http://www.quake3mm.net/",		//website of plugin
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
int g_gentsize = sizeof(gentity_t), g_maxgents = ENTITYNUM_MAX_NORMAL;
gclient_t* g_clients = NULL;
int g_clientsize = sizeof(gclient_t);

int g_time = 0;
gentity_t* g_timerent = NULL;

typedef void (*pfndie)(gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod);

pfndie g_olddie = NULL, g_vmdie = NULL;
void timer_think(gentity_t* self);
void player_vm_die(void);
void player_die(gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod);

//this is the first function called by QMM
//do not do anything here that requires shutdown routines
// - pinfo is a pointer of this plugin's plugininfo struct to pass back to QMM 
C_DLLEXPORT void QMM_Query(plugininfo_t** pinfo) {
	//give QMM our plugin info struct
	*pinfo = &g_plugininfo;	
}

//this is the second function called by QMM
//do game-independant startup routines here
// - engfunc is a pointer to the engine's syscall function
// - modfunc is a pointer to the mod's vmMain function
// - presult is a pointer to this plugin's result variable
// - iscmd is a boolean; 0 = was loaded by plugins.ini, 1 = was loaded by "qmm load"
// return 0 = failure
C_DLLEXPORT int QMM_Attach(eng_syscall_t engfunc, mod_vmMain_t modfunc, pluginres_t* presult, pluginfuncs_t* pluginfuncs, int vmbase, int iscmd) {
	g_syscall = engfunc;
	g_vmMain = modfunc;
	g_result = presult;
	g_vmbase = vmbase;
	g_pluginfuncs = pluginfuncs;

	//ignore 'iscmd' but satisfy gcc -Wall
	iscmd = 0;

	return 1;
}

//this is the last function called by QMM
//do game-independant shutdown routines here
// - iscmd is a boolean; 0 = was unloaded by server (or "qmm force_unload"), 1 = was unloaded by "qmm unload"
C_DLLEXPORT void QMM_Detach(int iscmd) {
	//ignore 'iscmd' but satisfy gcc -Wall
	iscmd = 0;
}

//this is called BEFORE the mod's vmMain function is called (by engine)
//do game-dependant shutdown routines here, in a cmd==GAME_SHUTDOWN check
C_DLLEXPORT int QMM_vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	
	//track the game time
	if (cmd == GAME_INIT || cmd == GAME_RUN_FRAME) {
		g_time = arg0;
		
		//if the timer's think is due, call it
		if (cmd == GAME_RUN_FRAME && g_timerent && g_timerent->think && g_timerent->nextthink) {
			if (g_time >= g_timerent->nextthink)
				g_timerent->think(g_timerent);
		}
	
	//check for setgod server command and toggle godmode
	} else if (cmd == GAME_CONSOLE_COMMAND && g_gents) {
		char buf[16];
		g_syscall(G_ARGV, 0, buf, sizeof(buf));
		if (!strcmp(buf, "setgod")) {
			g_syscall(G_ARGV, 1, buf, sizeof(buf));
			int id = atoi(buf);
			gentity_t* p = ENT_FROM_NUM(id);
			if (!p->inuse) {
				g_syscall(G_PRINT, QMM_VARARGS("[STUB_QMM] Userid %d not found\n", id));
			} else {
				//p->takedamage = (qboolean)!set;
				p->flags ^= FL_GODMODE;
				g_syscall(G_PRINT, QMM_VARARGS("[STUB_QMM] User \"%s\" (%d) godmode %s\n", GETPTR(p->client,gclient_t*)->pers.netname, id, p->flags & FL_GODMODE ? "enabled" : "disabled"));
			}
			QMM_RET_SUPERCEDE(1);
		} else if (!strcmp(buf, "timer") && g_timerent && g_timerent->think) {
			g_syscall(G_ARGV, 1, buf, sizeof(buf));
			g_timerent->nextthink = g_time + atoi(buf) * 1000;
			QMM_RET_SUPERCEDE(1);
		}
	
	//check for myinfo client command and display his userinfo to him
	} else if (cmd == GAME_CLIENT_COMMAND) {
		char buf[16];
		g_syscall(G_ARGV, 0, buf, sizeof(buf));
		if (!strcmp(buf, "myinfo")) {
			char userinfo[MAX_INFO_STRING];
			g_syscall(G_GET_USERINFO, arg0, userinfo, sizeof(userinfo));
			g_syscall(G_SEND_SERVER_COMMAND, arg0, QMM_VARARGS("print \"[STUB_QMM] Your infostring is: '%s'\"", userinfo));
			QMM_RET_SUPERCEDE(1);
		}
	
	//if a player is about to think, modify his die pointer
	} else if (cmd == GAME_CLIENT_THINK) {
		gentity_t* ent = ENT_FROM_NUM(arg0);
		if (ent->die && (!g_olddie || !g_vmdie)) {
			g_olddie = ent->die;
			g_vmdie = (pfndie)QMM_ADDCALLBACK((void*)&player_vm_die, (void*)g_olddie);
		}

		if (QMM_ISVM())
			ent->die = g_vmdie;
		else
			ent->die = player_die;
	
	//reset all of the entity's die functions to normal
	} else if (cmd == GAME_SHUTDOWN) {
		gentity_t* p = NULL;
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			p = ENT_FROM_NUM(i);
			p->die = g_olddie;
		}
	}

	//g_syscall(G_PRINT, QMM_VARARGS("[STUB_QMM] QMM_vmMain(%s): Hi!\n", QMM_MODMSGNAME(cmd)));

	QMM_RET_IGNORED(1);
}

//this is called BEFORE the engine's syscall function is called (by mod)
C_DLLEXPORT int QMM_syscall(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {
	static int firstlocategamedata = 1;
	
	if (cmd == G_LOCATE_GAME_DATA) {
		g_gents = (gentity_t*)arg0;
		//g_maxgents = ENTITYNUM_MAX_NORMAL;
		g_gentsize = arg2;
		g_clients = (gclient_t*)arg3;
		g_clientsize = arg4;

		//if this is the first time we get a G_LOCATE_GAME_DATA message,
		//insert the entity into the list
		if (firstlocategamedata) {
			firstlocategamedata = 0;
			g_timerent = ENT_FROM_NUM(ENTITYNUM_MAX_NORMAL - 1);

			g_timerent->inuse = qtrue;
			g_timerent->s.number = ENTITYNUM_MAX_NORMAL - 1;
			g_timerent->neverFree = qtrue;
			g_timerent->nextthink = 0;
			
			//should the mod need to print the classname, it needs to be
			//modified into a VM pointer
			g_timerent->classname = SETPTR("timerent", char*);

			//since the mod never calls the think, we can use a regular pointer
			//however, if the entity is an item or weapon that the mod may call,
			//then you would need to use QMM_ADDCALLBACK when neccesary
			g_timerent->think = timer_think;
			g_timerent->s.eType = ET_INVISIBLE;
			g_timerent->r.svFlags = SVF_NOCLIENT;
			g_timerent->s.weapon = WP_NONE;
			g_timerent->s.eFlags = EF_NODRAW;
			g_timerent->r.ownerNum = ENTITYNUM_NONE;
			g_timerent->parent = NULL;
			g_timerent->clipmask = 0;
			g_timerent->client = NULL;

			g_timerent->s.pos.trType = TR_STATIONARY;
			g_timerent->s.pos.trTime = 0;
			
			vec3_t start = {0, 0, 0};
			
			VectorCopy(start, g_timerent->s.origin);
			VectorCopy(start, g_timerent->s.origin2);
			VectorCopy(start, g_timerent->r.currentOrigin);
		}

		g_syscall(cmd, arg0, g_maxgents, arg2, arg3, arg4);
		QMM_RET_SUPERCEDE(1);
	}

	//g_syscall(G_PRINT, QMM_VARARGS("[STUB_QMM] QMM_syscall(%s): Hi!\n", QMM_ENGMSGNAME(cmd)));

	QMM_RET_IGNORED(1);
}

//this is called AFTER the mod's vmMain function is called (by engine)
//do game-dependant startup routines here, in a cmd==GAME_INIT check
C_DLLEXPORT int QMM_vmMain_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	//g_syscall(G_PRINT, QMM_VARARGS("[STUB_QMM] QMM_vmMain_Post(%s): Hi!\n", QMM_MODMSGNAME(cmd)));

	//if the client just joined, set this die pointer to our hook (VM or real)
	if (cmd == GAME_CLIENT_BEGIN) {
		gentity_t* ent = ENT_FROM_NUM(arg0);
		if (ent->die && (!g_olddie || !g_vmdie)) {
			g_olddie = ent->die;
			g_vmdie = (pfndie)QMM_ADDCALLBACK((void*)&player_vm_die, (void*)g_olddie);
		}

		if (QMM_ISVM())
			ent->die = g_vmdie;
		else
			ent->die = player_die;
	}
	
	QMM_RET_IGNORED(1);
}

//this is called AFTER the engine's syscall function is called (by mod)
C_DLLEXPORT int QMM_syscall_Post(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {
	//g_syscall(G_PRINT, QMM_VARARGS("[STUB_QMM] QMM_syscall_Post(%s): Hi!\n", QMM_ENGMSGNAME(cmd)));

	QMM_RET_IGNORED(1);
}

//function that hooks ent->die for DLL mods
void player_die(gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod) {
	int sid = self->s.number;
	int aid = attacker->s.number;
	gclient_t* selfclient = CLIENT_FROM_NUM(sid);
	gclient_t* attackerclient = CLIENT_FROM_NUM(aid);

	//die can be called multiple times for the same death, check this to see if it's been
	//called already
	if (selfclient->ps.pm_type != PM_DEAD)
		g_syscall(G_PRINT, QMM_VARARGS("[STUB_QMM] Player %d killed player %d with weapon id %d\n", aid, sid, mod));

	if (!QMM_ISVM())
		g_olddie(self, inflictor, attacker, damage, mod);
}

//function that hooks ent->die for QVM mods
void player_vm_die(void) {
	player_die(GETPTR(QMM_ARGV(0), gentity_t*), (gentity_t*)QMM_ARGV(1), GETPTR(QMM_ARGV(2), gentity_t*), QMM_ARGV(3), QMM_ARGV(4));
}

//function called for our timerent's think
void timer_think(gentity_t* self) {
	self->nextthink = 0;
	char* entclass = GETPTR(self->classname, char*);
	g_syscall(G_PRINT, QMM_VARARGS("[STUB_QMM] timerent->think() (classname=\"%s\") called\n", entclass));
}
