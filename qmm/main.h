/*

QMM - Q3 MultiMod
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
    Code3Arena (http://www.planetquake.com/code3arena/)

*/

/* $Id: main.h,v 1.12 2004/08/30 00:45:08 cybermind Exp $ */

#ifndef __MAIN_H__
#define __MAIN_H__

#include "CEngine.h"
#include "CMod.h"
#include "CLinkList.h"
#include "CPlugin.h"

//stores info about the engine
extern CEngine Engine;
//stores info about the mod
extern CMod Mod;
//list of plugins
extern CLinkList<CPlugin> Plugins;

extern char qmm_modpath[MAX_QPATH], qmm_pluginfile[MAX_QPATH], qmm_execcfg[MAX_QPATH], fs_game[MAX_QPATH];
extern int qmm_debug;

extern FILE* g_df;

//initial entry point (gives us engine syscall ptr)
C_DLLEXPORT void dllEntry(eng_syscall_t);
//normal entry point (cmd, args), intercept game calls and route to plugins
C_DLLEXPORT int vmMain(int,int,int,int,int,int,int,int,int,int,int,int,int);
//this is given to the mod file's dllEntry() func, intercept engine calls and route to plugins
int QDECL syscall(int,...);

//call plugins' QMM_vmMain, mod's vmMain, plugins' QMM_vmMain_Post
int CallMain(int,int,int,int,int,int,int,int,int,int,int,int,int);
//call plugins' QMM_syscall, engine's syscall, plugins' QMM_syscall_Post
int CallSyscall(int,int,int,int,int,int,int,int,int,int,int,int,int);

//load plugin with iscmd flag
bool LoadPlugin(const char*, int);
//unload plugin (based on id) with iscmd flag
bool UnloadPlugin(int, int);
//pause plugin (based on id)
bool PausePlugin(int);
//unpause plugin (based on id)
void UnpausePlugin(int);
//show info on plugin (based on id)
void InfoPlugin(int);

//macros for easier function calling
//engine's syscall
#define ENGFUNC			(Engine.get_syscall())
//mod's vmMain
#define MODFUNC			(Mod.get_vmMain())
//plugin's vmMain
#define PLUGINMODFUNC		(p->data->get_QMM_vmMain())
//plugin's vmMain post
#define PLUGINMODFUNC_POST	(p->data->get_QMM_vmMain_Post())
//plugin's syscall
#define PLUGINENGFUNC		(p->data->get_QMM_syscall())
//plugin's syscall post
#define PLUGINENGFUNC_POST	(p->data->get_QMM_syscall_Post())

#endif //__MAIN_H__
