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

/* $Id: util.cpp,v 1.8 2004/08/30 00:45:36 cybermind Exp $ */

#include "version.h"
#include <stdio.h>
#include <stdarg.h>
#include <q_shared.h>
#include <g_public.h>
#include "util.h"

//copied from vm.h just so I don't have to include the damn thing
typedef enum {
	OP_UNDEF,
	OP_NOP,
	OP_BREAK,
	OP_ENTER,
	OP_LEAVE,
	OP_CALL,
	OP_PUSH,
	OP_POP,
	OP_CONST,
	OP_LOCAL,
	OP_JUMP,
	OP_EQ,
	OP_NE,
	OP_LTI,
	OP_LEI,
	OP_GTI,
	OP_GEI,
	OP_LTU,
	OP_LEU,
	OP_GTU,
	OP_GEU,
	OP_EQF,
	OP_NEF,
	OP_LTF,
	OP_LEF,
	OP_GTF,
	OP_GEF,
	OP_LOAD1,
	OP_LOAD2,
	OP_LOAD4,
	OP_STORE1,
	OP_STORE2,
	OP_STORE4,
	OP_ARG,
	OP_BLOCK_COPY,
	OP_SEX8,
	OP_SEX16,
	OP_NEGI,
	OP_ADD,
	OP_SUB,
	OP_DIVI,
	OP_DIVU,
	OP_MODI,
	OP_MODU,
	OP_MULI,
	OP_MULU,
	OP_BAND,
	OP_BOR,
	OP_BXOR,
	OP_BCOM,
	OP_LSH,
	OP_RSHI,
	OP_RSHU,
	OP_NEGF,
	OP_ADDF,
	OP_SUBF,
	OP_DIVF,
	OP_MULF,
	OP_CVIF,
	OP_CVFI
} vmOps_t;

//from sdk/game/q_shared.c
int byteswap(int i) {
	byte b1,b2,b3,b4;

	b1 = i&255;
	b2 = (i>>8)&255;
	b3 = (i>>16)&255;
	b4 = (i>>24)&255;

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}
short byteswap(short s) {
	byte b1,b2;

	b1 = s&255;
	b2 = (s>>8)&255;

	return ((int)b1<<8) + b2;
}

#ifdef WIN32
 #define vsnprintf	_vsnprintf
#endif

//Format a string
char* vaf(char* format, ...) {
	va_list	argptr;
	static char str[1024];

	va_start(argptr, format);
	vsnprintf(str, 1024, format, argptr);
	va_end(argptr);

	return str;
}

//return a string name for the VM opcode
const char* opcodename(int op) {
	switch(op) {
	case OP_UNDEF:
		return "OP_UNDEF";
	case OP_NOP:
		return "OP_NOP";
	case OP_BREAK:
		return "OP_BREAK";
	case OP_ENTER:
		return "OP_ENTER";
	case OP_LEAVE:
		return "OP_LEAVE";
	case OP_CALL:
		return "OP_CALL";
	case OP_PUSH:
		return "OP_PUSH";
	case OP_POP:
		return "OP_POP";
	case OP_CONST:
		return "OP_CONST";
	case OP_LOCAL:
		return "OP_LOCAL";
	case OP_JUMP:
		return "OP_JUMP";
	case OP_EQ:
		return "OP_EQ";
	case OP_NE:
		return "OP_NE";
	case OP_LTI:
		return "OP_LTI";
	case OP_LEI:
		return "OP_LEI";
	case OP_GTI:
		return "OP_GTI";
	case OP_GEI:
		return "OP_GEI";
	case OP_LTU:
		return "OP_LTU";
	case OP_LEU:
		return "OP_LEU";
	case OP_GTU:
		return "OP_GTU";
	case OP_GEU:
		return "OP_GEU";
	case OP_EQF:
		return "OP_EQF";
	case OP_NEF:
		return "OP_NEF";
	case OP_LTF:
		return "OP_LTF";
	case OP_LEF:
		return "OP_LEF";
	case OP_GTF:
		return "OP_GTF";
	case OP_GEF:
		return "OP_GEF";
	case OP_LOAD1:
		return "OP_LOAD1";
	case OP_LOAD2:
		return "OP_LOAD2";
	case OP_LOAD4:
		return "OP_LOAD4";
	case OP_STORE1:
		return "OP_STORE1";
	case OP_STORE2:
		return "OP_STORE2";
	case OP_STORE4:
		return "OP_STORE4";
	case OP_ARG:
		return "OP_ARG";
	case OP_BLOCK_COPY:
		return "OP_BLOCK_COPY";
	case OP_SEX8:
		return "OP_SEX8";
	case OP_SEX16:
		return "OP_SEX16";
	case OP_NEGI:
		return "OP_NEGI";
	case OP_ADD:
		return "OP_ADD";
	case OP_SUB:
		return "OP_SUB";
	case OP_DIVI:
		return "OP_DIVI";
	case OP_DIVU:
		return "OP_DIVU";
	case OP_MODI:
		return "OP_MODI";
	case OP_MODU:
		return "OP_MODU";
	case OP_MULI:
		return "OP_MULI";
	case OP_MULU:
		return "OP_MULU";
	case OP_BAND:
		return "OP_BAND";
	case OP_BOR:
		return "OP_BOR";
	case OP_BXOR:
		return "OP_BXOR";
	case OP_BCOM:
		return "OP_BCOM";
	case OP_LSH:
		return "OP_LSH";
	case OP_RSHI:
		return "OP_RSHI";
	case OP_RSHU:
		return "OP_RSHU";
	case OP_NEGF:
		return "OP_NEGF";
	case OP_ADDF:
		return "OP_ADDF";
	case OP_SUBF:
		return "OP_SUBF";
	case OP_DIVF:
		return "OP_DIVF";
	case OP_MULF:
		return "OP_MULF";
	case OP_CVIF:
		return "OP_CVIF";
	case OP_CVFI:
		return "OP_CVFI";
	default:
		return "unknown";
	}

	return "unknown";
}

