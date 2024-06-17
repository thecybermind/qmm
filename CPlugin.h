/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __CPLUGIN_H__
#define __CPLUGIN_H__

#include <string.h>
#include "CDLL.h"
#include "qmmapi.h"
#include "osdef.h"

class CPlugin {
	public:
		CPlugin();
		~CPlugin();

		int LoadQuery(const char*, int);
		int Attach(eng_syscall_t, mod_vmMain_t, pluginfuncs_t*, int, int);
		int Pause();
		int Unpause();
		int Unload(int);

		int Paused();

		int IsCmd();

		plugin_vmmain vmMain();
		plugin_vmmain vmMain_Post();
		plugin_syscall syscall();
		plugin_syscall syscall_Post();

		const plugininfo_t* PluginInfo();

		pluginres_t Result();
		void ResetResult();

	private:
		CDLL dll;
		plugin_query QMM_Query;
		plugin_attach QMM_Attach;
		plugin_detach QMM_Detach;
		plugin_vmmain QMM_vmMain;
		plugin_vmmain QMM_vmMain_Post;
		plugin_syscall QMM_syscall;
		plugin_syscall QMM_syscall_Post;
		plugininfo_t* plugininfo;
		int paused;
		int iscmd;
		static pluginres_t result;
};
#endif //__CPLUGIN_H__
