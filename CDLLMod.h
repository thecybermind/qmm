/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __CDLLMOD_H__
#define __CDLLMOD_H__

#include "osdef.h"
#include "CMod.h"
#include "CDLL.h"
#include "qmmapi.h"

class CDLLMod : public CMod {
	public:
		CDLLMod();
		~CDLLMod();

		int LoadMod(const char*);

		int vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11);

		inline int IsVM();

		inline const char* File();
		inline int GetBase();

		void Status();

	private:
		char file[MAX_PATH];
		CDLL dll;
		mod_vmMain_t pfnvmMain;
		mod_dllEntry_t pfndllEntry;
};

#endif //__CDLLMOD_H__
