/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#include <stddef.h>
#include "game_api.h"

//add your game file's msg arrays here
GEN_EXTS(Q3A);
GEN_VMEXT(Q3A);
GEN_EXTS(RTCWMP);
GEN_EXTS(JK2);
GEN_VMEXT(JK2);
GEN_EXTS(JKA);
GEN_EXTS(RTCWSP);
GEN_EXTS(RTCWET);

//add your game's info data here
gameinfo_t g_GameInfo[] = {

//	mod filename			qvm mod filename	default moddir	full gamename									msgs/short name		qvm handler		homepath
#ifdef WIN32
	{ "qagamex86.dll",		"vm/qagame.qvm",	"baseq3",		"Quake 3 Arena",								GEN_INFO(Q3A),		&Q3A_vmsyscall,	NULL },
	{ "jk2mpgamex86.dll",	"vm/jk2mpgame.qvm",	"base",			"Jedi Knight 2: Jedi Outcast",					GEN_INFO(JK2),		&JK2_vmsyscall,	NULL },
	{ "jampgamex86.dll",	NULL,				"base",			"Jedi Knight: Jedi Academy",					GEN_INFO(JKA),		NULL,			NULL },
	{ "qagame_mp_x86.dll",	NULL,				"Main",			"Return to Castle Wolfenstein (Multiplayer)",	GEN_INFO(RTCWMP),	NULL,			NULL },
	{ "qagamex86.dll",		NULL,				".",			"Return to Castle Wolfenstein (Singleplayer)",	GEN_INFO(RTCWSP),	NULL,			NULL },
	{ "qagame_mp_x86.dll",	NULL,				"etmain",		"Return to Castle Wolfenstein: Enemy Territory",GEN_INFO(RTCWET),	NULL,			NULL },
#else
	{ "qagamei386.so",	"vm/qagame.qvm",		"baseq3",		"Quake 3 Arena",								GEN_INFO(Q3A),		&Q3A_vmsyscall,	"/.q3a/" },
	{ "jk2mpgamei386.so",	"vm/jk2mpgame.qvm",	"base",			"Jedi Knight 2: Jedi Outcast",					GEN_INFO(JK2),		&JK2_vmsyscall,	"/.jkii/" },
	{ "jampgamei386.so",	NULL,				"base",			"Jedi Knight: Jedi Academy",					GEN_INFO(JKA),		NULL,			"/.ja/" },
	{ "qagame.mp.i386.so",	NULL,				"main",			"Return to Castle Wolfenstein (Multiplayer)",	GEN_INFO(RTCWMP),	NULL,			"/.wolf/" },
	{ "qagamei386.so",	NULL,					"main",			"Return to Castle Wolfenstein (Singleplayer)",	GEN_INFO(RTCWSP),	NULL,			"/.wolf/" },
	{ "qagame.mp.i386.so",	NULL,				"etmain",		"Return to Castle Wolfenstein: Enemy Territory",GEN_INFO(RTCWET),	NULL,			"/.etwolf/" },
#endif

	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};
