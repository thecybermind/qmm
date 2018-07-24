/*

QMMVM - VM Wrapper Tool
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
    VM code from Q3Fusion (http://www.sourceforge.net/projects/q3fusion/)
    VM info from Phaethon (http://www.icculus.org/~phaethon/q3mc/q3vm_specs.html)
    Code3Arena (http://www.planetquake.com/code3arena/)

*/

/* $Id: main.cpp,v 1.15 2004/08/30 00:45:36 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>	//always
#include <g_public.h>	//always
#include <malloc.h>	//for alloc functions
#include "vm.h"
#include "main.h"
#include "util.h"

syscall_t g_syscall = NULL;
vm_t g_VM;

FILE* g_df = NULL;

char qmmvm_vmpath[MAX_QPATH], qmmvm_vmbase[16];
int qmmvm_debug = 0, qmmvm_stacksize = 0;

C_DLLEXPORT void dllEntry(syscall_t psyscall) {
	g_syscall = psyscall;

	//don't call engine's syscall from here, it will crash (trust me)
	//in fact, it's best not to do anything else here other than save the pointer
}

//route any incoming vmMain() calls to the VM (and hook some for various operations)
C_DLLEXPORT int vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	//handle some of our special VM interaction messages here
	
	//-1 = SETHOOK
	if (cmd == -1) {
		g_VM.hook_realfunc = arg0;
		if (qmmvm_debug)
			fprintf(g_df, "vmMain(-1) call\n");
		return 0;

	//do startup operations
	} else if (cmd == GAME_INIT) {

		g_syscall(G_PRINT, "[QMMVM] QMMVM v" QMMVM_VERSION " (" QMMVM_GAME "/" QMMVM_OS ") loaded\n");
		g_syscall(G_PRINT, "[QMMVM] Built " QMMVM_COMPILE " by " QMMVM_BUILDER "\n");
		g_syscall(G_PRINT, "[QMMVM] URL: http://www.quake3mm.net/\n");

		//make version cvar
		g_syscall(G_CVAR_REGISTER, NULL, "qmmvm_version", QMMVM_VERSION, CVAR_ROM | CVAR_NORESTART);

		//check cvars
		g_syscall(G_CVAR_VARIABLE_STRING_BUFFER, "qmmvm_vmpath", qmmvm_vmpath, sizeof(qmmvm_vmpath));
		qmmvm_stacksize = g_syscall(G_CVAR_VARIABLE_INTEGER_VALUE, "qmmvm_stacksize");
		qmmvm_debug = g_syscall(G_CVAR_VARIABLE_INTEGER_VALUE, "qmmvm_debug");

		if (qmmvm_debug) {
			char fs_game[MAX_QPATH];
			//get gamedir
			g_syscall(G_CVAR_VARIABLE_STRING_BUFFER, "fs_game", fs_game, sizeof(fs_game));
			//if fs_game cvar isn't set, use default
			if (!strlen(fs_game))
				strncpy(fs_game, QMMVM_DEFAULT_MODDIR, sizeof(fs_game));

			g_df = fopen(vaf("%s/qmmvmlog.txt", fs_game), "w");
			g_syscall(G_PRINT, "[QMMVM] Cvar \"qmmvm_debug\" enabled, opening logfile\n");
			fprintf(g_df, "Opening logfile\n");
			fprintf(g_df, "QMMVM v" QMMVM_VERSION " (" QMMVM_OS ") loaded\n");
			fprintf(g_df, "Built " QMMVM_COMPILE " by " QMMVM_BUILDER "\n");
			fprintf(g_df, "URL: http://www.quake3mm.net/\n");
		}

		//if vmpath isn't set, use default
		if (strlen(qmmvm_vmpath))
			g_syscall(G_PRINT, vaf("[QMMVM] Cvar found: \"qmmvm_vmpath\", VM path set to \"%s\"\n", qmmvm_vmpath));
		else {
			strncpy(qmmvm_vmpath, QMMVM_DEFAULT_VMPATH, sizeof(qmmvm_vmpath));
			g_syscall(G_PRINT, vaf("[QMMVM] WARNING: Cvar not found: \"qmmvm_vmpath\", VM path set to \"%s\"\n", qmmvm_vmpath));
		}

		//if stacksize isn't set, use default
		if (qmmvm_stacksize > 0) {
			g_syscall(G_PRINT, vaf("[QMMVM] Cvar found: \"qmmvm_stacksize\", VM stack size set to %iMB\n", qmmvm_stacksize));
		} else if (qmmvm_stacksize < 0) {
			g_syscall(G_PRINT, vaf("[QMMVM] WARNING: Invalid value (%i) for cvar \"qmmvm_stacksize\", VM stack size set to %iMB\n", -qmmvm_stacksize, qmmvm_stacksize));
			qmmvm_stacksize = -qmmvm_stacksize;
		} else {
			qmmvm_stacksize = 1;
			g_syscall(G_PRINT, vaf("[QMMVM] WARNING: Cvar not found: \"qmmvm_stacksize\", VM stack size set to %iMB\n", qmmvm_stacksize));
		}
		qmmvm_stacksize *= (1<<20);	//convert to MB

		g_syscall(G_PRINT, vaf("[QMMVM] Loading VM \"%s\"\n", qmmvm_vmpath));

		//clear VM
		memset(&g_VM, 0, sizeof(vm_t));
		//if we can't load the VM, we pretty much have to exit
		//or we can fake it, and sell it to the Fox Network
		if (!VM_Create(&g_VM, qmmvm_vmpath)) {
			g_syscall(G_ERROR, vaf("[QMMVM] FATAL ERROR: Unable to load VM \"%s\"\n", qmmvm_vmpath));
			return 0;
		}

		//make vmbase cvar
		strncpy(qmmvm_vmbase, vaf("%u", g_VM.dataSegment), sizeof(qmmvm_vmbase));
		g_syscall(G_CVAR_REGISTER, NULL, "qmmvm_vmbase", qmmvm_vmbase, CVAR_ROM | CVAR_NORESTART);

		g_syscall(G_PRINT, vaf("[QMMVM] Successfully loaded VM \"%s\"\n", qmmvm_vmpath));
		g_syscall(G_PRINT, vaf("[QMMVM] VM file size: %i\n", g_VM.fileSize));
		g_syscall(G_PRINT, vaf("[QMMVM] VM memory size: %i\n", g_VM.memorySize));
		g_syscall(G_PRINT, vaf("[QMMVM] VM stack size: %i\n", qmmvm_stacksize));
		g_syscall(G_PRINT, vaf("[QMMVM] VM data offset: %i\n", qmmvm_vmbase));

		//we're done (whew)
		g_syscall(G_PRINT, "[QMMVM] Startup successful, proceeding to VM startup.\n");

	//hook qmmvm_status command
	} else if (cmd == GAME_CONSOLE_COMMAND) {

		char buf[16];
		g_syscall(G_ARGV, 0, buf, sizeof(buf));
		if (!strcmp("qmmvm_status",buf)) {
			g_syscall(G_PRINT, "[QMMVM] QMMVM v" QMMVM_VERSION " (" QMMVM_GAME "/" QMMVM_OS ") loaded\n");
			g_syscall(G_PRINT, "[QMMVM] Built " QMMVM_COMPILE " by " QMMVM_BUILDER "\n");
			g_syscall(G_PRINT, "[QMMVM] URL: http://www.quake3mm.net/\n");
			g_syscall(G_PRINT, vaf("[QMMVM] Loaded VM: %s\n", qmmvm_vmpath));
			g_syscall(G_PRINT, vaf("[QMMVM] Debuglog: %s\n", (qmmvm_debug ? "on" : "off")));
			g_syscall(G_PRINT, vaf("[QMMVM] Byteswapped: %s\n", (g_VM.swapped ? "yes" : "no")));
			g_syscall(G_PRINT, vaf("[QMMVM] VM file size: %i\n", g_VM.fileSize));
			g_syscall(G_PRINT, vaf("[QMMVM] VM memory size: %i\n", g_VM.memorySize));
			g_syscall(G_PRINT, vaf("[QMMVM] VM stack size: %i\n", qmmvm_stacksize));
			g_syscall(G_PRINT, vaf("[QMMVM] VM data offset: %i\n", qmmvm_vmbase));
			return 1;	//do not pass to VM
		}

	}

	//call vmMain() in the VM
	int ret = 0;
	//if the VM wasn't loaded, don't route anything to it
	//(for GAME_SHUTDOWN, which is called due to our G_ERROR after VM failed to load)
	if (g_VM.memory) {
		//now route call to VM (save return value)
		if (qmmvm_debug) fprintf(g_df, "vmMain(%s) call\n", modfuncname(cmd));
		ret = VM_Exec(&g_VM, cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
		if (qmmvm_debug) fprintf(g_df, "vmMain(%s) = %i\n", modfuncname(cmd), ret);
	} else {
		if (qmmvm_debug) fprintf(g_df, "vmMain(%s) call bypassed\n", modfuncname(cmd));
	}

	//if this is the shutdown, we need to close down some stuff
	if (cmd == GAME_SHUTDOWN) {
		//unload VM (forgot to do this in early versions...oops)
		g_syscall(G_PRINT, "[QMMVM] Shutting down VM\n");
		VM_Destroy(&g_VM);

		if (qmmvm_debug) {
			fprintf(g_df, "Closing logfile\n");
			g_syscall(G_PRINT, "[QMMVM] Closing logfile\n");
			fclose(g_df);
		}
		g_syscall(G_PRINT, "[QMMVM] Finished shutting down, prepared for unload.\n");
	}

	//return the value the VM returned
	return ret;
}



//handling syscalls from QVM (passing them to engine)

//this adds the base VM address to a given value
#define add(x)		((x) ? (void*)((x) + (int)memoryBase) : NULL)
//this subtracts the base VM address from a given value
#define sub(x)		((x) ? (void*)((x) - (int)memoryBase) : NULL)
//this gets an argument value
#define arg(x)		(args[(x)])
//this adds the base VM address to an argument value
#define ptr(x)		(add(arg(x)))

int QDECL VM_SysCalls(byte *memoryBase, int cmd, int *args) {
	if (qmmvm_debug) {
		fprintf(g_df, "VM_SysCalls(%s)\n", engfuncname(cmd));
	}

	switch(cmd) {
#if defined(GAME_Q3A)
	case G_PRINT:				// ( const char *string );
		return g_syscall(cmd, ptr(0));
	case G_ERROR:				// ( const char *string );
		return g_syscall(cmd, ptr(0));
	case G_MILLISECONDS:			// ( void );
		return g_syscall(cmd);
	case G_CVAR_REGISTER:			// ( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags );
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), arg(3));
	case G_CVAR_UPDATE:			// ( vmCvar_t *vmCvar );
		return g_syscall(cmd, ptr(0));
	case G_CVAR_SET:			// ( const char *var_name, const char *value );
		return g_syscall(cmd, ptr(0), ptr(1));
	case G_CVAR_VARIABLE_INTEGER_VALUE:	// ( const char *var_name );
		return g_syscall(cmd, ptr(0));
	case G_CVAR_VARIABLE_STRING_BUFFER:	// ( const char *var_name, char *buffer, int bufsize );
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case G_ARGC:				// ( void );
		return g_syscall(cmd);
	case G_ARGV:				// ( int n, char *buffer, int bufferLength );
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case G_FS_FOPEN_FILE:			// ( const char *qpath, fileHandle_t *file, fsMode_t mode );
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case G_FS_READ:				// ( void *buffer, int len, fileHandle_t f );
		return g_syscall(cmd, ptr(0), arg(1), arg(2));
	case G_FS_WRITE:			// ( const void *buffer, int len, fileHandle_t f );
		return g_syscall(cmd, ptr(0), arg(1), arg(2));
	case G_FS_FCLOSE_FILE:			// ( fileHandle_t f );
		return g_syscall(cmd, arg(0));
	case G_SEND_CONSOLE_COMMAND:		// ( int exec_when, const char *text )
		return g_syscall(cmd, arg(0), ptr(1));
	case G_LOCATE_GAME_DATA:		// ( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *clients, int sizeofGameClient );
		return g_syscall(cmd, ptr(0), arg(1), arg(2), ptr(3), arg(4));
	case G_DROP_CLIENT:			// ( int clientNum, const char *reason );
		return g_syscall(cmd, arg(0), ptr(1));
	case G_SEND_SERVER_COMMAND:		// ( int clientNum, const char *fmt );
		return g_syscall(cmd, arg(0), ptr(1));
	case G_SET_CONFIGSTRING:		// ( int num, const char *string );
		return g_syscall(cmd, arg(0), ptr(1));
	case G_GET_CONFIGSTRING:		// ( int num, char *buffer, int bufferSize );
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case G_GET_USERINFO:			// ( int num, char *buffer, int bufferSize );
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case G_SET_USERINFO:			// ( int num, const char *buffer );
		return g_syscall(cmd, arg(0), ptr(1));
	case G_GET_SERVERINFO:			// ( char *buffer, int bufferSize );
		return g_syscall(cmd, ptr(0), arg(1));
	case G_SET_BRUSH_MODEL:			// ( gentity_t *ent, const char *name );
		return g_syscall(cmd, ptr(0), ptr(1));
	case G_TRACE:				// ( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), ptr(3), ptr(4), arg(5), arg(6));
	case G_POINT_CONTENTS:			// ( const vec3_t point, int passEntityNum );
		return g_syscall(cmd, ptr(0), arg(1));
	case G_IN_PVS:				// ( const vec3_t p1, const vec3_t p2 );
		return g_syscall(cmd, ptr(0), ptr(1));
	case G_IN_PVS_IGNORE_PORTALS:		// ( const vec3_t p1, const vec3_t p2 );
		return g_syscall(cmd, ptr(0), ptr(1));
	case G_ADJUST_AREA_PORTAL_STATE:	// ( gentity_t *ent, qboolean open );
		return g_syscall(cmd, ptr(0), arg(1));
	case G_AREAS_CONNECTED:			// ( int area1, int area2 );
		return g_syscall(cmd, arg(0), arg(1));
	case G_LINKENTITY:			// ( gentity_t *ent );
		return g_syscall(cmd, ptr(0));
	case G_UNLINKENTITY:			// ( gentity_t *ent );
		return g_syscall(cmd, ptr(0));
	case G_ENTITIES_IN_BOX:			// ( const vec3_t mins, const vec3_t maxs, gentity_t **list, int maxcount );
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), arg(3));
	case G_ENTITY_CONTACT:			// ( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2));
	case G_BOT_ALLOCATE_CLIENT:		// ( void );
		return g_syscall(cmd);
	case G_BOT_FREE_CLIENT:			// ( int clientNum );
		return g_syscall(cmd, arg(0));
	case G_GET_USERCMD:			// ( int clientNum, usercmd_t *cmd )
		return g_syscall(cmd, arg(0), ptr(1));
	case G_GET_ENTITY_TOKEN:		// qboolean ( char *buffer, int bufferSize )
		return g_syscall(cmd, ptr(0), arg(1));
	case G_FS_GETFILELIST:			//(  const char *path, const char *extension, char *listbuf, int bufsize ) {
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), arg(3));
	case G_DEBUG_POLYGON_CREATE:		//(int color, int numPoints, vec3_t *points)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case G_DEBUG_POLYGON_DELETE:		//(int id)
		return g_syscall(cmd, arg(0));
	case G_REAL_TIME:			//( qtime_t *qtime )
		return g_syscall(cmd, ptr(0));
	case G_SNAPVECTOR:			//(float *v)
		return g_syscall(cmd, ptr(0));
	case G_TRACECAPSULE:			// ( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
		return g_syscall(cmd, arg(0), arg(1), arg(2), arg(3), arg(4), arg(5));
	case G_ENTITY_CONTACTCAPSULE:		// ( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case G_FS_SEEK:				//( fileHandle_t f, long offset, int origin )
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case BOTLIB_SETUP:			//(void)
		return g_syscall(cmd);
	case BOTLIB_SHUTDOWN:			//(void)
		return g_syscall(cmd);
	case BOTLIB_LIBVAR_SET:			//(char *var_name, char *value)
		return g_syscall(cmd, ptr(0), ptr(1));
	case BOTLIB_LIBVAR_GET:			//(char *var_name, char *value, int size)
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case BOTLIB_PC_ADD_GLOBAL_DEFINE:	//(char* string)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_START_FRAME:		//(float time)
		return g_syscall(cmd, arg(0));
	case BOTLIB_LOAD_MAP:			//(const char *mapname)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_UPDATENTITY:		//(int ent, void /* struct bot_updateentity_s */ *bue)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_TEST:			//(int parm0, char *parm1, vec3_t parm2, vec3_t parm3)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), ptr(3));
	case BOTLIB_GET_SNAPSHOT_ENTITY:	//( int clientNum, int sequence )
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_GET_CONSOLE_MESSAGE:	//(int clientNum, char *message, int size)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_USER_COMMAND:		//(int clientNum, usercmd_t *ucmd)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AAS_ENTITY_INFO:		//(int entnum, void /* struct aas_entityinfo_s */ *info)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AAS_INITIALIZED:		//(void)
		return g_syscall(cmd);
	case BOTLIB_AAS_PRESENCE_TYPE_BOUNDING_BOX:	//(int presencetype, vec3_t mins, vec3_t maxs)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AAS_TIME:			//(void)
		return g_syscall(cmd);
	case BOTLIB_AAS_POINT_AREA_NUM:		//(vec3_t point)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AAS_POINT_REACHABILITY_AREA_INDEX:	//(vec3_t point)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AAS_TRACE_AREAS:		//(vec3_t start, vec3_t end, int *areas, vec3_t *points, int maxareas)
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), ptr(3), arg(4));
	case BOTLIB_AAS_BBOX_AREAS:		//(vec3_t absmins, vec3_t absmaxs, int *areas, int maxareas)
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), arg(3));
	case BOTLIB_AAS_AREA_INFO:		//( int areanum, void /* struct aas_areainfo_s */ *info )
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AAS_POINT_CONTENTS:		//(vec3_t point)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AAS_NEXT_BSP_ENTITY:	//(int ent)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AAS_VALUE_FOR_BSP_EPAIR_KEY:	//(int ent, char *key, char *value, int size)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), arg(3));
	case BOTLIB_AAS_VECTOR_FOR_BSP_EPAIR_KEY:	//(int ent, char *key, vec3_t v)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AAS_FLOAT_FOR_BSP_EPAIR_KEY:	//(int ent, char *key, float *value)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AAS_INT_FOR_BSP_EPAIR_KEY:	//(int ent, char *key, int *value)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AAS_AREA_REACHABILITY:	//(int areanum)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AAS_AREA_TRAVEL_TIME_TO_GOAL_AREA:	//(int areanum, vec3_t origin, int goalareanum, int travelflags)
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3));
	case BOTLIB_AAS_ENABLE_ROUTING_AREA:	//(int areanum, int enable)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AAS_PREDICT_ROUTE:		//(void /*struct aas_predictroute_s*/ *route, int areanum, vec3_t origin, int goalareanum, int travelflags, int maxareas, int maxtime, int stopevent, int stopcontents, int stoptfl, int stopareanum)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3), arg(4), arg(5), arg(6), arg(7), arg(8), arg(9), arg(10), arg(11));
	case BOTLIB_AAS_ALTERNATIVE_ROUTE_GOAL:	//(vec3_t start, int startareanum, vec3_t goal, int goalareanum, int travelflags, void /*struct aas_altroutegoal_s*/ *altroutegoals, int maxaltroutegoals, int type)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3), arg(4), ptr(5), arg(6), arg(7));
	case BOTLIB_AAS_SWIMMING:		//(vec3_t origin)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AAS_PREDICT_CLIENT_MOVEMENT:	//(void /* struct aas_clientmove_s */ *move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3), arg(4), ptr(5), ptr(6), arg(7), arg(8), arg(9), arg(10), arg(11), arg(12));
	case BOTLIB_EA_SAY:			//(int client, char *str)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_EA_SAY_TEAM:		//(int client, char *str)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_EA_COMMAND:			//(int client, char *command)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_EA_ACTION:			//(int client, int action)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_EA_GESTURE:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_TALK:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_ATTACK:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_USE:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_RESPAWN:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_CROUCH:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_UP:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_DOWN:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_FORWARD:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_BACK:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_LEFT:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_RIGHT:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_SELECT_WEAPON:		//(int client, int weapon)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_EA_JUMP:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_DELAYED_JUMP:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE:			//(int client, vec3_t dir, float speed)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_EA_VIEW:			//(int client, vec3_t viewangles)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_EA_END_REGULAR:		//(int client, float thinktime)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_EA_GET_INPUT:		//(int client, float thinktime, void /* struct bot_input_s */ *input)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case BOTLIB_EA_RESET_INPUT:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_LOAD_CHARACTER:		//(char *charfile, float skill)
		return g_syscall(cmd, ptr(0), arg(1));
	case BOTLIB_AI_FREE_CHARACTER:		//(int character)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_CHARACTERISTIC_FLOAT:	//(int character, int index)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_CHARACTERISTIC_BFLOAT:	//(int character, int index, float min, float max)
		return g_syscall(cmd, arg(0), arg(1), arg(2), arg(3));
	case BOTLIB_AI_CHARACTERISTIC_INTEGER:	//(int character, int index)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_CHARACTERISTIC_BINTEGER:	//(int character, int index, int min, int max)
		return g_syscall(cmd, arg(0), arg(1), arg(2), arg(3));
	case BOTLIB_AI_CHARACTERISTIC_STRING:	//(int character, int index, char *buf, int size)
		return g_syscall(cmd, arg(0), arg(1), ptr(2), arg(3));
	case BOTLIB_AI_ALLOC_CHAT_STATE:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_FREE_CHAT_STATE:		//(int handle)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_QUEUE_CONSOLE_MESSAGE:	//(int chatstate, int type, char *message)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case BOTLIB_AI_REMOVE_CONSOLE_MESSAGE:	//(int chatstate, int handle)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_NEXT_CONSOLE_MESSAGE:	//(int chatstate, void /* struct bot_consolemessage_s */ *cm)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_NUM_CONSOLE_MESSAGE:	//(int chatstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_INITIAL_CHAT:		//(int chatstate, char *type, int mcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 )
		return g_syscall(cmd, arg(0), ptr(1), arg(2), ptr(3), ptr(4), ptr(5), ptr(6), ptr(7), ptr(8), ptr(9), ptr(10));
	case BOTLIB_AI_NUM_INITIAL_CHATS:	//(int chatstate, char *type)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_REPLY_CHAT:		//(int chatstate, char *message, int mcontext, int vcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 )
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3), ptr(4), ptr(5), ptr(6), ptr(7), ptr(8), ptr(9), ptr(10), ptr(11));
	case BOTLIB_AI_CHAT_LENGTH:		//(int chatstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_ENTER_CHAT:		//(int chatstate, int client, int sendto)
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case BOTLIB_AI_GET_CHAT_MESSAGE:	//(int chatstate, char *buf, int size)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_AI_STRING_CONTAINS:		//(char *str1, char *str2, int casesensitive)
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case BOTLIB_AI_FIND_MATCH:		//(char *str, void /* struct bot_match_s */ *match, unsigned long int context)
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case BOTLIB_AI_MATCH_VARIABLE:		//(void /* struct bot_match_s */ *match, int variable, char *buf, int size)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3));
	case BOTLIB_AI_UNIFY_WHITE_SPACES:	//(char* string)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AI_REPLACE_SYNONYMS:	//(char *string, unsigned long int context)
		return g_syscall(cmd, ptr(0), arg(1));
	case BOTLIB_AI_LOAD_CHAT_FILE:		//(int chatstate, char* chatfile, char* chatname)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AI_SET_CHAT_GENDER:		//(int chatstate, int gender)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_SET_CHAT_NAME:		//(int chatstate, char *name, int client)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_AI_RESET_GOAL_STATE:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_RESET_AVOID_GOALS:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_REMOVE_FROM_AVOID_GOALS:	//(int goalstate, int number)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_PUSH_GOAL:		//(int goalstate, void* goal)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_POP_GOAL:		//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_EMPTY_GOAL_STACK:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_DUMP_AVOID_GOALS:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_DUMP_GOAL_STACK:		//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_GOAL_NAME:		//(int number, char *name, int size)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_AI_GET_TOP_GOAL:		//(int goalstate, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_GET_SECOND_GOAL:		//(int goalstate, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_CHOOSE_LTG_ITEM:		//(int goalstate, vec3_t origin, int *inventory, int travelflags)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), arg(3));
	case BOTLIB_AI_CHOOSE_NBG_ITEM:		//(int goalstate, vec3_t origin, int *inventory, int travelflags, void /* struct bot_goal_s */ *ltg, float maxtime)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), arg(3), ptr(4), arg(5));
	case BOTLIB_AI_TOUCHING_GOAL:		//(vec3_t origin, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, ptr(0), ptr(1));
	case BOTLIB_AI_ITEM_GOAL_IN_VIS_BUT_NOT_VISIBLE:	//(int viewer, vec3_t eye, vec3_t viewangles, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), ptr(3));
	case BOTLIB_AI_GET_LEVEL_ITEM_GOAL:	//(int index, char *classname, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AI_GET_NEXT_CAMP_SPOT_GOAL:	//(int num, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_GET_MAP_LOCATION_GOAL:	//(char *name, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, ptr(0), ptr(1));
	case BOTLIB_AI_AVOID_GOAL_TIME:		//(int goalstate, int number)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_SET_AVOID_GOAL_TIME:	//(int goalstate, int number, float avoidtime)
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case BOTLIB_AI_INIT_LEVEL_ITEMS:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_UPDATE_ENTITY_ITEMS:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_LOAD_ITEM_WEIGHTS:	//(int, char*)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_FREE_ITEM_WEIGHTS:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_INTERBREED_GOAL_FUZZY_LOGIC:	//(int,int,int)
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case BOTLIB_AI_SAVE_GOAL_FUZZY_LOGIC:	//(int, char*)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_MUTATE_GOAL_FUZZY_LOGIC: //(int goalstate, float range)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_ALLOC_GOAL_STATE:	//(int state)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_FREE_GOAL_STATE:		//(int handle)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_RESET_MOVE_STATE:	//(int movestate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_ADD_AVOID_SPOT:		//(int movestate, vec3_t origin, float radius, int type)
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3));
	case BOTLIB_AI_MOVE_TO_GOAL:		//(void /* struct bot_moveresult_s */ *result, int movestate, void /* struct bot_goal_s */ *goal, int travelflags)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3));
	case BOTLIB_AI_MOVE_IN_DIRECTION:	//(int movestate, vec3_t dir, float speed, int type)
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3));
	case BOTLIB_AI_RESET_AVOID_REACH:	//(int movestate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_RESET_LAST_AVOID_REACH:	//(int movestate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_REACHABILITY_AREA:	//(vec3_t origin, int testground)
		return g_syscall(cmd, ptr(0), arg(1));
	case BOTLIB_AI_MOVEMENT_VIEW_TARGET:	//(int movestate, void /* struct bot_goal_s */ *goal, int travelflags, float lookahead, vec3_t target)
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3), ptr(4));
	case BOTLIB_AI_PREDICT_VISIBLE_POSITION:	//(vec3_t origin, int areanum, void /* struct bot_goal_s */ *goal, int travelflags, vec3_t target)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3), ptr(4));
	case BOTLIB_AI_ALLOC_MOVE_STATE:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_FREE_MOVE_STATE:		//(int handle)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_INIT_MOVE_STATE:		//(int handle, void* initmove)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_CHOOSE_BEST_FIGHT_WEAPON:	//(int weaponstate, int *inventory)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_GET_WEAPON_INFO:		//(int weaponstate, int weapon, void /* struct weaponinfo_s */ *weaponinfo)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case BOTLIB_AI_LOAD_WEAPON_WEIGHTS:	//(int, char*)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_ALLOC_WEAPON_STATE:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_FREE_WEAPON_STATE:	//(int)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_RESET_WEAPON_STATE:	//(int)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_GENETIC_PARENTS_AND_CHILD_SELECTION:	//(int numranks, float *ranks, int *parent1, int *parent2, int *child)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), ptr(3), ptr(4));
	case BOTLIB_PC_LOAD_SOURCE:		//(const char*)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_PC_FREE_SOURCE:		//(int)
		return g_syscall(cmd, arg(0));
	case BOTLIB_PC_READ_TOKEN:		//(int, void*)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_PC_SOURCE_FILE_AND_LINE:	//( int handle, char *filename, int *line )
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case G_MEMSET:				//memset(void* dest, int c, size_t count)
		return g_syscall(cmd, ptr(0), arg(1), arg(2));
	case G_MEMCPY:				//memcpy( void *dest, const void *src, size_t count )
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case G_STRNCPY:				//strncpy( char *strDest, const char *strSource, size_t count )
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case G_SIN:				//sin(double)
		return g_syscall(cmd, arg(0));
	case G_COS:				//cos(double)
		return g_syscall(cmd, arg(0));
	case G_ATAN2:				//atan2(double, double)
		return g_syscall(cmd, arg(0), arg(1));
	case G_SQRT:				//sqrt(double)
		return g_syscall(cmd, arg(0));
	case G_FLOOR:				//floor(double)
		return g_syscall(cmd, arg(0));
	case G_CEIL:				//ceil(double)
		return g_syscall(cmd, arg(0));
	case G_TESTPRINTINT:			//(char*, int)
		return g_syscall(cmd, ptr(0), arg(1));
	case G_TESTPRINTFLOAT:			//(char*, float)
		return g_syscall(cmd, ptr(0), arg(1));
	/* QMMVM TRAP FUNCTIONS
		QMMVM has been modified to understand pointer values larger than
		the VM's dataSegmentLen to be real pointers, and it will properly
		dereference them.
	*/
	case QMMVM_G_MALLOC:			//void* malloc(size_t size)
		return (int)malloc((size_t)arg(0));
	case QMMVM_G_REALLOC:			//void* realloc(void* addr, size_t size)
		return (int)realloc(ptr(0), (size_t)arg(1));
	case QMMVM_G_FREE:			//free(void* addr)
		free(ptr(0));

