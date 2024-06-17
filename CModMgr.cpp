/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#include <stddef.h>
#include <string.h>
#include "osdef.h"
#include "CConfigMgr.h"
#include "CEngineMgr.h"
#include "CModMgr.h"
#include "CMod.h"
#include "CDLLMod.h"
#include "CVMMod.h"
#include "game_api.h"
#include "util.h"

CModMgr::CModMgr(eng_syscall_t qmm_syscall, int msgarray[], msgname_t msgnames) {
	this->qmm_syscall = qmm_syscall;
	this->msgarray = msgarray;
	this->msgnames = msgnames;

	this->mod = NULL;
}

CModMgr::~CModMgr() {
	this->UnloadMod();
}

// - file is the path relative to the mod directory
//this uses the engine functions to reliably open the file regardless of homepath crap
CMod* CModMgr::newmod(const char* file) {
	CMod* ret = NULL;
	int fmod;
	char hdr[4];
	
	//attempt to open the file
	int filesize = ENG_SYSCALL(ENG_MSG(QMM_G_FS_FOPEN_FILE), file, &fmod, ENG_MSG(QMM_FS_READ));
	//if it exists and loaded
	if (filesize > 0) {
		//read first 4 bytes and close file handle
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), &hdr, sizeof(hdr), fmod);
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_FCLOSE_FILE), fmod);

		//compare first 4 bytes with the following list:
		//dll: 4D 5A 90 00 'MZ??'
		//so:  7F 45 4C 46 '?ELF'
		//qvm: 44 14 72 12 'D?r?'

		#ifdef WIN32
		if (hdr[0] == 'M' && hdr[1] == 'Z' && hdr[2] == 0x90 && hdr[3] == 0x00)
		#else
		if (hdr[0] == 0x7F && hdr[1] == 'E' && hdr[2] == 'L' && hdr[3] == 'F')
		#endif
			ret = new CDLLMod;
		//only allow it to return a qvm mod if this game supports it
		else if (g_EngineMgr->VMSysCall() && hdr[0] == 'D' && hdr[1] == 0x14 && hdr[2] == 'r' && hdr[3] == 0x12)
			ret = new CVMMod;
	}

	//if unable to determine file type via header, just check the extension
	if (!ret) {
		//get the 3-letter file extension
		char* qfile = (char*)&file[strlen(file) - strlen(QVM_EXT)];
		//get the 2/3-letter file extension
		char* dfile = (char*)&file[strlen(file) - strlen(DLL_EXT)];
	
		//if the extension is .dll/.so
		if (!strcasecmp(dfile, DLL_EXT))
			ret = new CDLLMod;
		
		//if the extension is .qvm, only allow if this game supports it
		else if (!strcasecmp(qfile, QVM_EXT) && g_EngineMgr->VMSysCall())
			ret = new CVMMod;
	}
	
	return ret;
}

//attempts to load a mod in the following search order:
// - a mod file specified in the config file
//	- dll mod is loaded from homepath then install dir
//	- vm mod is loaded using engine functions
// - a dll/so named qmm_<modfilename> in the homepath
// - a dll/so named qmm_<modfilename> in the install dir
// - a qvm named vm/<modqvmname>
int CModMgr::LoadMod() {

	//load mod file setting from config file
	//this should be relative to mod directory
	char* cfg_mod = g_ConfigMgr->GetStr(vaf("%s/mod", g_EngineMgr->GetModDir()));
	
	if (cfg_mod && *cfg_mod) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] CModMgr::LoadMod(): Mod file specified in configuration file: \"%s\"\n", cfg_mod));

		//detect mod type
		this->mod = this->newmod(cfg_mod);

		//if a type was detected
		if (this->mod) {
			//vm mod, no path adjustment is needed, it either loads or doesn't
			if (this->mod->IsVM()) {
				if (this->mod->LoadMod(cfg_mod))
					return 1;

				//delete VM mod object since we will try to load a DLL mod next
				delete this->mod;
			
			//dll mod, try homepath first, then install dir
			} else {
				//load with homepath first
				if (this->mod->LoadMod(vaf("%s%s/%s", g_EngineMgr->GetHomepath(), g_EngineMgr->GetModDir(), cfg_mod)))
					return 1;

				//if a homepath exists, and the above load failed, load from install dir
				if (g_EngineMgr->GetHomepath()[0]) {
					ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\" in homepath, checking install directory\n", cfg_mod));
					if (this->mod->LoadMod(vaf("%s/%s", g_EngineMgr->GetModDir(), cfg_mod)))
						return 1;

					//load failed
					ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\" in install directory\n", cfg_mod));
				}

				//attempt to load dll mod using default filename
				ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\", attempting to load default DLL mod file \"qmm_%s\"\n", cfg_mod, g_EngineMgr->GetDLLName()));
			}
		
		//mod type wasn't detected
		} else {
			ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to determine mod type of file \"%s\"\n", cfg_mod));
		}
	} else {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] WARNING: CModMgr::LoadMod(): Unable to detect mod file setting from configuration file, attempting to load default DLL mod file \"qmm_%s\"\n", g_EngineMgr->GetDLLName()));
	}

	//attempt to load qmm_<dllname>
	cfg_mod = vaf("qmm_%s", g_EngineMgr->GetDLLName());

	//make dll mod object
	this->mod = new CDLLMod;

	//load with homepath first
	if (this->mod->LoadMod(vaf("%s%s/%s", g_EngineMgr->GetHomepath(), g_EngineMgr->GetModDir(), cfg_mod)))
		return 1;

	//if a homepath exists, and the above load failed, load from install dir
	if (g_EngineMgr->GetHomepath()[0]) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\" in homepath, checking install directory\n", cfg_mod));
		if (this->mod->LoadMod(vaf("%s/%s", g_EngineMgr->GetModDir(), cfg_mod)))
			return 1;

		//load failed
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\" in install directory\n", cfg_mod));
	}

	//attempt to load qvm mod using default filename if the game supports it
	if (g_EngineMgr->GetQVMName()) {
		//delete DLL mod object since we will try to load a VM mod next
		delete this->mod;
		
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CModMgr::LoadMod(): Unable to load mod file \"%s\", attempting to load default QVM mod file \"%s\"\n", cfg_mod, g_EngineMgr->GetQVMName()));

		cfg_mod = (char*)g_EngineMgr->GetQVMName();
		this->mod = new CVMMod;
		if (this->mod->LoadMod(cfg_mod))
			return 1;
	}

	//delete mod object since we failed
	delete this->mod;

	ENG_SYSCALL(ENG_MSG(QMM_G_ERROR), "[QMM] FATAL ERROR: Unable to load mod file\n");

	return 0;
}

void CModMgr::UnloadMod() {
	if (this->mod)
		delete this->mod;
}

eng_syscall_t CModMgr::QMM_SysCall() {
	return this->qmm_syscall;
}

CMod* CModMgr::Mod() {
	return this->mod;
}

int CModMgr::GetMsg(mod_msg_t msg) {
	return this->msgarray[msg];
}

const char* CModMgr::GetMsgName(int msg) {
	return this->msgnames(msg);
}

CModMgr* CModMgr::GetInstance(eng_syscall_t qmm_syscall, int msgarray[], msgname_t msgnames) {
	if (!CModMgr::instance)
		CModMgr::instance = new CModMgr(qmm_syscall, msgarray, msgnames);

	return CModMgr::instance;
}

CModMgr* CModMgr::instance = NULL;