//returns number of params
//0 = param
//1 = opStack[0]
//2 = opStack[0] & opStack[1]
//3 = param & opStack[0] & opStack[1]
int opparms(int op) {
	switch(op) {
	case OP_UNDEF:
	case OP_NOP:
	case OP_BREAK:
	case OP_ENTER:
	case OP_LEAVE: return 0;
	case OP_CALL:
	case OP_PUSH:
	case OP_POP:
	case OP_CONST:
	case OP_LOCAL:
	case OP_LOAD1:
	case OP_LOAD2:
	case OP_LOAD4:
	case OP_ARG:
	case OP_CVIF:
	case OP_CVFI: return 1;
	case OP_JUMP:
	case OP_EQ:
	case OP_NE:
	case OP_LTI:
	case OP_LEI:
	case OP_GTI:
	case OP_GEI:
	case OP_LTU:
	case OP_LEU:
	case OP_GTU:
	case OP_GEU:
	case OP_EQF:
	case OP_NEF:
	case OP_LTF:
	case OP_LEF:
	case OP_GTF:
	case OP_GEF:
	case OP_STORE1:
	case OP_STORE2:
	case OP_STORE4:
	case OP_SEX8:
	case OP_SEX16:
	case OP_NEGI:
	case OP_ADD:
	case OP_SUB:
	case OP_DIVI:
	case OP_DIVU:
	case OP_MODI:
	case OP_MODU:
	case OP_MULI:
	case OP_MULU:
	case OP_BAND:
	case OP_BOR:
	case OP_BXOR:
	case OP_BCOM:
	case OP_LSH:
	case OP_RSHI:
	case OP_RSHU:
	case OP_NEGF:
	case OP_ADDF:
	case OP_SUBF:
	case OP_DIVF:
	case OP_MULF: return 2;
	case OP_BLOCK_COPY: return 3;
	default: return 0;
	}
	return 0;
}

//return a string name for the vmMain param
#if defined(GAME_Q3A)
const char* modfuncname(int cmd) {
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

	return "unknown";
}
#elif defined(GAME_JK2)
const char* modfuncname(int cmd) {
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
	case GAME_ROFF_NOTETRACK_CALLBACK:
		return "GAME_ROFF_NOTETRACK_CALLBACK";
	default:
		return "unknown";
	}

	return "unknown";
}
#endif

