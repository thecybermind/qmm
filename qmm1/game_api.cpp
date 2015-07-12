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

/* $Id: game_api.cpp,v 1.11 2006/01/29 22:45:37 cybermind Exp $ */

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

//	mod filename		qvm mod filename	default moddir	full gamename					msgs/short name		qvm handler	homepath
#ifdef WIN32
	{ "qagamex86.dll",	"vm/qagame.qvm",	"baseq3",	"Quake 3 Arena",				GEN_INFO(Q3A),		&Q3A_vmsyscall,	NULL },
	{ "jk2mpgamex86.dll",	"vm/jk2mpgame.qvm",	"base",		"Jedi Knight 2: Jedi Outcast",			GEN_INFO(JK2),		&JK2_vmsyscall,	NULL },
	{ "jampgamex86.dll",	NULL,			"base",		"Jedi Knight: Jedi Academy",			GEN_INFO(JKA),		NULL,		NULL },
	{ "qagame_mp_x86.dll",	NULL,			"Main",		"Return to Castle Wolfenstein (Multiplayer)",	GEN_INFO(RTCWMP),	NULL,		NULL },
	{ "qagamex86.dll",	NULL,			".",		"Return to Castle Wolfenstein (Singleplayer)",	GEN_INFO(RTCWSP),	NULL,		NULL },
	{ "qagame_mp_x86.dll",	NULL,			"etmain",	"Return to Castle Wolfenstein: Enemy Territory",GEN_INFO(RTCWET),	NULL,		NULL },
#else
	{ "qagamei386.so",	"vm/qagame.qvm",	"baseq3",	"Quake 3 Arena",				GEN_INFO(Q3A),		&Q3A_vmsyscall,	"/.q3a/" },
	{ "jk2mpgamei386.so",	"vm/jk2mpgame.qvm",	"base",		"Jedi Knight 2: Jedi Outcast",			GEN_INFO(JK2),		&JK2_vmsyscall,	"/.jkii/" },
	{ "jampgamei386.so",	NULL,			"base",		"Jedi Knight: Jedi Academy",			GEN_INFO(JKA),		NULL,		"/.ja/" },
	{ "qagame.mp.i386.so",	NULL,			"main",		"Return to Castle Wolfenstein (Multiplayer)",	GEN_INFO(RTCWMP),	NULL,		"/.wolf/" },
	{ "qagamei386.so",	NULL,			"main",		"Return to Castle Wolfenstein (Singleplayer)",	GEN_INFO(RTCWSP),	NULL,		"/.wolf/" },
	{ "qagame.mp.i386.so",	NULL,			"etmain",	"Return to Castle Wolfenstein: Enemy Territory",GEN_INFO(RTCWET),	NULL,		"/.etwolf/" },
#endif

	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};
