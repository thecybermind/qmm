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

/* $Id: game_q3a.cpp,v 1.6 2006/01/29 22:45:37 cybermind Exp $ */

#include <q3a/game/q_shared.h>
#include <q3a/game/g_public.h>
#include "CEngineMgr.h"
#include "qmm.h"
#include "game_api.h"
#include "util.h"

GEN_MSGS(Q3A);

//these functions are defined in the g_syscalls.asm file but not used
#define G_MEMSET		100
#define G_MEMCPY		101
#define G_STRNCPY		102
#define G_SIN			103
#define G_COS			104
#define G_ATAN2			105
#define G_SQRT			106
#define G_FLOOR			110
#define G_CEIL			111
#define G_TESTPRINTINT		112
#define G_TESTPRINTFLOAT	113

const char* Q3A_eng_msg_names(int cmd) {
	switch(cmd) {
		case G_PRINT:
			return "G_PRINT";
		case G_ERROR:
			return "G_ERROR";
		case G_MILLISECONDS:
			return "G_MILLISECONDS";
		case G_CVAR_REGISTER:
			return "G_CVAR_REGISTER";
		case G_CVAR_UPDATE:
			return "G_CVAR_UPDATE";
		case G_CVAR_SET:
			return "G_CVAR_SET";
		case G_CVAR_VARIABLE_INTEGER_VALUE:
			return "G_CVAR_VARIABLE_INTEGER_VALUE";
		case G_CVAR_VARIABLE_STRING_BUFFER:
			return "G_CVAR_VARIABLE_STRING_BUFFER";
		case G_ARGC:
			return "G_ARGC";
		case G_ARGV:
			return "G_ARGV";
		case G_FS_FOPEN_FILE:
			return "G_FS_FOPEN_FILE";
		case G_FS_READ:
			return "G_FS_READ";
		case G_FS_WRITE:
			return "G_FS_WRITE";
		case G_FS_FCLOSE_FILE:
			return "G_FS_FCLOSE_FILE";
		case G_SEND_CONSOLE_COMMAND:
			return "G_SEND_CONSOLE_COMMAND";
		case G_LOCATE_GAME_DATA:
			return "G_LOCATE_GAME_DATA";
		case G_DROP_CLIENT:
			return "G_DROP_CLIENT";
		case G_SEND_SERVER_COMMAND:
			return "G_SEND_SERVER_COMMAND";
		case G_SET_CONFIGSTRING:
			return "G_SET_CONFIGSTRING";
		case G_GET_CONFIGSTRING:
			return "G_GET_CONFIGSTRING";
		case G_GET_USERINFO:
			return "G_GET_USERINFO";
		case G_SET_USERINFO:
			return "G_SET_USERINFO";
		case G_GET_SERVERINFO:
			return "G_GET_SERVERINFO";
		case G_SET_BRUSH_MODEL:
			return "G_SET_BRUSH_MODEL";
		case G_TRACE:
			return "G_TRACE";
		case G_POINT_CONTENTS:
			return "G_POINT_CONTENTS";
		case G_IN_PVS:
			return "G_IN_PVS";
		case G_IN_PVS_IGNORE_PORTALS:
			return "G_IN_PVS_IGNORE_PORTALS";
		case G_ADJUST_AREA_PORTAL_STATE:
			return "G_ADJUST_AREA_PORTAL_STATE";
		case G_AREAS_CONNECTED:
			return "G_AREAS_CONNECTED";
		case G_LINKENTITY:
			return "G_LINKENTITY";
		case G_UNLINKENTITY:
			return "G_UNLINKENTITY";
		case G_ENTITIES_IN_BOX:
			return "G_ENTITIES_IN_BOX";
		case G_ENTITY_CONTACT:
			return "G_ENTITY_CONTACT";
		case G_BOT_ALLOCATE_CLIENT:
			return "G_BOT_ALLOCATE_CLIENT";
		case G_BOT_FREE_CLIENT:
			return "G_BOT_FREE_CLIENT";
		case G_GET_USERCMD:
			return "G_GET_USERCMD";
		case G_GET_ENTITY_TOKEN:
			return "G_GET_ENTITY_TOKEN";
		case G_FS_GETFILELIST:
			return "G_FS_GETFILELIST";
		case G_DEBUG_POLYGON_CREATE:
			return "G_DEBUG_POLYGON_CREATE";
		case G_DEBUG_POLYGON_DELETE:
			return "G_DEBUG_POLYGON_DELETE";
		case G_REAL_TIME:
			return "G_REAL_TIME";
		case G_SNAPVECTOR:
			return "G_SNAPVECTOR";
		case G_TRACECAPSULE:
			return "G_TRACECAPSULE";
		case G_ENTITY_CONTACTCAPSULE:
			return "G_ENTITY_CONTACTCAPSULE";
		case G_FS_SEEK:
			return "G_FS_SEEK";
		case BOTLIB_SETUP:
			return "BOTLIB_SETUP";
		case BOTLIB_SHUTDOWN:
			return "BOTLIB_SHUTDOWN";
		case BOTLIB_LIBVAR_SET:
			return "BOTLIB_LIBVAR_SET";
		case BOTLIB_LIBVAR_GET:
			return "BOTLIB_LIBVAR_GET";
		case BOTLIB_PC_ADD_GLOBAL_DEFINE:
			return "BOTLIB_PC_ADD_GLOBAL_DEFINE";
		case BOTLIB_START_FRAME:
			return "BOTLIB_START_FRAME";
		case BOTLIB_LOAD_MAP:
			return "BOTLIB_LOAD_MAP";
		case BOTLIB_UPDATENTITY:
			return "BOTLIB_UPDATENTITY";
		case BOTLIB_TEST:
			return "BOTLIB_TEST";
		case BOTLIB_GET_SNAPSHOT_ENTITY:
			return "BOTLIB_GET_SNAPSHOT_ENTITY";
		case BOTLIB_GET_CONSOLE_MESSAGE:
			return "BOTLIB_GET_CONSOLE_MESSAGE";
		case BOTLIB_USER_COMMAND:
			return "BOTLIB_USER_COMMAND";
		case BOTLIB_AAS_ENTITY_INFO:
			return "BOTLIB_AAS_ENTITY_INFO";
		case BOTLIB_AAS_INITIALIZED:
			return "BOTLIB_AAS_INITIALIZED";
		case BOTLIB_AAS_PRESENCE_TYPE_BOUNDING_BOX:
			return "BOTLIB_AAS_PRESENCE_TYPE_BOUNDING_BOX";
		case BOTLIB_AAS_TIME:
			return "BOTLIB_AAS_TIME";
		case BOTLIB_AAS_POINT_AREA_NUM:
			return "BOTLIB_AAS_POINT_AREA_NUM";
		case BOTLIB_AAS_POINT_REACHABILITY_AREA_INDEX:
			return "BOTLIB_AAS_POINT_REACHABILITY_AREA_INDEX";
		case BOTLIB_AAS_TRACE_AREAS:
			return "BOTLIB_AAS_TRACE_AREAS";
		case BOTLIB_AAS_BBOX_AREAS:
			return "BOTLIB_AAS_BBOX_AREAS";
		case BOTLIB_AAS_AREA_INFO:
			return "BOTLIB_AAS_AREA_INFO";
		case BOTLIB_AAS_POINT_CONTENTS:
			return "BOTLIB_AAS_POINT_CONTENTS";
		case BOTLIB_AAS_NEXT_BSP_ENTITY:
			return "BOTLIB_AAS_NEXT_BSP_ENTITY";
		case BOTLIB_AAS_VALUE_FOR_BSP_EPAIR_KEY:
			return "BOTLIB_AAS_VALUE_FOR_BSP_EPAIR_KEY";
		case BOTLIB_AAS_VECTOR_FOR_BSP_EPAIR_KEY:
			return "BOTLIB_AAS_VECTOR_FOR_BSP_EPAIR_KEY";
		case BOTLIB_AAS_FLOAT_FOR_BSP_EPAIR_KEY:
			return "BOTLIB_AAS_FLOAT_FOR_BSP_EPAIR_KEY";
		case BOTLIB_AAS_INT_FOR_BSP_EPAIR_KEY:
			return "BOTLIB_AAS_INT_FOR_BSP_EPAIR_KEY";
		case BOTLIB_AAS_AREA_REACHABILITY:
			return "BOTLIB_AAS_AREA_REACHABILITY";
		case BOTLIB_AAS_AREA_TRAVEL_TIME_TO_GOAL_AREA:
			return "BOTLIB_AAS_AREA_TRAVEL_TIME_TO_GOAL_AREA";
		case BOTLIB_AAS_ENABLE_ROUTING_AREA:
			return "BOTLIB_AAS_ENABLE_ROUTING_AREA";
		case BOTLIB_AAS_PREDICT_ROUTE:
			return "BOTLIB_AAS_PREDICT_ROUTE";
		case BOTLIB_AAS_ALTERNATIVE_ROUTE_GOAL:
			return "BOTLIB_AAS_ALTERNATIVE_ROUTE_GOAL";
		case BOTLIB_AAS_SWIMMING:
			return "BOTLIB_AAS_SWIMMING";
		case BOTLIB_AAS_PREDICT_CLIENT_MOVEMENT:
			return "BOTLIB_AAS_PREDICT_CLIENT_MOVEMENT";
		case BOTLIB_EA_SAY:
			return "BOTLIB_EA_SAY";
		case BOTLIB_EA_SAY_TEAM:
			return "BOTLIB_EA_SAY_TEAM";
		case BOTLIB_EA_COMMAND:
			return "BOTLIB_EA_COMMAND";
		case BOTLIB_EA_ACTION:
			return "BOTLIB_EA_ACTION";
		case BOTLIB_EA_GESTURE:
			return "BOTLIB_EA_GESTURE";
		case BOTLIB_EA_TALK:
			return "BOTLIB_EA_TALK";
		case BOTLIB_EA_ATTACK:
			return "BOTLIB_EA_ATTACK";
		case BOTLIB_EA_USE:
			return "BOTLIB_EA_USE";
		case BOTLIB_EA_RESPAWN:
			return "BOTLIB_EA_RESPAWN";
		case BOTLIB_EA_CROUCH:
			return "BOTLIB_EA_CROUCH";
		case BOTLIB_EA_MOVE_UP:
			return "BOTLIB_EA_MOVE_UP";
		case BOTLIB_EA_MOVE_DOWN:
			return "BOTLIB_EA_MOVE_DOWN";
		case BOTLIB_EA_MOVE_FORWARD:
			return "BOTLIB_EA_MOVE_FORWARD";
		case BOTLIB_EA_MOVE_BACK:
			return "BOTLIB_EA_MOVE_BACK";
		case BOTLIB_EA_MOVE_LEFT:
			return "BOTLIB_EA_MOVE_LEFT";
		case BOTLIB_EA_MOVE_RIGHT:
			return "BOTLIB_EA_MOVE_RIGHT";
		case BOTLIB_EA_SELECT_WEAPON:
			return "BOTLIB_EA_SELECT_WEAPON";
		case BOTLIB_EA_JUMP:
			return "BOTLIB_EA_JUMP";
		case BOTLIB_EA_DELAYED_JUMP:
			return "BOTLIB_EA_DELAYED_JUMP";
		case BOTLIB_EA_MOVE:
			return "BOTLIB_EA_MOVE";
		case BOTLIB_EA_VIEW:
			return "BOTLIB_EA_VIEW";
		case BOTLIB_EA_END_REGULAR:
			return "BOTLIB_EA_END_REGULAR";
		case BOTLIB_EA_GET_INPUT:
			return "BOTLIB_EA_GET_INPUT";
		case BOTLIB_EA_RESET_INPUT:
			return "BOTLIB_EA_RESET_INPUT";
		case BOTLIB_AI_LOAD_CHARACTER:
			return "BOTLIB_AI_LOAD_CHARACTER";
		case BOTLIB_AI_FREE_CHARACTER:
			return "BOTLIB_AI_FREE_CHARACTER";
		case BOTLIB_AI_CHARACTERISTIC_FLOAT:
			return "BOTLIB_AI_CHARACTERISTIC_FLOAT";
		case BOTLIB_AI_CHARACTERISTIC_BFLOAT:
			return "BOTLIB_AI_CHARACTERISTIC_BFLOAT";
		case BOTLIB_AI_CHARACTERISTIC_INTEGER:
			return "BOTLIB_AI_CHARACTERISTIC_INTEGER";
		case BOTLIB_AI_CHARACTERISTIC_BINTEGER:
			return "BOTLIB_AI_CHARACTERISTIC_BINTEGER";
		case BOTLIB_AI_CHARACTERISTIC_STRING:
			return "BOTLIB_AI_CHARACTERISTIC_STRING";
		case BOTLIB_AI_ALLOC_CHAT_STATE:
			return "BOTLIB_AI_ALLOC_CHAT_STATE";
		case BOTLIB_AI_FREE_CHAT_STATE:
			return "BOTLIB_AI_FREE_CHAT_STATE";
		case BOTLIB_AI_QUEUE_CONSOLE_MESSAGE:
			return "BOTLIB_AI_QUEUE_CONSOLE_MESSAGE";
		case BOTLIB_AI_REMOVE_CONSOLE_MESSAGE:
			return "BOTLIB_AI_REMOVE_CONSOLE_MESSAGE";
		case BOTLIB_AI_NEXT_CONSOLE_MESSAGE:
			return "BOTLIB_AI_NEXT_CONSOLE_MESSAGE";
		case BOTLIB_AI_NUM_CONSOLE_MESSAGE:
			return "BOTLIB_AI_NUM_CONSOLE_MESSAGE";
		case BOTLIB_AI_INITIAL_CHAT:
			return "BOTLIB_AI_INITIAL_CHAT";
		case BOTLIB_AI_NUM_INITIAL_CHATS:
			return "BOTLIB_AI_NUM_INITIAL_CHATS";
		case BOTLIB_AI_REPLY_CHAT:
			return "BOTLIB_AI_REPLY_CHAT";
		case BOTLIB_AI_CHAT_LENGTH:
			return "BOTLIB_AI_CHAT_LENGTH";
		case BOTLIB_AI_ENTER_CHAT:
			return "BOTLIB_AI_ENTER_CHAT";
		case BOTLIB_AI_GET_CHAT_MESSAGE:
			return "BOTLIB_AI_GET_CHAT_MESSAGE";
		case BOTLIB_AI_STRING_CONTAINS:
			return "BOTLIB_AI_STRING_CONTAINS";
		case BOTLIB_AI_FIND_MATCH:
			return "BOTLIB_AI_FIND_MATCH";
		case BOTLIB_AI_MATCH_VARIABLE:
			return "BOTLIB_AI_MATCH_VARIABLE";
		case BOTLIB_AI_UNIFY_WHITE_SPACES:
			return "BOTLIB_AI_UNIFY_WHITE_SPACES";
		case BOTLIB_AI_REPLACE_SYNONYMS:
			return "BOTLIB_AI_REPLACE_SYNONYMS";
		case BOTLIB_AI_LOAD_CHAT_FILE:
			return "BOTLIB_AI_LOAD_CHAT_FILE";
		case BOTLIB_AI_SET_CHAT_GENDER:
			return "BOTLIB_AI_SET_CHAT_GENDER";
		case BOTLIB_AI_SET_CHAT_NAME:
			return "BOTLIB_AI_SET_CHAT_NAME";
		case BOTLIB_AI_RESET_GOAL_STATE:
			return "BOTLIB_AI_RESET_GOAL_STATE";
		case BOTLIB_AI_RESET_AVOID_GOALS:
			return "BOTLIB_AI_RESET_AVOID_GOALS";
		case BOTLIB_AI_REMOVE_FROM_AVOID_GOALS:
			return "BOTLIB_AI_REMOVE_FROM_AVOID_GOALS";
		case BOTLIB_AI_PUSH_GOAL:
			return "BOTLIB_AI_PUSH_GOAL";
		case BOTLIB_AI_POP_GOAL:
			return "BOTLIB_AI_POP_GOAL";
		case BOTLIB_AI_EMPTY_GOAL_STACK:
			return "BOTLIB_AI_EMPTY_GOAL_STACK";
		case BOTLIB_AI_DUMP_AVOID_GOALS:
			return "BOTLIB_AI_DUMP_AVOID_GOALS";
		case BOTLIB_AI_DUMP_GOAL_STACK:
			return "BOTLIB_AI_DUMP_GOAL_STACK";
		case BOTLIB_AI_GOAL_NAME:
			return "BOTLIB_AI_GOAL_NAME";
		case BOTLIB_AI_GET_TOP_GOAL:
			return "BOTLIB_AI_GET_TOP_GOAL";
		case BOTLIB_AI_GET_SECOND_GOAL:
			return "BOTLIB_AI_GET_SECOND_GOAL";
		case BOTLIB_AI_CHOOSE_LTG_ITEM:
			return "BOTLIB_AI_CHOOSE_LTG_ITEM";
		case BOTLIB_AI_CHOOSE_NBG_ITEM:
			return "BOTLIB_AI_CHOOSE_NBG_ITEM";
		case BOTLIB_AI_TOUCHING_GOAL:
			return "BOTLIB_AI_TOUCHING_GOAL";
		case BOTLIB_AI_ITEM_GOAL_IN_VIS_BUT_NOT_VISIBLE:
			return "BOTLIB_AI_ITEM_GOAL_IN_VIS_BUT_NOT_VISIBLE";
		case BOTLIB_AI_GET_LEVEL_ITEM_GOAL:
			return "BOTLIB_AI_GET_LEVEL_ITEM_GOAL";
		case BOTLIB_AI_GET_NEXT_CAMP_SPOT_GOAL:
			return "BOTLIB_AI_GET_NEXT_CAMP_SPOT_GOAL";
		case BOTLIB_AI_GET_MAP_LOCATION_GOAL:
			return "BOTLIB_AI_GET_MAP_LOCATION_GOAL";
		case BOTLIB_AI_AVOID_GOAL_TIME:
			return "BOTLIB_AI_AVOID_GOAL_TIME";
		case BOTLIB_AI_SET_AVOID_GOAL_TIME:
			return "BOTLIB_AI_SET_AVOID_GOAL_TIME";
		case BOTLIB_AI_INIT_LEVEL_ITEMS:
			return "BOTLIB_AI_INIT_LEVEL_ITEMS";
		case BOTLIB_AI_UPDATE_ENTITY_ITEMS:
			return "BOTLIB_AI_UPDATE_ENTITY_ITEMS";
		case BOTLIB_AI_LOAD_ITEM_WEIGHTS:
			return "BOTLIB_AI_LOAD_ITEM_WEIGHTS";
		case BOTLIB_AI_FREE_ITEM_WEIGHTS:
			return "BOTLIB_AI_FREE_ITEM_WEIGHTS";
		case BOTLIB_AI_INTERBREED_GOAL_FUZZY_LOGIC:
			return "BOTLIB_AI_INTERBREED_GOAL_FUZZY_LOGIC";
		case BOTLIB_AI_SAVE_GOAL_FUZZY_LOGIC:
			return "BOTLIB_AI_SAVE_GOAL_FUZZY_LOGIC";
		case BOTLIB_AI_MUTATE_GOAL_FUZZY_LOGIC:
			return "BOTLIB_AI_MUTATE_GOAL_FUZZY_LOGIC";
		case BOTLIB_AI_ALLOC_GOAL_STATE:
			return "BOTLIB_AI_ALLOC_GOAL_STATE";
		case BOTLIB_AI_FREE_GOAL_STATE:
			return "BOTLIB_AI_FREE_GOAL_STATE";
		case BOTLIB_AI_RESET_MOVE_STATE:
			return "BOTLIB_AI_RESET_MOVE_STATE";
		case BOTLIB_AI_ADD_AVOID_SPOT:
			return "BOTLIB_AI_ADD_AVOID_SPOT";
		case BOTLIB_AI_MOVE_TO_GOAL:
			return "BOTLIB_AI_MOVE_TO_GOAL";
		case BOTLIB_AI_MOVE_IN_DIRECTION:
			return "BOTLIB_AI_MOVE_IN_DIRECTION";
		case BOTLIB_AI_RESET_AVOID_REACH:
			return "BOTLIB_AI_RESET_AVOID_REACH";
		case BOTLIB_AI_RESET_LAST_AVOID_REACH:
			return "BOTLIB_AI_RESET_LAST_AVOID_REACH";
		case BOTLIB_AI_REACHABILITY_AREA:
			return "BOTLIB_AI_REACHABILITY_AREA";
		case BOTLIB_AI_MOVEMENT_VIEW_TARGET:
			return "BOTLIB_AI_MOVEMENT_VIEW_TARGET";
		case BOTLIB_AI_PREDICT_VISIBLE_POSITION:
			return "BOTLIB_AI_PREDICT_VISIBLE_POSITION";
		case BOTLIB_AI_ALLOC_MOVE_STATE:
			return "BOTLIB_AI_ALLOC_MOVE_STATE";
		case BOTLIB_AI_FREE_MOVE_STATE:
			return "BOTLIB_AI_FREE_MOVE_STATE";
		case BOTLIB_AI_INIT_MOVE_STATE:
			return "BOTLIB_AI_INIT_MOVE_STATE";
		case BOTLIB_AI_CHOOSE_BEST_FIGHT_WEAPON:
			return "BOTLIB_AI_CHOOSE_BEST_FIGHT_WEAPON";
		case BOTLIB_AI_GET_WEAPON_INFO:
			return "BOTLIB_AI_GET_WEAPON_INFO";
		case BOTLIB_AI_LOAD_WEAPON_WEIGHTS:
			return "BOTLIB_AI_LOAD_WEAPON_WEIGHTS";
		case BOTLIB_AI_ALLOC_WEAPON_STATE:
			return "BOTLIB_AI_ALLOC_WEAPON_STATE";
		case BOTLIB_AI_FREE_WEAPON_STATE:
			return "BOTLIB_AI_FREE_WEAPON_STATE";
		case BOTLIB_AI_RESET_WEAPON_STATE:
			return "BOTLIB_AI_RESET_WEAPON_STATE";
		case BOTLIB_AI_GENETIC_PARENTS_AND_CHILD_SELECTION:
			return "BOTLIB_AI_GENETIC_PARENTS_AND_CHILD_SELECTION";
		case BOTLIB_PC_LOAD_SOURCE:
			return "BOTLIB_PC_LOAD_SOURCE";
		case BOTLIB_PC_FREE_SOURCE:
			return "BOTLIB_PC_FREE_SOURCE";
		case BOTLIB_PC_READ_TOKEN:
			return "BOTLIB_PC_READ_TOKEN";
		case BOTLIB_PC_SOURCE_FILE_AND_LINE:
			return "BOTLIB_PC_SOURCE_FILE_AND_LINE";
		case G_MEMSET:
			return "G_MEMSET";
		case G_MEMCPY:
			return "G_MEMCPY";
		case G_STRNCPY:
			return "G_STRNCPY";
		case G_SIN:
			return "G_SIN";
		case G_COS:
			return "G_COS";
		case G_ATAN2:
			return "G_ATAN2";
		case G_SQRT:
			return "G_SQRT";
		case G_FLOOR:
			return "G_FLOOR";
		case G_CEIL:
			return "G_CEIL";
		case G_TESTPRINTINT:
			return "G_TESTPRINTINT";
		case G_TESTPRINTFLOAT:
			return "G_TESTPRINTFLOAT";
		default:
			return "unknown";
	}
}

