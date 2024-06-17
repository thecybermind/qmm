/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __CMOD_H__
#define __CMOD_H__

#include "osdef.h"

class CMod {
	public:
		virtual ~CMod() {};

		virtual int LoadMod(const char*) = 0;

		virtual int vmMain(int, int, int, int, int, int, int, int, int, int, int, int, int) = 0;
		virtual int IsVM() = 0;
		virtual const char* File() = 0;

		virtual int GetBase() = 0;

		virtual void Status() = 0;

	private:
		char file[MAX_PATH];
};

#endif //__CMOD_H__
