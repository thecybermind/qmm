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

/* $Id: CDLLMod.cpp,v 1.15 2006/01/29 22:45:37 cybermind Exp $ */

#include <string.h>
#include "osdef.h"
#include "qmm.h"
#include "CModMgr.h"
#include "CEngineMgr.h"
#include "game_api.h"
#include "qmmapi.h"
#include "CDLL.h"
#include "CDLLMod.h"
#include "util.h"

CDLLMod::CDLLMod() {
	memset(this->file, 0, sizeof(this->file));
}

CDLLMod::~CDLLMod() {
	this->dll.Unload();
}

// - file is either the full path or relative to the install directory
//homepath shit is all handled in CModMgr::LoadMod()
int CDLLMod::LoadMod(const char* file) {
	if (!file || !*file)
		return 0;

	strncpy(this->file, file, sizeof(this->file));
	
	//load dll
	int x = this->dll.Load(this->file);

	//file not found
	if (!x) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CDLLMod::LoadMod(\"%s\"): Unable to load mod file: %s\n", file, dlerror()));
		return 0;
	} else if (x == -1) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CDLLMod::LoadMod(\"%s\"): Unable to load mod file: module already loaded\n", file));
		return 0;
	}

	//locate dllEntry() function or fail
	if ((this->pfndllEntry = (mod_dllEntry_t)this->dll.GetProc("dllEntry")) == NULL) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CDLLMod::LoadMod(\"%s\"): Unable to locate dllEntry() mod entry point\n", file));
		return 0;
	}

	//locate vmMain() function or fail
	if ((this->pfnvmMain = (mod_vmMain_t)this->dll.GetProc("vmMain")) == NULL) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CDLLMod::LoadMod(\"%s\"): Unable to locate vmMain() mod entry point\n", file));
		return 0;
	}

	//call mod's dllEntry
	this->pfndllEntry(g_ModMgr->QMM_SysCall());

	return 1;
}

int CDLLMod::vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	return this->pfnvmMain ? this->pfnvmMain(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) : 0;
}

int CDLLMod::IsVM() {
	return 0;
}

const char* CDLLMod::File() {
	return this->file[0] ? this->file : NULL;
}

int CDLLMod::GetBase() {
	return 0;
}

void CDLLMod::Status() {
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] dllEntry() offset: %p\n", this->pfndllEntry));
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] vmMain() offset: %p\n", this->pfnvmMain));
}
