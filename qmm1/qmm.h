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

/* $Id: qmm.h,v 1.5 2006/01/29 22:45:37 cybermind Exp $ */

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
