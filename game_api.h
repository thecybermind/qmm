/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __GAME_API_H__
#define __GAME_API_H__

typedef int (*vmsyscall_t)(unsigned char*, int, int*);
typedef const char* (*msgname_t)(int);

//some information for each game engine supported by QMM
typedef struct gameinfo_s {
	const char* dllname;		//default dll mod filename
	const char* qvmname;		//default qvm mod filename (NULL = qmm_<dllname>)
	const char* basedir;		//default moddir name
	const char* gamename_long;	//long, descriptive, game name
	const char* gamename_short;	//short initials for game
	int* eng_msgs;			//array of engine messages used by QMM
	msgname_t eng_msg_names;	//pointer to a function that returns a string for a given engine message
	int* mod_msgs;			//array of mod messages used by QMM
	msgname_t mod_msg_names;	//pointer to a function that returns a string for a given mod message
	vmsyscall_t vmsyscall;		//pointer to a function that handles mod->engine calls from a VM (NULL = not required)
	const char* althomepath;	//alternate homepath for linux only (win = NULL)
} gameinfo_t;

extern gameinfo_t g_GameInfo[];

//a list of all the mod messages used by QMM
typedef enum mod_msg_e {
	QMM_GAME_INIT,
	QMM_GAME_SHUTDOWN,
	QMM_GAME_CONSOLE_COMMAND,
	QMM_GAME_CLIENT_CONNECT,
	QMM_GAME_CLIENT_COMMAND,

	QMM_GAME_MSG_MAX
} mod_msg_t;

//a list of all the engine messages used by QMM
typedef enum eng_msg_e {
	QMM_G_PRINT,
	QMM_G_ERROR,
	QMM_G_ARGV,
	QMM_G_ARGC,
	QMM_G_SEND_CONSOLE_COMMAND,
	QMM_G_SEND_SERVER_COMMAND,

	QMM_G_CVAR_REGISTER,
	QMM_G_CVAR_SET,
	QMM_G_CVAR_VARIABLE_STRING_BUFFER,
	QMM_G_CVAR_VARIABLE_INTEGER_VALUE,

	QMM_G_FS_FOPEN_FILE,
	QMM_G_FS_READ,
	QMM_G_FS_WRITE,
	QMM_G_FS_FCLOSE_FILE,
	
	QMM_EXEC_APPEND,
	QMM_FS_READ,
	QMM_FS_APPEND,
	QMM_FS_APPEND_SYNC,

	QMM_CVAR_SERVERINFO,
	QMM_CVAR_ROM,
	QMM_CVAR_ARCHIVE,

	QMM_G_MSG_MAX
} eng_msg_t;

//macros to make game support a bit easier to do
//these macros go in game_api.cpp

//generate externs for the msg arrays/funcs
#define GEN_EXTS(game)	extern int game##_eng_msgs[], game##_mod_msgs[]; \
			const char* game##_eng_msg_names(int); \
			const char* game##_mod_msg_names(int);
//generate extern for the vmsyscall function
#define GEN_VMEXT(game)	int game##_vmsyscall(unsigned char*, int, int*)
//generate the struct info
#define GEN_INFO(game)	#game, game##_eng_msgs, game##_eng_msg_names, game##_mod_msgs, game##_mod_msg_names

//macro to easily output message values to match the lists above
//this macro goes in game_*.cpp
#define GEN_MSGS(game) \
	int game##_eng_msgs[] = { \
		G_PRINT, G_ERROR, G_ARGV, G_ARGC, G_SEND_CONSOLE_COMMAND, G_SEND_SERVER_COMMAND, \
		G_CVAR_REGISTER, G_CVAR_SET, G_CVAR_VARIABLE_STRING_BUFFER, G_CVAR_VARIABLE_INTEGER_VALUE, \
		G_FS_FOPEN_FILE, G_FS_READ, G_FS_WRITE, G_FS_FCLOSE_FILE, \
		EXEC_APPEND, FS_READ, FS_APPEND, FS_APPEND_SYNC, \
		CVAR_SERVERINFO, CVAR_ROM, CVAR_ARCHIVE \
	}; \
	int game##_mod_msgs[] = { \
		GAME_INIT, GAME_SHUTDOWN, GAME_CONSOLE_COMMAND, GAME_CLIENT_CONNECT, GAME_CLIENT_COMMAND \
	}

//this gets an argument value
#define vmarg(x)	(args[(x)])
//this adds the base VM address to a given value
#define vmadd(x)	((x) ? (x) + (int)membase : NULL)
//this subtracts the base VM address from the given value
#define vmsub(x)	((x) ? (x) - (int)membase : NULL)
//this adds the base VM address to an argument value
#define vmptr(x)	(vmadd(vmarg(x)))

#if 0	//seriously who uses powerpc

	/* powerpc linux changes added as a result of the q3 source release
	 * ---
	 * following the code leads me to believe that 'id' is magical
	 * because i don't see how this could work at all
	 *
	 * they do not add the base VM address to pointer parameters, they
	 * are left as-is. i can understand what they did for DLL syscalls
	 * (in qcommon/vm.c:VM_DllSyscall), but for QVM syscalls, it just
	 * doesn't make sense at all
	 */
	#if !defined(WIN32) && defined(__powerpc__)
	 #define vmadd(x)	(x)
	 #define vmsub(x)	(x)
	 #define vmptr(x)	((void*)vmarg(x))
	#endif
#endif

#endif //__GAME_API_H__
