/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __QMM_H__
#define __QMM_H__

#include "CEngineMgr.h"
#include "CModMgr.h"
#include "CPluginMgr.h"
#include "CConfigMgr.h"
#include "qmmapi.h"

pluginfuncs_t* get_pluginfuncs();

typedef unsigned char byte;

extern CEngineMgr* g_EngineMgr;
extern CModMgr* g_ModMgr;
extern CPluginMgr* g_PluginMgr;
extern CConfigMgr* g_ConfigMgr;

#define ENG_MSG		g_EngineMgr->GetMsg
#define ENG_MSGNAME	g_EngineMgr->GetMsgName

#define MOD_MSG		g_ModMgr->GetMsg
#define MOD_MSGNAME	g_ModMgr->GetMsgName

#define ENG_SYSCALL	(g_EngineMgr->SysCall())
#define MOD_VMMAIN	g_ModMgr->Mod()->vmMain
#define	QMM_SYSCALL	(g_ModMgr->QMM_SysCall())

#define QMM_FAIL_G_ERROR	1
#define QMM_FAIL_GAME_SHUTDOWN	1

C_DLLEXPORT void dllEntry(eng_syscall_t);
C_DLLEXPORT int vmMain(int, int, int, int, int, int, int, int, int, int, int, int, int);
int QMM_syscall(int, ...);

#endif //__QMM_H__
