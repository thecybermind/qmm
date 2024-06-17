/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __CPLUGINMGR_H__
#define __CPLUGINMGR_H__

#include "CPlugin.h"
#include "CLinkList.h"

class CPluginMgr {
	public:
		CPluginMgr();
		~CPluginMgr();

		int LoadPlugins();
		int UnloadPlugins();

		int LoadPlugin(const char*, int);
		int UnloadPlugin(CPlugin*, int);

		CPlugin* FindPlugin(int);
		CPlugin* FindPlugin(const char*);

		int NumPlugins();
		void ListPlugins();

		int CallvmMain(int, int, int, int, int, int, int, int, int, int, int, int, int);
		int Callsyscall(int, int, int, int, int, int, int, int, int, int, int, int, int, int);

		static CPluginMgr* GetInstance();

	private:
		CLinkList<CPlugin> plugins;
		int numplugins;

		static CPluginMgr* instance;
};
#endif //__CPLUGINMGR_H__
