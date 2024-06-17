/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

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
