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

/* $Id: CEngineMgr.h,v 1.7 2005/10/12 22:57:57 cybermind Exp $ */

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