#elif defined(GAME_JK2)

	case G_PRINT:				// ( const char *string );
		return g_syscall(cmd, ptr(0));
	case G_ERROR:				// ( const char *string );
		return g_syscall(cmd, ptr(0));
	case G_MILLISECONDS:			// ( void );
		return g_syscall(cmd);
	case G_CVAR_REGISTER:			// ( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags );
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), arg(3));
	case G_CVAR_UPDATE:			// ( vmCvar_t *vmCvar );
		return g_syscall(cmd, ptr(0));
	case G_CVAR_SET:			// ( const char *var_name, const char *value );
		return g_syscall(cmd, ptr(0), ptr(1));
	case G_CVAR_VARIABLE_INTEGER_VALUE:	// ( const char *var_name );
		return g_syscall(cmd, ptr(0));
	case G_CVAR_VARIABLE_STRING_BUFFER:	// ( const char *var_name, char *buffer, int bufsize );
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case G_ARGC:				// ( void );
		return g_syscall(cmd);
	case G_ARGV:				// ( int n, char *buffer, int bufferLength );
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case G_FS_FOPEN_FILE:			// ( const char *qpath, fileHandle_t *file, fsMode_t mode );
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case G_FS_READ:				// ( void *buffer, int len, fileHandle_t f );
		return g_syscall(cmd, ptr(0), arg(1), arg(2));
	case G_FS_WRITE:			// ( const void *buffer, int len, fileHandle_t f );
		return g_syscall(cmd, ptr(0), arg(1), arg(2));
	case G_FS_FCLOSE_FILE:			// ( fileHandle_t f );
		return g_syscall(cmd, arg(0));
	case G_SEND_CONSOLE_COMMAND:		// ( int exec_when, const char *text )
		return g_syscall(cmd, arg(0), ptr(1));
	case G_LOCATE_GAME_DATA:		// ( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *clients, int sizeofGameClient );
		return g_syscall(cmd, ptr(0), arg(1), arg(2), ptr(3), arg(4));
	case G_DROP_CLIENT:			// ( int clientNum, const char *reason );
		return g_syscall(cmd, arg(0), ptr(1));
	case G_SEND_SERVER_COMMAND:		// ( int clientNum, const char *fmt );
		return g_syscall(cmd, arg(0), ptr(1));
	case G_SET_CONFIGSTRING:		// ( int num, const char *string );
		return g_syscall(cmd, arg(0), ptr(1));
	case G_GET_CONFIGSTRING:		// ( int num, char *buffer, int bufferSize );
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case G_GET_USERINFO:			// ( int num, char *buffer, int bufferSize );
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case G_SET_USERINFO:			// ( int num, const char *buffer );
		return g_syscall(cmd, arg(0), ptr(1));
	case G_GET_SERVERINFO:			// ( char *buffer, int bufferSize );
		return g_syscall(cmd, ptr(0), arg(1));
	case G_SET_BRUSH_MODEL:			// ( gentity_t *ent, const char *name );
		return g_syscall(cmd, ptr(0), ptr(1));
	case G_TRACE:				// ( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), ptr(3), ptr(4), arg(5), arg(6));
	case G_POINT_CONTENTS:			// ( const vec3_t point, int passEntityNum );
		return g_syscall(cmd, ptr(0), arg(1));
	case G_IN_PVS:				// ( const vec3_t p1, const vec3_t p2 );
		return g_syscall(cmd, ptr(0), ptr(1));
	case G_IN_PVS_IGNORE_PORTALS:		// ( const vec3_t p1, const vec3_t p2 );
		return g_syscall(cmd, ptr(0), ptr(1));
	case G_ADJUST_AREA_PORTAL_STATE:	// ( gentity_t *ent, qboolean open );
		return g_syscall(cmd, ptr(0), arg(1));
	case G_AREAS_CONNECTED:			// ( int area1, int area2 );
		return g_syscall(cmd, arg(0), arg(1));
	case G_LINKENTITY:			// ( gentity_t *ent );
		return g_syscall(cmd, ptr(0));
	case G_UNLINKENTITY:			// ( gentity_t *ent );
		return g_syscall(cmd, ptr(0));
	case G_ENTITIES_IN_BOX:			// ( const vec3_t mins, const vec3_t maxs, gentity_t **list, int maxcount );
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), arg(3));
	case G_ENTITY_CONTACT:			// ( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2));
	case G_BOT_ALLOCATE_CLIENT:		// ( void );
		return g_syscall(cmd);
	case G_BOT_FREE_CLIENT:			// ( int clientNum );
		return g_syscall(cmd, arg(0));
	case G_GET_USERCMD:			// ( int clientNum, usercmd_t *cmd )
		return g_syscall(cmd, arg(0), ptr(1));
	case G_GET_ENTITY_TOKEN:		// qboolean ( char *buffer, int bufferSize )
		return g_syscall(cmd, ptr(0), arg(1));
	case G_FS_GETFILELIST:			//(  const char *path, const char *extension, char *listbuf, int bufsize ) {
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), arg(3));
	case G_DEBUG_POLYGON_CREATE:		//(int color, int numPoints, vec3_t *points)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case G_DEBUG_POLYGON_DELETE:		//(int id)
		return g_syscall(cmd, arg(0));
	case G_REAL_TIME:			//( qtime_t *qtime )
		return g_syscall(cmd, ptr(0));
	case G_SNAPVECTOR:			//(float *v)
		return g_syscall(cmd, ptr(0));
	case G_TRACECAPSULE:			// ( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
		return g_syscall(cmd, arg(0), arg(1), arg(2), arg(3), arg(4), arg(5));
	case G_ENTITY_CONTACTCAPSULE:		// ( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case SP_REGISTER_SERVER_CMD:		//( const char *package )
		return g_syscall(cmd, ptr(0));
	case SP_GETSTRINGTEXTSTRING:		//(const char *text, char *buffer, int bufferLength)
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case G_ROFF_CLEAN:			//( void )
		return g_syscall(cmd);
	case G_ROFF_UPDATE_ENTITIES:		//( void )
		return g_syscall(cmd);
	case G_ROFF_CACHE:			//( char *file ) 
		return g_syscall(cmd, ptr(0));
	case G_ROFF_PLAY:			//( int entID, int roffID, qboolean doTranslation ) 
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case G_ROFF_PURGE_ENT:			//( int entID ) 
		return g_syscall(cmd, arg(0));
	case G_MEMSET:				//memset(void* dest, int c, size_t count)
		return g_syscall(cmd, ptr(0), arg(1), arg(2));
	case G_MEMCPY:				//memcpy( void *dest, const void *src, size_t count )
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case G_STRNCPY:				//strncpy( char *strDest, const char *strSource, size_t count )
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case G_SIN:				//sin(double)
		return g_syscall(cmd, arg(0));
	case G_COS:				//cos(double)
		return g_syscall(cmd, arg(0));
	case G_ATAN2:				//atan2(double, double)
		return g_syscall(cmd, arg(0), arg(1));
	case G_SQRT:				//sqrt(double)
		return g_syscall(cmd, arg(0));
	case G_MATRIXMULTIPLY:			//(float in1[3][3], float in2[3][3], float out[3][3])
		return g_syscall(cmd, ptr(0), ptr(1), ptr(3));
	case G_ANGLEVECTORS:			//( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), ptr(3));
	case G_PERPENDICULARVECTOR:		//( vec3_t dst, const vec3_t src )
		return g_syscall(cmd, ptr(0), ptr(1));
	case G_FLOOR:				//floor(double)
		return g_syscall(cmd, arg(0));
	case G_CEIL:				//ceil(double)
		return g_syscall(cmd, arg(0));
	case G_TESTPRINTINT:			//(char*, int)
		return g_syscall(cmd, ptr(0), arg(1));
	case G_TESTPRINTFLOAT:			//(char*, float)
		return g_syscall(cmd, ptr(0), arg(1));
	case G_ACOS:				//( double x )
		return g_syscall(cmd, arg(0));
	case G_ASIN:				//not used, but probably ( double x )
		return g_syscall(cmd, arg(0));
	case BOTLIB_SETUP:			//(void)
		return g_syscall(cmd);
	case BOTLIB_SHUTDOWN:			//(void)
		return g_syscall(cmd);
	case BOTLIB_LIBVAR_SET:			//(char *var_name, char *value)
		return g_syscall(cmd, ptr(0), ptr(1));
	case BOTLIB_LIBVAR_GET:			//(char *var_name, char *value, int size)
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case BOTLIB_PC_ADD_GLOBAL_DEFINE:	//(char* string)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_START_FRAME:		//(float time)
		return g_syscall(cmd, arg(0));
	case BOTLIB_LOAD_MAP:			//(const char *mapname)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_UPDATENTITY:		//(int ent, void /* struct bot_updateentity_s */ *bue)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_TEST:			//(int parm0, char *parm1, vec3_t parm2, vec3_t parm3)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), ptr(3));
	case BOTLIB_GET_SNAPSHOT_ENTITY:	//( int clientNum, int sequence )
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_GET_CONSOLE_MESSAGE:	//(int clientNum, char *message, int size)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_USER_COMMAND:		//(int clientNum, usercmd_t *ucmd)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AAS_ENTITY_INFO:		//(int entnum, void /* struct aas_entityinfo_s */ *info)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AAS_INITIALIZED:		//(void)
		return g_syscall(cmd);
	case BOTLIB_AAS_PRESENCE_TYPE_BOUNDING_BOX:	//(int presencetype, vec3_t mins, vec3_t maxs)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AAS_TIME:			//(void)
		return g_syscall(cmd);
	case BOTLIB_AAS_POINT_AREA_NUM:		//(vec3_t point)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AAS_POINT_REACHABILITY_AREA_INDEX:	//(vec3_t point)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AAS_TRACE_AREAS:		//(vec3_t start, vec3_t end, int *areas, vec3_t *points, int maxareas)
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), ptr(3), arg(4));
	case BOTLIB_AAS_BBOX_AREAS:		//(vec3_t absmins, vec3_t absmaxs, int *areas, int maxareas)
		return g_syscall(cmd, ptr(0), ptr(1), ptr(2), arg(3));
	case BOTLIB_AAS_AREA_INFO:		//( int areanum, void /* struct aas_areainfo_s */ *info )
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AAS_POINT_CONTENTS:		//(vec3_t point)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AAS_NEXT_BSP_ENTITY:	//(int ent)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AAS_VALUE_FOR_BSP_EPAIR_KEY:	//(int ent, char *key, char *value, int size)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), arg(3));
	case BOTLIB_AAS_VECTOR_FOR_BSP_EPAIR_KEY:	//(int ent, char *key, vec3_t v)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AAS_FLOAT_FOR_BSP_EPAIR_KEY:	//(int ent, char *key, float *value)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AAS_INT_FOR_BSP_EPAIR_KEY:	//(int ent, char *key, int *value)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AAS_AREA_REACHABILITY:	//(int areanum)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AAS_AREA_TRAVEL_TIME_TO_GOAL_AREA:	//(int areanum, vec3_t origin, int goalareanum, int travelflags)
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3));
	case BOTLIB_AAS_ENABLE_ROUTING_AREA:	//(int areanum, int enable)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AAS_PREDICT_ROUTE:		//(void /*struct aas_predictroute_s*/ *route, int areanum, vec3_t origin, int goalareanum, int travelflags, int maxareas, int maxtime, int stopevent, int stopcontents, int stoptfl, int stopareanum)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3), arg(4), arg(5), arg(6), arg(7), arg(8), arg(9), arg(10), arg(11));
	case BOTLIB_AAS_ALTERNATIVE_ROUTE_GOAL:	//(vec3_t start, int startareanum, vec3_t goal, int goalareanum, int travelflags, void /*struct aas_altroutegoal_s*/ *altroutegoals, int maxaltroutegoals, int type)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3), arg(4), ptr(5), arg(6), arg(7));
	case BOTLIB_AAS_SWIMMING:		//(vec3_t origin)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AAS_PREDICT_CLIENT_MOVEMENT:	//(void /* struct aas_clientmove_s */ *move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3), arg(4), ptr(5), ptr(6), arg(7), arg(8), arg(9), arg(10), arg(11), arg(12));
	case BOTLIB_EA_SAY:			//(int client, char *str)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_EA_SAY_TEAM:		//(int client, char *str)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_EA_COMMAND:			//(int client, char *command)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_EA_ACTION:			//(int client, int action)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_EA_GESTURE:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_TALK:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_ATTACK:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_ALT_ATTACK:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_FORCEPOWER:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_USE:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_RESPAWN:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_CROUCH:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_UP:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_DOWN:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_FORWARD:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_BACK:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_LEFT:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE_RIGHT:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_SELECT_WEAPON:		//(int client, int weapon)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_EA_JUMP:			//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_DELAYED_JUMP:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_EA_MOVE:			//(int client, vec3_t dir, float speed)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_EA_VIEW:			//(int client, vec3_t viewangles)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_EA_END_REGULAR:		//(int client, float thinktime)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_EA_GET_INPUT:		//(int client, float thinktime, void /* struct bot_input_s */ *input)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case BOTLIB_EA_RESET_INPUT:		//(int client)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_LOAD_CHARACTER:		//(char *charfile, float skill)
		return g_syscall(cmd, ptr(0), arg(1));
	case BOTLIB_AI_FREE_CHARACTER:		//(int character)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_CHARACTERISTIC_FLOAT:	//(int character, int index)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_CHARACTERISTIC_BFLOAT:	//(int character, int index, float min, float max)
		return g_syscall(cmd, arg(0), arg(1), arg(2), arg(3));
	case BOTLIB_AI_CHARACTERISTIC_INTEGER:	//(int character, int index)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_CHARACTERISTIC_BINTEGER:	//(int character, int index, int min, int max)
		return g_syscall(cmd, arg(0), arg(1), arg(2), arg(3));
	case BOTLIB_AI_CHARACTERISTIC_STRING:	//(int character, int index, char *buf, int size)
		return g_syscall(cmd, arg(0), arg(1), ptr(2), arg(3));
	case BOTLIB_AI_ALLOC_CHAT_STATE:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_FREE_CHAT_STATE:		//(int handle)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_QUEUE_CONSOLE_MESSAGE:	//(int chatstate, int type, char *message)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case BOTLIB_AI_REMOVE_CONSOLE_MESSAGE:	//(int chatstate, int handle)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_NEXT_CONSOLE_MESSAGE:	//(int chatstate, void /* struct bot_consolemessage_s */ *cm)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_NUM_CONSOLE_MESSAGE:	//(int chatstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_INITIAL_CHAT:		//(int chatstate, char *type, int mcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 )
		return g_syscall(cmd, arg(0), ptr(1), arg(2), ptr(3), ptr(4), ptr(5), ptr(6), ptr(7), ptr(8), ptr(9), ptr(10));
	case BOTLIB_AI_NUM_INITIAL_CHATS:	//(int chatstate, char *type)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_REPLY_CHAT:		//(int chatstate, char *message, int mcontext, int vcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 )
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3), ptr(4), ptr(5), ptr(6), ptr(7), ptr(8), ptr(9), ptr(10), ptr(11));
	case BOTLIB_AI_CHAT_LENGTH:		//(int chatstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_ENTER_CHAT:		//(int chatstate, int client, int sendto)
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case BOTLIB_AI_GET_CHAT_MESSAGE:	//(int chatstate, char *buf, int size)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_AI_STRING_CONTAINS:		//(char *str1, char *str2, int casesensitive)
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case BOTLIB_AI_FIND_MATCH:		//(char *str, void /* struct bot_match_s */ *match, unsigned long int context)
		return g_syscall(cmd, ptr(0), ptr(1), arg(2));
	case BOTLIB_AI_MATCH_VARIABLE:		//(void /* struct bot_match_s */ *match, int variable, char *buf, int size)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3));
	case BOTLIB_AI_UNIFY_WHITE_SPACES:	//(char* string)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_AI_REPLACE_SYNONYMS:	//(char *string, unsigned long int context)
		return g_syscall(cmd, ptr(0), arg(1));
	case BOTLIB_AI_LOAD_CHAT_FILE:		//(int chatstate, char* chatfile, char* chatname)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AI_SET_CHAT_GENDER:		//(int chatstate, int gender)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_SET_CHAT_NAME:		//(int chatstate, char *name, int client)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_AI_RESET_GOAL_STATE:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_RESET_AVOID_GOALS:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_REMOVE_FROM_AVOID_GOALS:	//(int goalstate, int number)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_PUSH_GOAL:		//(int goalstate, void* goal)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_POP_GOAL:		//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_EMPTY_GOAL_STACK:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_DUMP_AVOID_GOALS:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_DUMP_GOAL_STACK:		//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_GOAL_NAME:		//(int number, char *name, int size)
		return g_syscall(cmd, arg(0), ptr(1), arg(2));
	case BOTLIB_AI_GET_TOP_GOAL:		//(int goalstate, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_GET_SECOND_GOAL:		//(int goalstate, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_CHOOSE_LTG_ITEM:		//(int goalstate, vec3_t origin, int *inventory, int travelflags)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), arg(3));
	case BOTLIB_AI_CHOOSE_NBG_ITEM:		//(int goalstate, vec3_t origin, int *inventory, int travelflags, void /* struct bot_goal_s */ *ltg, float maxtime)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), arg(3), ptr(4), arg(5));
	case BOTLIB_AI_TOUCHING_GOAL:		//(vec3_t origin, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, ptr(0), ptr(1));
	case BOTLIB_AI_ITEM_GOAL_IN_VIS_BUT_NOT_VISIBLE:	//(int viewer, vec3_t eye, vec3_t viewangles, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), ptr(3));
	case BOTLIB_AI_GET_LEVEL_ITEM_GOAL:	//(int index, char *classname, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case BOTLIB_AI_GET_NEXT_CAMP_SPOT_GOAL:	//(int num, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_GET_MAP_LOCATION_GOAL:	//(char *name, void /* struct bot_goal_s */ *goal)
		return g_syscall(cmd, ptr(0), ptr(1));
	case BOTLIB_AI_AVOID_GOAL_TIME:		//(int goalstate, int number)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_SET_AVOID_GOAL_TIME:	//(int goalstate, int number, float avoidtime)
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case BOTLIB_AI_INIT_LEVEL_ITEMS:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_UPDATE_ENTITY_ITEMS:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_LOAD_ITEM_WEIGHTS:	//(int, char*)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_FREE_ITEM_WEIGHTS:	//(int goalstate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_INTERBREED_GOAL_FUZZY_LOGIC:	//(int,int,int)
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case BOTLIB_AI_SAVE_GOAL_FUZZY_LOGIC:	//(int, char*)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_MUTATE_GOAL_FUZZY_LOGIC: //(int goalstate, float range)
		return g_syscall(cmd, arg(0), arg(1));
	case BOTLIB_AI_ALLOC_GOAL_STATE:	//(int state)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_FREE_GOAL_STATE:		//(int handle)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_RESET_MOVE_STATE:	//(int movestate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_ADD_AVOID_SPOT:		//(int movestate, vec3_t origin, float radius, int type)
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3));
	case BOTLIB_AI_MOVE_TO_GOAL:		//(void /* struct bot_moveresult_s */ *result, int movestate, void /* struct bot_goal_s */ *goal, int travelflags)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3));
	case BOTLIB_AI_MOVE_IN_DIRECTION:	//(int movestate, vec3_t dir, float speed, int type)
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3));
	case BOTLIB_AI_RESET_AVOID_REACH:	//(int movestate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_RESET_LAST_AVOID_REACH:	//(int movestate)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_REACHABILITY_AREA:	//(vec3_t origin, int testground)
		return g_syscall(cmd, ptr(0), arg(1));
	case BOTLIB_AI_MOVEMENT_VIEW_TARGET:	//(int movestate, void /* struct bot_goal_s */ *goal, int travelflags, float lookahead, vec3_t target)
		return g_syscall(cmd, arg(0), ptr(1), arg(2), arg(3), ptr(4));
	case BOTLIB_AI_PREDICT_VISIBLE_POSITION:	//(vec3_t origin, int areanum, void /* struct bot_goal_s */ *goal, int travelflags, vec3_t target)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), arg(3), ptr(4));
	case BOTLIB_AI_ALLOC_MOVE_STATE:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_FREE_MOVE_STATE:		//(int handle)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_INIT_MOVE_STATE:		//(int handle, void* initmove)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_CHOOSE_BEST_FIGHT_WEAPON: //(int weaponstate, int *inventory)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_GET_WEAPON_INFO:	//(int weaponstate, int weapon, void /* struct weaponinfo_s */ *weaponinfo)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case BOTLIB_AI_LOAD_WEAPON_WEIGHTS:	//(int, char*)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_AI_ALLOC_WEAPON_STATE:	//(void)
		return g_syscall(cmd);
	case BOTLIB_AI_FREE_WEAPON_STATE:	//(int)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_RESET_WEAPON_STATE:	//(int)
		return g_syscall(cmd, arg(0));
	case BOTLIB_AI_GENETIC_PARENTS_AND_CHILD_SELECTION:	//(int numranks, float *ranks, int *parent1, int *parent2, int *child)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2), ptr(3), ptr(4));
	case BOTLIB_PC_LOAD_SOURCE:		//(const char*)
		return g_syscall(cmd, ptr(0));
	case BOTLIB_PC_FREE_SOURCE:		//(int)
		return g_syscall(cmd, arg(0));
	case BOTLIB_PC_READ_TOKEN:		//(int, void*)
		return g_syscall(cmd, arg(0), ptr(1));
	case BOTLIB_PC_SOURCE_FILE_AND_LINE:	//( int handle, char *filename, int *line )
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	//do NOT convert the ghoul pointers, they are already real, passed in by the engine
	//(via double pointers, which must be converted once)
	case G_G2_LISTBONES:			//(void *ghlInfo, int frame)
		return g_syscall(cmd, arg(0), arg(1));
	case G_G2_LISTSURFACES:			//(void *ghlInfo)
		return g_syscall(cmd, arg(0));
	case G_G2_HAVEWEGHOULMODELS:		//(void *ghoul2)
		return g_syscall(cmd, arg(0));
	case G_G2_SETMODELS:			//(void *ghoul2, qhandle_t *modelList, qhandle_t *skinList)
		return g_syscall(cmd, arg(0), ptr(1), ptr(2));
	case G_G2_GETBOLT:			//(void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale)
		return g_syscall(cmd, arg(0), arg(1), arg(2), ptr(3), ptr(4), ptr(5), arg(6), ptr(7), ptr(8));
	case G_G2_GETBOLT_NOREC:		//(void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale)
		return g_syscall(cmd, arg(0), arg(1), arg(2), ptr(3), ptr(4), ptr(5), arg(6), ptr(7), ptr(8));
	case G_G2_GETBOLT_NOREC_NOROT:		//(void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale)
		return g_syscall(cmd, arg(0), arg(1), arg(2), ptr(3), ptr(4), ptr(5), arg(6), ptr(7), ptr(8));
	case G_G2_INITGHOUL2MODEL:		//(void **ghoul2Ptr, const char *fileName, int modelIndex, qhandle_t customSkin, qhandle_t customShader, int modelFlags, int lodBias)
		return g_syscall(cmd, ptr(0), ptr(1), arg(2), arg(3), arg(4), arg(5), arg(6));
	case G_G2_ADDBOLT:			//(void *ghoul2, int modelIndex, const char *boneName)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case G_G2_SETBOLTINFO:			//(void *ghoul2, int modelIndex, int boltInfo)
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case G_G2_ANGLEOVERRIDE:		//(void *ghoul2, int modelIndex, const char *boneName, const vec3_t angles, const int flags, const int up, const int right, const int forward, qhandle_t *modelList, int blendTime , int currentTime )
		return g_syscall(cmd, arg(0), arg(1), ptr(2), ptr(3), arg(4), arg(5), arg(6), arg(7), ptr(8), arg(9), arg(10));
	case G_G2_PLAYANIM:			//(void *ghoul2, const int modelIndex, const char *boneName, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame , const int blendTime )
		return g_syscall(cmd, arg(0), arg(1), ptr(2), arg(3), arg(4), arg(5), arg(6), arg(7), ptr(8), arg(9));
	case G_G2_GETGLANAME:			//(void *ghoul2, int modelIndex, char *fillBuf)
		return g_syscall(cmd, arg(0), arg(1), ptr(2));
	case G_G2_COPYGHOUL2INSTANCE:		//(void *g2From, void *g2To, int modelIndex)
		return g_syscall(cmd, arg(0), arg(1), arg(2));
	case G_G2_COPYSPECIFICGHOUL2MODEL:	//(void *g2From, int modelFrom, void *g2To, int modelTo)
		return g_syscall(cmd, arg(0), arg(1), arg(2), arg(3));
	case G_G2_DUPLICATEGHOUL2INSTANCE:	//(void *g2From, void **g2To)
		return g_syscall(cmd, arg(0), ptr(1));
	case G_G2_HASGHOUL2MODELONINDEX:	//(void *ghlInfo, int modelIndex)
		return g_syscall(cmd, arg(0), arg(1));
	case G_G2_REMOVEGHOUL2MODEL:		//(void *ghlInfo, int modelIndex)
		return g_syscall(cmd, arg(0), arg(1));
	case G_G2_CLEANMODELS:			//(void **ghoul2Ptr)
		return g_syscall(cmd, ptr(0));
	case G_G2_COLLISIONDETECT:		//(CollisionRecord_t *collRecMap, void* ghoul2, const vec3_t angles, const vec3_t position,int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod, float fRadius)
		return g_syscall(cmd, ptr(0), arg(1), ptr(2), ptr(3), arg(4), arg(5), ptr(6), ptr(7), ptr(8), arg(9), arg(10), arg(11));
	/* QMMVM TRAP FUNCTIONS
		QMMVM has been modified to understand pointer values larger than
		the VM's dataSegmentLen to be real pointers, and it will properly
		dereference them.
	*/
	case QMMVM_G_MALLOC:			//void* malloc(size_t size)
		return (int)malloc((size_t)arg(0));
	case QMMVM_G_REALLOC:			//void* realloc(void* addr, size_t size)
		return (int)realloc(ptr(0), (size_t)arg(1));
	case QMMVM_G_FREE:			//free(void* addr)
		free(ptr(0));
#endif
	default:				//bad trap (ignore it, print error to console)
		return g_syscall(G_PRINT, vaf("[QMMVM] ERROR: Unhandled syscall: %s (%i)\n", engfuncname(cmd), cmd));
	}

	return g_syscall(G_PRINT, vaf("[QMMVM] ERROR: Unhandled syscall: %s (%i)\n", engfuncname(cmd), cmd));;
}
