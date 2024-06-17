/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __CCONFIGMGR_H__
#define __CCONFIGMGR_H__

#include "osdef.h"
#include "pdb.h"

class CConfigMgr {
	public:
		CConfigMgr();
		~CConfigMgr();
		
		void LoadLib(const char*);
		void LoadConf(const char*);
		int IsLibLoaded();
		int IsConfLoaded();
		void UnloadConf();
		void UnloadLib();

		int GetInt(const char*, int = 0);
		char* GetStr(const char*);
		void* GetListRootNode(const char*);
		void* GetListNextNode(void*);
		char* GetListNodeID(void*);

		static CConfigMgr* GetInstance();

	private:
		struct pdb* db;
		int isloaded;

		static CConfigMgr* instance;
};

#endif //__CCONFIGMGR_H__