//return a string name for the syscall param
#if defined(GAME_Q3A)
const char* engfuncname(int cmd) {
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
	case QMMVM_G_MALLOC:
		return "QMMVM_G_MALLOC";
	case QMMVM_G_REALLOC:
		return "QMMVM_G_REALLOC";
	case QMMVM_G_FREE:
		return "QMMVM_G_FREE";
	default:
		return "unknown";
	}

	return "unknown";
}
#elif defined(GAME_JK2)
const char* engfuncname(int cmd) {
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
	case SP_REGISTER_SERVER_CMD:
		return "SP_REGISTER_SERVER_CMD";
	case SP_GETSTRINGTEXTSTRING:
		return "SP_GETSTRINGTEXTSTRING";
	case G_ROFF_CLEAN:
		return "G_ROFF_CLEAN";
	case G_ROFF_UPDATE_ENTITIES:
		return "G_ROFF_UPDATE_ENTITIES";
	case G_ROFF_CACHE:
		return "G_ROFF_CACHE";
	case G_ROFF_PLAY:
		return "G_ROFF_PLAY";
	case G_ROFF_PURGE_ENT:
		return "G_ROFF_PURGE_ENT";
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
	case G_MATRIXMULTIPLY:
		return "G_MATRIXMULTIPLY";
	case G_ANGLEVECTORS:
		return "G_ANGLEVECTORS";
	case G_PERPENDICULARVECTOR:
		return "G_PERPENDICULARVECTOR";
	case G_FLOOR:
		return "G_FLOOR";
	case G_CEIL:
		return "G_CEIL";
	case G_TESTPRINTINT:
		return "G_TESTPRINTINT";
	case G_TESTPRINTFLOAT:
		return "G_TESTPRINTFLOAT";
	case G_ACOS:
		return "G_ACOS";
	case G_ASIN:
		return "G_ASIN";
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
	case BOTLIB_AAS_ENABLE_ROUTING_AREA:
		return "BOTLIB_AAS_ENABLE_ROUTING_AREA";
	case BOTLIB_AAS_BBOX_AREAS:
		return "BOTLIB_AAS_BBOX_AREAS";
	case BOTLIB_AAS_AREA_INFO:
		return "BOTLIB_AAS_AREA_INFO";
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
	case BOTLIB_AAS_TRACE_AREAS:
		return "BOTLIB_AAS_TRACE_AREAS";
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
	case BOTLIB_EA_ALT_ATTACK:
		return "BOTLIB_EA_ALT_ATTACK";
	case BOTLIB_EA_FORCEPOWER:
		return "BOTLIB_EA_FORCEPOWER";
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
	case BOTLIB_AI_REPLY_CHAT:
		return "BOTLIB_AI_REPLY_CHAT";
	case BOTLIB_AI_CHAT_LENGTH:
		return "BOTLIB_AI_CHAT_LENGTH";
	case BOTLIB_AI_ENTER_CHAT:
		return "BOTLIB_AI_ENTER_CHAT";
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
	case BOTLIB_AI_AVOID_GOAL_TIME:
		return "BOTLIB_AI_AVOID_GOAL_TIME";
	case BOTLIB_AI_INIT_LEVEL_ITEMS:
		return "BOTLIB_AI_INIT_LEVEL_ITEMS";
	case BOTLIB_AI_UPDATE_ENTITY_ITEMS:
		return "BOTLIB_AI_UPDATE_ENTITY_ITEMS";
	case BOTLIB_AI_LOAD_ITEM_WEIGHTS:
		return "BOTLIB_AI_LOAD_ITEM_WEIGHTS";
	case BOTLIB_AI_FREE_ITEM_WEIGHTS:
		return "BOTLIB_AI_FREE_ITEM_WEIGHTS";
	case BOTLIB_AI_SAVE_GOAL_FUZZY_LOGIC:
		return "BOTLIB_AI_SAVE_GOAL_FUZZY_LOGIC";
	case BOTLIB_AI_ALLOC_GOAL_STATE:
		return "BOTLIB_AI_ALLOC_GOAL_STATE";
	case BOTLIB_AI_FREE_GOAL_STATE:
		return "BOTLIB_AI_FREE_GOAL_STATE";
	case BOTLIB_AI_RESET_MOVE_STATE:
		return "BOTLIB_AI_RESET_MOVE_STATE";
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
	case BOTLIB_AI_INTERBREED_GOAL_FUZZY_LOGIC:
		return "BOTLIB_AI_INTERBREED_GOAL_FUZZY_LOGIC";
	case BOTLIB_AI_MUTATE_GOAL_FUZZY_LOGIC:
		return "BOTLIB_AI_MUTATE_GOAL_FUZZY_LOGIC";
	case BOTLIB_AI_GET_NEXT_CAMP_SPOT_GOAL:
		return "BOTLIB_AI_GET_NEXT_CAMP_SPOT_GOAL";
	case BOTLIB_AI_GET_MAP_LOCATION_GOAL:
		return "BOTLIB_AI_GET_MAP_LOCATION_GOAL";
	case BOTLIB_AI_NUM_INITIAL_CHATS:
		return "BOTLIB_AI_NUM_INITIAL_CHATS";
	case BOTLIB_AI_GET_CHAT_MESSAGE:
		return "BOTLIB_AI_GET_CHAT_MESSAGE";
	case BOTLIB_AI_REMOVE_FROM_AVOID_GOALS:
		return "BOTLIB_AI_REMOVE_FROM_AVOID_GOALS";
	case BOTLIB_AI_PREDICT_VISIBLE_POSITION:
		return "BOTLIB_AI_PREDICT_VISIBLE_POSITION";
	case BOTLIB_AI_SET_AVOID_GOAL_TIME:
		return "BOTLIB_AI_SET_AVOID_GOAL_TIME";
	case BOTLIB_AI_ADD_AVOID_SPOT:
		return "BOTLIB_AI_ADD_AVOID_SPOT";
	case BOTLIB_AAS_ALTERNATIVE_ROUTE_GOAL:
		return "BOTLIB_AAS_ALTERNATIVE_ROUTE_GOAL";
	case BOTLIB_AAS_PREDICT_ROUTE:
		return "BOTLIB_AAS_PREDICT_ROUTE";
	case BOTLIB_AAS_POINT_REACHABILITY_AREA_INDEX:
		return "BOTLIB_AAS_POINT_REACHABILITY_AREA_INDEX";
	case BOTLIB_PC_LOAD_SOURCE:
		return "BOTLIB_PC_LOAD_SOURCE";
	case BOTLIB_PC_FREE_SOURCE:
		return "BOTLIB_PC_FREE_SOURCE";
	case BOTLIB_PC_READ_TOKEN:
		return "BOTLIB_PC_READ_TOKEN";
	case BOTLIB_PC_SOURCE_FILE_AND_LINE:
		return "BOTLIB_PC_SOURCE_FILE_AND_LINE";
	case G_G2_LISTBONES:
		return "G_G2_LISTBONES";
	case G_G2_LISTSURFACES:
		return "G_G2_LISTSURFACES";
	case G_G2_HAVEWEGHOULMODELS:
		return "G_G2_HAVEWEGHOULMODELS";
	case G_G2_SETMODELS:
		return "G_G2_SETMODELS";
	case G_G2_GETBOLT:
		return "G_G2_GETBOLT";
	case G_G2_GETBOLT_NOREC:
		return "G_G2_GETBOLT_NOREC";
	case G_G2_GETBOLT_NOREC_NOROT:
		return "G_G2_GETBOLT_NOREC_NOROT";
	case G_G2_INITGHOUL2MODEL:
		return "G_G2_INITGHOUL2MODEL";
	case G_G2_ADDBOLT:
		return "G_G2_ADDBOLT";
	case G_G2_SETBOLTINFO:
		return "G_G2_SETBOLTINFO";
	case G_G2_ANGLEOVERRIDE:
		return "G_G2_ANGLEOVERRIDE";
	case G_G2_PLAYANIM:
		return "G_G2_PLAYANIM";
	case G_G2_GETGLANAME:
		return "G_G2_GETGLANAME";
	case G_G2_COPYGHOUL2INSTANCE:
		return "G_G2_COPYGHOUL2INSTANCE";
	case G_G2_COPYSPECIFICGHOUL2MODEL:
		return "G_G2_COPYSPECIFICGHOUL2MODEL";
	case G_G2_DUPLICATEGHOUL2INSTANCE:
		return "G_G2_DUPLICATEGHOUL2INSTANCE";
	case G_G2_HASGHOUL2MODELONINDEX:
		return "G_G2_HASGHOUL2MODELONINDEX";
	case G_G2_REMOVEGHOUL2MODEL:
		return "G_G2_REMOVEGHOUL2MODEL";
	case G_G2_CLEANMODELS:
		return "G_G2_CLEANMODELS";
	case G_G2_COLLISIONDETECT:
		return "G_G2_COLLISIONDETECT";
	case QMMVM_G_MALLOC:
		return "QMMVM_G_MALLOC";
	case QMMVM_G_REALLOC:
		return "QMMVM_G_REALLOC";
	case QMMVM_G_FREE:
		return "QMMVM_G_FREE";
	default:
		return "unknown";
	}

	return "unknown";
}
#endif
