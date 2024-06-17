/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __CDLL_H__
#define __CDLL_H__

#include <stddef.h>
#include "CLinkList.h"

class CDLL {
	public:
		CDLL();
		~CDLL();

		int Load(const char*);
		void* GetProc(const char*);
		void Unload();

	private:
		void* hDLL;
};

#endif //__CDLL_H__
