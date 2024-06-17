/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __CENGINEMGR_H__
#define __CENGINEMGR_H__

#include "qmmapi.h"
#include "game_api.h"

class CEngineMgr {
	public:
		CEngineMgr(eng_syscall_t syscall, const char* dllname, const char* qvmname, const char* basedir, const char* gamename_long, const char* gamename_short, int msgarray[], msgname_t msgnames, vmsyscall_t vmsyscall, int autodetected, const char* homepath);
		~CEngineMgr();
		
		eng_syscall_t SysCall();
		const char* GetDLLName();
		const char* GetQVMName();
		const char* GetModDir();
		const char* GetBaseDir();
		const char* GetGameName_Long();
		const char* GetGameName_Short();
		int GetMsg(eng_msg_t msg);
		const char* GetMsgName(int msg);
		vmsyscall_t VMSysCall();
		int AutoDetected();
		const char* GetHomepath();

		static CEngineMgr* GetInstance(eng_syscall_t syscall, const char* dllname, const char* qvmname, const char* basedir, const char* gamename_long, const char* gamename_short, int msgarray[], msgname_t msgnames, vmsyscall_t vmsyscall, int autodetected, const char* homepath);

	private:
		eng_syscall_t pfnsyscall;
		const char* dllname;
		const char* qvmname;
		int* msgarray;
		msgname_t msgnames;
		const char* basedir;
		const char* gamename_long;
		const char* gamename_short;
		vmsyscall_t vmsyscall;
		int autodetected;
		const char* homepath;

		static CEngineMgr* instance;
};

#endif //__CENGINEMGR_H__
