/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __CMODMGR_H__
#define __CMODMGR_H__

#include "osdef.h"
#include "CMod.h"
#include "game_api.h"
#include "qmmapi.h"

class CModMgr {
	public:
		CModMgr(eng_syscall_t qmm_syscall, int msgarray[], msgname_t msgnames);
		~CModMgr();

		int LoadMod();
		void UnloadMod();

		eng_syscall_t QMM_SysCall();

		CMod* Mod();

		int GetMsg(mod_msg_t msg);
		const char* GetMsgName(int msg);

		static CModMgr* GetInstance(eng_syscall_t qmm_syscall, int msgarray[], msgname_t msgnames);

	private:
		CMod* mod;
		int* msgarray;
		msgname_t msgnames;
		eng_syscall_t qmm_syscall;
		CMod* newmod(const char*);

		static CModMgr* instance;
};

#endif //__CMODMGR_H__
