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

/* $Id: CMod.cpp,v 1.9 2004/08/23 07:32:27 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_public.h>
#include "qmm.h"
#include "main.h"
#include "CMod.h"
#include "CDLL.h"
#include "util.h"

CMod::CMod() {
	this->dllEntry = NULL;
	this->vmMain = NULL;
	this->filename = NULL;
}

CMod::~CMod() {
	this->UnloadMod();
}

//load the given file as the mod, pass the given syscall pointer to the dllEntry() function
bool CMod::LoadMod(const char* file, eng_syscall_t psyscall) {
	//since the filename is made with vaf(), we should copy it to avoid recursive vaf() problems
	char tempfile[MAX_QPATH];
	memcpy(tempfile, file, strlen(file) + 1);
	file = (const char*)tempfile;

	//load DLL, or fail
	if (!this->dll.Load(file)) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: CDLL::Load(\"%s\") failed\n", file));
		return 0;
	}

	//find dllEntry() func, or fail
	if ((this->dllEntry = (mod_dllEntry_t)this->dll.GetFunc("dllEntry")) == NULL) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to find \"dllEntry\" function in mod file \"%s\"\n", file));
		return 0;
	}

	//find vmMain() func, or fail
	if ((this->vmMain = (mod_vmMain_t)this->dll.GetFunc("vmMain")) == NULL) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to find \"vmMain\" function in mod file \"%s\"\n", file));
		return 0;
	}

	//give our local syscall to the mod's dllEntry() func
	(this->dllEntry)(psyscall);

	//store filename
	int slen = strlen(file) + 1;
	this->filename = (char*)new char[slen];
	memcpy(this->filename, file, slen);

	return 1;
}

//unload mod file
void CMod::UnloadMod() {
	this->dllEntry = NULL;
	this->vmMain = NULL;
	if (this->filename)
		delete [] this->filename;
	this->filename = NULL;
	this->dll.Unload();
}
