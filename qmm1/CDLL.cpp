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

/* $Id: CDLL.cpp,v 1.5 2005/09/24 07:11:42 cybermind Exp $ */

#include "CDLL.h"
#include "osdef.h"
#include "util.h"

CDLL::CDLL() {
	this->hDLL = NULL;
}

CDLL::~CDLL() {
	this->Unload();
}

int CDLL::Load(const char* file) {
	if (this->hDLL)
		return 0;

	this->hDLL = dlopen(file, RTLD_NOW);

	if (!this->hDLL)
		return 0;

	//if this handle is already loaded, report a failed load and reset hDLL
	if (this->hDLL == get_modulehandle() || ismoduleloaded(this->hDLL)) {
		this->hDLL = NULL;
		return -1;
	}

	setmoduleloaded(this->hDLL);

	return 1;
}

void* CDLL::GetProc(const char* func) {
	if (this->hDLL)
		return dlsym(this->hDLL, func);

	return NULL;
}

void CDLL::Unload() {
	if (this->hDLL) {
		dlclose(this->hDLL);
		setmoduleunloaded(this->hDLL);
	}
};
