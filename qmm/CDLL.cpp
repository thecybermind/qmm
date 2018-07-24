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

/* $Id: CDLL.cpp,v 1.7 2004/08/17 03:56:16 cybermind Exp $ */

#include "version.h"
#include <stdlib.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "CDLL.h"

CDLL::CDLL() {
	this->hDLL = NULL;
}

CDLL::~CDLL() {
	this->Unload();

	this->hDLL = NULL;
}

//load lib with given filename
bool CDLL::Load(const char* file) {
#ifdef WIN32
	//don't bother with the fugly win32 types
	this->hDLL = (void*)LoadLibrary(file);
#else
	this->hDLL = dlopen(file, RTLD_NOW);
#endif
	return (this->hDLL ? 1 : 0);
}

void* CDLL::GetFunc(const char* func) {
#ifdef WIN32
	return (void*)GetProcAddress((HINSTANCE)this->hDLL, func);
#else
	return dlsym(this->hDLL, func);
#endif
}

void CDLL::Unload() {
	if (this->hDLL)
#ifdef WIN32
		FreeLibrary((HINSTANCE)this->hDLL);
#else
		dlclose(this->hDLL);
#endif
}