const char* Q3A_mod_msg_names(int cmd) {
	switch(cmd) {
		case GAME_INIT:
			return "GAME_INIT";
		case GAME_SHUTDOWN:
			return "GAME_SHUTDOWN";
		case GAME_CLIENT_CONNECT:
			return "GAME_CLIENT_CONNECT";
		case GAME_CLIENT_BEGIN:
			return "GAME_CLIENT_BEGIN";
		case GAME_CLIENT_USERINFO_CHANGED:
			return "GAME_CLIENT_USERINFO_CHANGED";
		case GAME_CLIENT_DISCONNECT:
			return "GAME_CLIENT_DISCONNECT";
		case GAME_CLIENT_COMMAND:
			return "GAME_CLIENT_COMMAND";
		case GAME_CLIENT_THINK:
			return "GAME_CLIENT_THINK";
		case GAME_RUN_FRAME:
			return "GAME_RUN_FRAME";
		case GAME_CONSOLE_COMMAND:
			return "GAME_CONSOLE_COMMAND";
		case BOTAI_START_FRAME:
			return "BOTAI_START_FRAME";
		default:
			return "unknown";
	}
}

//setting *x = 0 means that no matching function was found
int Q3A_vmsyscall(byte* membase, int cmd, int* args) {
	register int cmd2 = cmd;
	switch(cmd2) {
		case G_MILLISECONDS:			//(void)
		case G_ARGC:				//(void)
		case G_BOT_ALLOCATE_CLIENT:		//(void)
		case BOTLIB_SETUP:			//(void)
		case BOTLIB_SHUTDOWN:			//(void)
		case BOTLIB_AAS_INITIALIZED:		//(void)
		case BOTLIB_AAS_TIME:			//(void)
		case BOTLIB_AI_ALLOC_CHAT_STATE:	//(void)
		case BOTLIB_AI_INIT_LEVEL_ITEMS:	//(void)
		case BOTLIB_AI_UPDATE_ENTITY_ITEMS:	//(void)
		case BOTLIB_AI_ALLOC_MOVE_STATE:	//(void)
		case BOTLIB_AI_ALLOC_WEAPON_STATE:	//(void)
			return QMM_SYSCALL(cmd);
		case G_PRINT:				//(const char* string);
		case G_ERROR:				//(const char* string);
		case G_CVAR_UPDATE:			//(vmCvar_t* vmCvar);
		case G_CVAR_VARIABLE_INTEGER_VALUE:	//(const char* var_name);
		case G_LINKENTITY:			//(gentity_t* ent);
		case G_UNLINKENTITY:			//(gentity_t* ent);
		case G_REAL_TIME:			//(qtime_t* qtime)
		case G_SNAPVECTOR:			//(float* v)
		case BOTLIB_PC_ADD_GLOBAL_DEFINE:	//(char* string)
		case BOTLIB_LOAD_MAP:			//(const char* mapname)
		case BOTLIB_AAS_POINT_AREA_NUM:		//(vec3_t point)
		case BOTLIB_AAS_POINT_REACHABILITY_AREA_INDEX:	//(vec3_t point)
		case BOTLIB_AAS_POINT_CONTENTS:		//(vec3_t point)
		case BOTLIB_AAS_SWIMMING:		//(vec3_t origin)
		case BOTLIB_AI_UNIFY_WHITE_SPACES:	//(char* string)
		case BOTLIB_PC_LOAD_SOURCE:		//(const char*)
			return QMM_SYSCALL(cmd, vmptr(0));
		case G_FS_FCLOSE_FILE:			//(fileHandle_t f);
		case G_BOT_FREE_CLIENT:			//(int clientNum);
		case G_DEBUG_POLYGON_DELETE:		//(int id)
		case BOTLIB_START_FRAME:		//(float time)
		case BOTLIB_AAS_NEXT_BSP_ENTITY:	//(int ent)
		case BOTLIB_AAS_AREA_REACHABILITY:	//(int areanum)
		case BOTLIB_EA_GESTURE:			//(int client)
		case BOTLIB_EA_TALK:			//(int client)
		case BOTLIB_EA_ATTACK:			//(int client)
		case BOTLIB_EA_USE:			//(int client)
		case BOTLIB_EA_RESPAWN:			//(int client)
		case BOTLIB_EA_CROUCH:			//(int client)
		case BOTLIB_EA_MOVE_UP:			//(int client)
		case BOTLIB_EA_MOVE_DOWN:		//(int client)
		case BOTLIB_EA_MOVE_FORWARD:		//(int client)
		case BOTLIB_EA_MOVE_BACK:		//(int client)
		case BOTLIB_EA_MOVE_LEFT:		//(int client)
		case BOTLIB_EA_MOVE_RIGHT:		//(int client)
		case BOTLIB_EA_JUMP:			//(int client)
		case BOTLIB_EA_DELAYED_JUMP:		//(int client)
		case BOTLIB_EA_RESET_INPUT:		//(int client)
		case BOTLIB_AI_FREE_CHARACTER:		//(int character)
		case BOTLIB_AI_FREE_CHAT_STATE:		//(int handle)
		case BOTLIB_AI_CHAT_LENGTH:		//(int chatstate)
		case BOTLIB_AI_NUM_CONSOLE_MESSAGE:	//(int chatstate)
		case BOTLIB_AI_RESET_GOAL_STATE:	//(int goalstate)
		case BOTLIB_AI_RESET_AVOID_GOALS:	//(int goalstate)
		case BOTLIB_AI_POP_GOAL:		//(int goalstate)
		case BOTLIB_AI_EMPTY_GOAL_STACK:	//(int goalstate)
		case BOTLIB_AI_DUMP_AVOID_GOALS:	//(int goalstate)
		case BOTLIB_AI_DUMP_GOAL_STACK:		//(int goalstate)
		case BOTLIB_AI_FREE_ITEM_WEIGHTS:	//(int goalstate)
		case BOTLIB_AI_ALLOC_GOAL_STATE:	//(int state)
		case BOTLIB_AI_FREE_GOAL_STATE:		//(int handle)
		case BOTLIB_AI_RESET_MOVE_STATE:	//(int movestate)
		case BOTLIB_AI_RESET_AVOID_REACH:	//(int movestate)
		case BOTLIB_AI_RESET_LAST_AVOID_REACH:	//(int movestate)
		case BOTLIB_AI_FREE_MOVE_STATE:		//(int handle)
		case BOTLIB_AI_FREE_WEAPON_STATE:	//(int)
		case BOTLIB_AI_RESET_WEAPON_STATE:	//(int)
		case BOTLIB_PC_FREE_SOURCE:		//(int)
		case G_SIN:				//(double)
		case G_COS:				//(double)
		case G_SQRT:				//(double)
		case G_FLOOR:				//(double)
		case G_CEIL:				//(double)
			return QMM_SYSCALL(cmd, vmarg(0));
		case G_CVAR_SET:			//(const char* var_name, const char* value);
		case G_SET_BRUSH_MODEL:			//(gentity_t* ent, const char* name);
		case G_IN_PVS:				//(const vec3_t p1, const vec3_t p2);
		case G_IN_PVS_IGNORE_PORTALS:		//(const vec3_t p1, const vec3_t p2);
		case BOTLIB_LIBVAR_SET:			//(char* var_name, char* value)
		case BOTLIB_AI_TOUCHING_GOAL:		//(vec3_t origin, void /* struct bot_goal_s* /* goal)
		case BOTLIB_AI_GET_MAP_LOCATION_GOAL:	//(char* name, void /* struct bot_goal_s* /* goal)
			return QMM_SYSCALL(cmd, vmptr(0), vmptr(1));
		case G_AREAS_CONNECTED:			//(int area1, int area2);
		case BOTLIB_GET_SNAPSHOT_ENTITY:	//(int clientNum, int sequence)
		case BOTLIB_AAS_ENABLE_ROUTING_AREA:	//(int areanum, int enable)
		case BOTLIB_EA_ACTION:			//(int client, int action)
		case BOTLIB_EA_SELECT_WEAPON:		//(int client, int weapon)
		case BOTLIB_EA_END_REGULAR:		//(int client, float thinktime)
		case BOTLIB_AI_CHARACTERISTIC_FLOAT:	//(int character, int index)
		case BOTLIB_AI_CHARACTERISTIC_INTEGER:	//(int character, int index)
		case BOTLIB_AI_REMOVE_CONSOLE_MESSAGE:	//(int chatstate, int handle)
		case BOTLIB_AI_SET_CHAT_GENDER:		//(int chatstate, int gender)
		case BOTLIB_AI_REMOVE_FROM_AVOID_GOALS:	//(int goalstate, int number)
		case BOTLIB_AI_AVOID_GOAL_TIME:		//(int goalstate, int number)
		case BOTLIB_AI_MUTATE_GOAL_FUZZY_LOGIC: //(int goalstate, float range)
		case G_ATAN2:				//(double, double)
			return QMM_SYSCALL(cmd, vmarg(0), vmarg(1));
		case G_SEND_CONSOLE_COMMAND:		//(int exec_when, const char* text)
		case G_DROP_CLIENT:			//(int clientNum, const char* reason);
		case G_SEND_SERVER_COMMAND:		//(int clientNum, const char* fmt);
		case G_SET_CONFIGSTRING:		//(int num, const char* string);
		case G_SET_USERINFO:			//(int num, const char* buffer);
		case G_GET_USERCMD:			//(int clientNum, usercmd_t* cmd)
		case BOTLIB_UPDATENTITY:		//(int ent, void /* struct bot_updateentity_s* /* bue)
		case BOTLIB_USER_COMMAND:		//(int clientNum, usercmd_t* ucmd)
		case BOTLIB_AAS_ENTITY_INFO:		//(int entnum, void /* struct aas_entityinfo_s* /* info)
		case BOTLIB_AAS_AREA_INFO:		//(int areanum, void /* struct aas_areainfo_s* /* info)
		case BOTLIB_EA_SAY:			//(int client, char* str)
		case BOTLIB_EA_SAY_TEAM:		//(int client, char* str)
		case BOTLIB_EA_COMMAND:			//(int client, char* command)
		case BOTLIB_EA_VIEW:			//(int client, vec3_t viewangles)
		case BOTLIB_AI_NEXT_CONSOLE_MESSAGE:	//(int chatstate, void /* struct bot_consolemessage_s* /* cm)
		case BOTLIB_AI_NUM_INITIAL_CHATS:	//(int chatstate, char* type)
		case BOTLIB_AI_PUSH_GOAL:		//(int goalstate, void* goal)
		case BOTLIB_AI_GET_TOP_GOAL:		//(int goalstate, void /* struct bot_goal_s* /* goal)
		case BOTLIB_AI_GET_SECOND_GOAL:		//(int goalstate, void /* struct bot_goal_s* /* goal)
		case BOTLIB_AI_GET_NEXT_CAMP_SPOT_GOAL:	//(int num, void /* struct bot_goal_s* /* goal)
		case BOTLIB_AI_LOAD_ITEM_WEIGHTS:	//(int, char*)
		case BOTLIB_AI_SAVE_GOAL_FUZZY_LOGIC:	//(int, char*)
		case BOTLIB_AI_INIT_MOVE_STATE:		//(int handle, void* initmove)
		case BOTLIB_AI_CHOOSE_BEST_FIGHT_WEAPON:	//(int weaponstate, int* inventory)
		case BOTLIB_AI_LOAD_WEAPON_WEIGHTS:	//(int, char*)
		case BOTLIB_PC_READ_TOKEN:		//(int, void*)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1));
		case G_GET_SERVERINFO:			//(char* buffer, int bufferSize);
		case G_POINT_CONTENTS:			//(const vec3_t point, int passEntityNum);
		case G_ADJUST_AREA_PORTAL_STATE:	//(gentity_t* ent, qboolean open);
		case G_GET_ENTITY_TOKEN:		//(char* buffer, int bufferSize)
		case BOTLIB_AI_LOAD_CHARACTER:		//(char* charfile, float skill)
		case BOTLIB_AI_REPLACE_SYNONYMS:	//(char* string, unsigned long int context)
		case BOTLIB_AI_REACHABILITY_AREA:	//(vec3_t origin, int testground)
		case G_TESTPRINTINT:			//(char*, int)
		case G_TESTPRINTFLOAT:			//(char*, float)
			return QMM_SYSCALL(cmd, vmptr(0), vmarg(1));
		case G_ARGV:				//(int n, char* buffer, int bufferLength);
		case G_GET_CONFIGSTRING:		//(int num, char* buffer, int bufferSize);
		case G_GET_USERINFO:			//(int num, char* buffer, int bufferSize);
		case BOTLIB_GET_CONSOLE_MESSAGE:	//(int clientNum, char* message, int size)
		case BOTLIB_EA_MOVE:			//(int client, vec3_t dir, float speed)
		case BOTLIB_AI_GET_CHAT_MESSAGE:	//(int chatstate, char* buf, int size)
		case BOTLIB_AI_SET_CHAT_NAME:		//(int chatstate, char* name, int client)
		case BOTLIB_AI_GOAL_NAME:		//(int number, char* name, int size)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmarg(2));
		case G_CVAR_VARIABLE_STRING_BUFFER:	//(const char* var_name, char* buffer, int bufsize);
		case G_FS_FOPEN_FILE:			//(const char* qpath, fileHandle_t* file, fsMode_t mode);
		case BOTLIB_LIBVAR_GET:			//(char* var_name, char* value, int size)
		case BOTLIB_AI_STRING_CONTAINS:		//(char* str1, char* str2, int casesensitive)
		case BOTLIB_AI_FIND_MATCH:		//(char* str, void /* struct bot_match_s* /* match, unsigned long int context)
		case G_MEMCPY:				//(void* dest, const void* src, size_t count)
		case G_STRNCPY:				//(char* strDest, const char* strSource, size_t count)
			return QMM_SYSCALL(cmd, vmptr(0), vmptr(1), vmarg(2));
		case G_FS_READ:				//(void* buffer, int len, fileHandle_t f);
		case G_FS_WRITE:			//(const void* buffer, int len, fileHandle_t f);
		case G_MEMSET:				//(void* dest, int c, size_t count)
			return QMM_SYSCALL(cmd, vmptr(0), vmarg(1), vmarg(2));
		case G_ENTITY_CONTACT:			//(const vec3_t mins, const vec3_t maxs, const gentity_t* ent);
			return QMM_SYSCALL(cmd, vmptr(0), vmptr(1), vmptr(2));
		case G_ENTITY_CONTACTCAPSULE:		//(const vec3_t mins, const vec3_t maxs, const gentity_t* ent);
		case G_FS_SEEK:				//(fileHandle_t f, long offset, int origin)
		case BOTLIB_AI_ENTER_CHAT:		//(int chatstate, int client, int sendto)
		case BOTLIB_AI_SET_AVOID_GOAL_TIME:	//(int goalstate, int number, float avoidtime)
		case BOTLIB_AI_INTERBREED_GOAL_FUZZY_LOGIC:	//(int,int,int)
			return QMM_SYSCALL(cmd, vmarg(0), vmarg(1), vmarg(2));
		case G_DEBUG_POLYGON_CREATE:		//(int color, int numPoints, vec3_t* points)
		case BOTLIB_EA_GET_INPUT:		//(int client, float thinktime, void /* struct bot_input_s* /* input)
		case BOTLIB_AI_QUEUE_CONSOLE_MESSAGE:	//(int chatstate, int type, char* message)
		case BOTLIB_AI_GET_WEAPON_INFO:		//(int weaponstate, int weapon, void /* struct weaponinfo_s* /* weaponinfo)
			return QMM_SYSCALL(cmd, vmarg(0), vmarg(1), vmptr(2));
		case BOTLIB_AAS_PRESENCE_TYPE_BOUNDING_BOX:	//(int presencetype, vec3_t mins, vec3_t maxs)
		case BOTLIB_AAS_VECTOR_FOR_BSP_EPAIR_KEY:	//(int ent, char* key, vec3_t v)
		case BOTLIB_AAS_FLOAT_FOR_BSP_EPAIR_KEY:	//(int ent, char* key, float* value)
		case BOTLIB_AAS_INT_FOR_BSP_EPAIR_KEY:	//(int ent, char* key, int* value)
		case BOTLIB_AI_LOAD_CHAT_FILE:		//(int chatstate, char* chatfile, char* chatname)
		case BOTLIB_AI_GET_LEVEL_ITEM_GOAL:	//(int index, char* classname, void /* struct bot_goal_s* /* goal)
		case BOTLIB_PC_SOURCE_FILE_AND_LINE:	//(int handle, char* filename, int* line)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmptr(2));
		case G_CVAR_REGISTER:			//(vmCvar_t* vmCvar, const char* varName, const char* defaultValue, int flags);
		case G_ENTITIES_IN_BOX:			//(const vec3_t mins, const vec3_t maxs, gentity_t* *list, int maxcount);
		case G_FS_GETFILELIST:			//( const char* path, const char* extension, char* listbuf, int bufsize) {
		case BOTLIB_AAS_BBOX_AREAS:		//(vec3_t absmins, vec3_t absmaxs, int* areas, int maxareas)
			return QMM_SYSCALL(cmd, vmptr(0), vmptr(1), vmptr(2), vmarg(3));
		case BOTLIB_TEST:			//(int parm0, char* parm1, vec3_t parm2, vec3_t parm3)
		case BOTLIB_AI_ITEM_GOAL_IN_VIS_BUT_NOT_VISIBLE:	//(int viewer, vec3_t eye, vec3_t viewangles, void /* struct bot_goal_s* /* goal)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmptr(2), vmptr(3));
		case BOTLIB_AAS_VALUE_FOR_BSP_EPAIR_KEY:	//(int ent, char* key, char* value, int size)
		case BOTLIB_AI_CHOOSE_LTG_ITEM:		//(int goalstate, vec3_t origin, int* inventory, int travelflags)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmptr(2), vmarg(3));
		case BOTLIB_AAS_AREA_TRAVEL_TIME_TO_GOAL_AREA:	//(int areanum, vec3_t origin, int goalareanum, int travelflags)
		case BOTLIB_AI_ADD_AVOID_SPOT:		//(int movestate, vec3_t origin, float radius, int type)
		case BOTLIB_AI_MOVE_IN_DIRECTION:	//(int movestate, vec3_t dir, float speed, int type)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmarg(2), vmarg(3));
		case BOTLIB_AI_CHARACTERISTIC_BFLOAT:	//(int character, int index, float min, float max)
		case BOTLIB_AI_CHARACTERISTIC_BINTEGER:	//(int character, int index, int min, int max)
			return QMM_SYSCALL(cmd, vmarg(0), vmarg(1), vmarg(2), vmarg(3));
		case BOTLIB_AI_MATCH_VARIABLE:		//(void /* struct bot_match_s* /* match, int variable, char* buf, int size)
		case BOTLIB_AI_MOVE_TO_GOAL:		//(void /* struct bot_moveresult_s* /* result, int movestate, void /* struct bot_goal_s* /* goal, int travelflags)
			return QMM_SYSCALL(cmd, vmptr(0), vmarg(1), vmptr(2), vmarg(3));
		case BOTLIB_AI_CHARACTERISTIC_STRING:	//(int character, int index, char* buf, int size)
			return QMM_SYSCALL(cmd, vmarg(0), vmarg(1), vmptr(2), vmarg(3));
		case G_LOCATE_GAME_DATA:		//(gentity_t* gEnts, int numGEntities, int sizeofGEntity_t, playerState_t* clients, int sizeofGameClient);
			return QMM_SYSCALL(cmd, vmptr(0), vmarg(1), vmarg(2), vmptr(3), vmarg(4));
		case BOTLIB_AAS_TRACE_AREAS:		//(vec3_t start, vec3_t end, int* areas, vec3_t* points, int maxareas)
			return QMM_SYSCALL(cmd, vmptr(0), vmptr(1), vmptr(2), vmptr(3), vmarg(4));
		case BOTLIB_AI_MOVEMENT_VIEW_TARGET:	//(int movestate, void /* struct bot_goal_s* /* goal, int travelflags, float lookahead, vec3_t tvmarget)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmarg(2), vmarg(3), vmptr(4));
		case BOTLIB_AI_PREDICT_VISIBLE_POSITION:	//(vec3_t origin, int areanum, void /* struct bot_goal_s* /* goal, int travelflags, vec3_t tvmarget)
			return QMM_SYSCALL(cmd, vmptr(0), vmarg(1), vmptr(2), vmarg(3), vmptr(4));
		case BOTLIB_AI_GENETIC_PARENTS_AND_CHILD_SELECTION:	//(int numranks, float* ranks, int* parent1, int* parent2, int* child)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmptr(2), vmptr(3), vmptr(4));
		case G_TRACECAPSULE:			//(trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask);
			return QMM_SYSCALL(cmd, vmarg(0), vmarg(1), vmarg(2), vmarg(3), vmarg(4), vmarg(5));
		case BOTLIB_AI_CHOOSE_NBG_ITEM:		//(int goalstate, vec3_t origin, int* inventory, int travelflags, void /* struct bot_goal_s* /* ltg, float maxtime)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmptr(2), vmarg(3), vmptr(4), vmarg(5));
		case G_TRACE:				//(trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask);
			return QMM_SYSCALL(cmd, vmptr(0), vmptr(1), vmptr(2), vmptr(3), vmptr(4), vmarg(5), vmarg(6));
		case BOTLIB_AAS_ALTERNATIVE_ROUTE_GOAL:	//(vec3_t start, int startareanum, vec3_t goal, int goalareanum, int travelflags, void /*struct aas_altroutegoal_s*/* altroutegoals, int maxaltroutegoals, int type)
			return QMM_SYSCALL(cmd, vmptr(0), vmarg(1), vmptr(2), vmarg(3), vmarg(4), vmptr(5), vmarg(6), vmarg(7));
		case BOTLIB_AI_INITIAL_CHAT:		//(int chatstate, char* type, int mcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmarg(2), vmptr(3), vmptr(4), vmptr(5), vmptr(6), vmptr(7), vmptr(8), vmptr(9), vmptr(10));
		case BOTLIB_AAS_PREDICT_ROUTE:		//(void /*struct aas_predictroute_s*/* route, int areanum, vec3_t origin, int goalareanum, int travelflags, int maxareas, int maxtime, int stopevent, int stopcontents, int stoptfl, int stopareanum)
			return QMM_SYSCALL(cmd, vmptr(0), vmarg(1), vmptr(2), vmarg(3), vmarg(4), vmarg(5), vmarg(6), vmarg(7), vmarg(8), vmarg(9), vmarg(10), vmarg(11));
		case BOTLIB_AI_REPLY_CHAT:		//(int chatstate, char* message, int mcontext, int vcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7)
			return QMM_SYSCALL(cmd, vmarg(0), vmptr(1), vmarg(2), vmarg(3), vmptr(4), vmptr(5), vmptr(6), vmptr(7), vmptr(8), vmptr(9), vmptr(10), vmptr(11));
		case BOTLIB_AAS_PREDICT_CLIENT_MOVEMENT:	//(void /* struct aas_clientmove_s* /* move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize)
			return QMM_SYSCALL(cmd, vmptr(0), vmarg(1), vmptr(2), vmarg(3), vmarg(4), vmptr(5), vmptr(6), vmarg(7), vmarg(8), vmarg(9), vmarg(10), vmarg(11), vmarg(12));

		default:
			return 0;
	}
}
