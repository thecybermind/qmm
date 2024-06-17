/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#include <stddef.h>		//for NULL
#include <string.h>		//for strncpy
#include "CEngineMgr.h"
#include "game_api.h"
#include "qmmapi.h"
#include "osdef.h"

CEngineMgr::CEngineMgr(eng_syscall_t syscall, const char* dllname, const char* qvmname, const char* basedir, const char* gamename_long, const char* gamename_short, int msgarray[], msgname_t msgnames, vmsyscall_t vmsyscall, int autodetected, const char* homepath) {
	this->pfnsyscall = syscall;
	this->dllname = dllname;
	this->qvmname = qvmname;
	this->basedir = basedir;
	this->gamename_long = gamename_long;
	this->gamename_short = gamename_short;
	this->msgarray = msgarray;
	this->msgnames = msgnames;
	this->vmsyscall = vmsyscall;
	this->autodetected = autodetected;
	this->homepath = homepath;
}

CEngineMgr::~CEngineMgr() {
}

eng_syscall_t CEngineMgr::SysCall() {
	return this->pfnsyscall;
}

const char* CEngineMgr::GetDLLName() {
	return this->dllname;
}

const char* CEngineMgr::GetQVMName() {
	return this->qvmname;
}

const char* CEngineMgr::GetModDir() {
	static char dir[MAX_PATH] = "";
	if (dir[0]) return dir;

	int x = this->msgarray[QMM_G_CVAR_VARIABLE_STRING_BUFFER];
	this->pfnsyscall(x, "fs_game", dir, sizeof(dir));
	if (!dir[0])
		strncpy(dir, this->basedir, sizeof(dir));

	return dir;
}

const char* CEngineMgr::GetBaseDir() {
	return this->basedir;
}

const char* CEngineMgr::GetGameName_Long() {
	return this->gamename_long;
}

const char* CEngineMgr::GetGameName_Short() {
	return this->gamename_short;
}

int CEngineMgr::GetMsg(eng_msg_t msg) {
	return this->msgarray[msg];
}

const char* CEngineMgr::GetMsgName(int msg) {
	return this->msgnames(msg);
}

vmsyscall_t CEngineMgr::VMSysCall() {
	return this->vmsyscall;
}

int CEngineMgr::AutoDetected() {
	return this->autodetected;
}

const char* CEngineMgr::GetHomepath() {
	return this->homepath;
}

CEngineMgr* CEngineMgr::GetInstance(eng_syscall_t syscall, const char* dllname, const char* qvmname, const char* basedir, const char* gamename_long, const char* gamename_short, int msgarray[], msgname_t msgnames, vmsyscall_t vmsyscall, int autodetected, const char* homepath) {
	if (!CEngineMgr::instance)
		CEngineMgr::instance = new CEngineMgr(syscall, dllname, qvmname, basedir, gamename_long, gamename_short, msgarray, msgnames, vmsyscall, autodetected, homepath);

	return CEngineMgr::instance;
}

CEngineMgr* CEngineMgr::instance = NULL;
