/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#include <stdlib.h>
#include "CEngineMgr.h"
#include "CPluginMgr.h"
#include "CPlugin.h"
#include "CModMgr.h"
#include "CLinkList.h"
#include "CConfigMgr.h"
#include "qmm.h"
#include "util.h"

CPluginMgr::CPluginMgr() {
	this->numplugins = 0;
}

CPluginMgr::~CPluginMgr() {
	this->UnloadPlugins();
}

int CPluginMgr::LoadPlugins() {
	if (!g_ConfigMgr->IsConfLoaded())
		return 0;

	void* linknode = g_ConfigMgr->GetListRootNode(vaf("%s/plugins", g_EngineMgr->GetModDir()));

	while (linknode) {
		//this->LoadPlugin fails on a NULL pointer so it's safe to do this
		this->numplugins += this->LoadPlugin(g_ConfigMgr->GetListNodeID(linknode), 0);
		
		linknode = g_ConfigMgr->GetListNextNode(linknode);
	}
	
	return this->numplugins;
}

int CPluginMgr::UnloadPlugins() {
	CLinkNode<CPlugin>* p = this->plugins.first();

	int i = 0;
	while (p) {
		p->data()->Unload(0);
		p = p->next();
		++i;
	}

	return i;
}

//i cant pass a member function pointer (g_ModMgr->Mod()->vmMain()) to plugins
//so this function is given which handles it
static int s_plugin_vmmain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	return MOD_VMMAIN(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
}

// - file is the path relative to the mod directory (mod dir and homepath are added in CPlugin::LoadQuery)
// - iscmd determines if the file was loaded via server command (or from config file)

int CPluginMgr::LoadPlugin(const char* file, int iscmd) {
	if (!file || !*file)
		return 0;

	CPlugin* p = new CPlugin;
	
	if (!p->LoadQuery(file, iscmd)) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPluginMgr::LoadPlugin(\"%s\"): Unable to load plugin due to previous errors\n", file));
		delete p;
		return 0;
	}
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] CPluginMgr::LoadPlugin(\"%s\"): Successfully queried plugin \"%s\"\n", file, p->PluginInfo()->name));

	if (!p->Attach(g_EngineMgr->SysCall(), s_plugin_vmmain, get_pluginfuncs(), g_ModMgr->Mod()->GetBase(), iscmd)) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] CPluginMgr::LoadPlugin(\"%s\"): QMM_Attach() returned 0 for plugin \"%s\"\n", file, p->PluginInfo()->name));
		p->Unload(0);
		delete p;
		return 0;
	}
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] CPluginMgr::LoadPlugin(\"%s\"): Successfully attached plugin \"%s\"\n", file, p->PluginInfo()->name));

	this->plugins.add(p);
	return 1;
}

int CPluginMgr::UnloadPlugin(CPlugin* plugin, int iscmd) {
	if (!plugin)
		return 0;

	CLinkNode<CPlugin>* p = this->plugins.first();

	while (p) {
		if (p->data() == plugin) {
			if (p->data()->Unload(iscmd)) {
				this->plugins.del(p);
				return 1;
			}
			return 0;
		}
		p = p->next();
	}

	return 0;
}

CPlugin* CPluginMgr::FindPlugin(const char* string) {
	if (!string || !*string)
		return NULL;

	if (isnumber(string))
		return this->FindPlugin(atoi(string));

	CPlugin* found = NULL;

	CLinkNode<CPlugin>* p = this->plugins.first();

	while (p) {
		if (my_strcasestr(p->data()->PluginInfo()->name, string)) {
			if (!found)
				found = p->data();
			else
				return NULL;
		}
		p = p->next();
	}

	return found;
}

CPlugin* CPluginMgr::FindPlugin(int num) {
	if (num < 0)
		return NULL;

	int i = 0;

	CLinkNode<CPlugin>* p = this->plugins.first();

	while (p) {
		if (i == num)
			return p->data();
		++i;
		p = p->next();
	}

	return NULL;
}

int CPluginMgr::NumPlugins() {
	return this->numplugins;
}

void CPluginMgr::ListPlugins() {
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), "[QMM] id - loadcmd / unloadcmd / canpause / paused / source - version - plugin\n");
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), "[QMM] ------------------------------------------------------------------------\n");
	CLinkNode<CPlugin>* p = this->plugins.first();
	int num = 0;
	const plugininfo_t* info = NULL;
	while (p) {
		info = p->data()->PluginInfo();
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] %.2d - %s / %s / %s / %s / %s - %7s - %s\n", num, info->loadcmd ? "  yes  " : "  no   ", info->unloadcmd ? "   yes   " : "   no    ", info->canpause ? "  yes   " : "   no   ", p->data()->Paused() ? " yes  " : "  no  ", p->data()->IsCmd() ? " cmd  " : " conf ", info->version, info->name));
		++num;
		p = p->next();
	}
}

int CPluginMgr::CallvmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	if (!g_ModMgr->Mod())
		return 0;

	CLinkNode<CPlugin>* p = this->plugins.first();

	//store max result
	pluginres_t maxresult = QMM_UNUSED;

	//store return value to use for the vmMain call (for QMM_OVERRIDE)
	int ret = 0;

	//temp int for return values for each plugin
	int temp = 0;
	while (p) {
		//if this plugin is paused, skip it
		if (p->data()->Paused()) {
			p = p->next();
			continue;
		}

		//call plugin's vmMain and store return value
		temp = (p->data()->vmMain())(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);

		//check plugin's result flag
		switch (p->data()->Result()) {
			//unchanged: show warning
			case QMM_UNUSED:
				ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] WARNING: CPluginMgr::CallvmMain(%s): Plugin \"%s\" did not set result flag\n", ENG_MSGNAME(cmd), p->data()->PluginInfo()->name));
				break;

			//error: show error
			case QMM_ERROR:
				ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPluginMgr::CallvmMain(%s): Plugin \"%s\" resulted in ERROR\n", ENG_MSGNAME(cmd), p->data()->PluginInfo()->name));
				break;
			
			//override: set maxresult and set ret to this return value (fall-through)
			case QMM_OVERRIDE:

			//supercede: set maxresult and set ret to this return value (fall-through)
			case QMM_SUPERCEDE:
				ret = temp;

			//ignored: set maxresult
			case QMM_IGNORED:
				//set new result if applicable
				if (maxresult < p->data()->Result())
					maxresult = p->data()->Result();
				break;

			default:
				ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPluginMgr::CallvmMain(%s): Plugin \"%s\" set unknown result flag \"%d\"\n", ENG_MSGNAME(cmd), p->data()->PluginInfo()->name, p->data()->Result()));
				break;
		}

		//reset the plugin result
		p->data()->ResetResult();
			
		p = p->next();
	}

	//call vmmain function based on maxresult rules (then call plugin's post func)
	switch(maxresult) {
		//run mod's vmMain and store result
		case QMM_UNUSED:
		case QMM_ERROR:
		case QMM_IGNORED:
		case QMM_OVERRIDE:
			temp = MOD_VMMAIN(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
			//the return value for GAME_CLIENT_CONNECT is a char* so we have to modify the pointer value for VMs
			if (cmd == MOD_MSG(QMM_GAME_CLIENT_CONNECT) && g_ModMgr->Mod()->IsVM() && temp /* dont bother if its NULL */)
				temp += g_ModMgr->Mod()->GetBase();

			if (maxresult != QMM_OVERRIDE)
				ret = temp;
		case QMM_SUPERCEDE:
			p = this->plugins.first();
			while (p) {
				//if this plugin is paused, skip it
				if (p->data()->Paused()) {
					p = p->next();
					continue;
				}

				//call plugin's post func
				(p->data()->vmMain_Post())(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
				if (p->data()->Result() == QMM_ERROR)
					ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPluginMgr::CallvmMain(%s): Plugin \"%s\" resulted in ERROR\n", ENG_MSGNAME(cmd), p->data()->PluginInfo()->name));
				
				//ignore result flag, reset to UNUSED
				p->data()->ResetResult();
				
				p = p->next();
			}

			return ret;
		//no idea, but just act like nothing happened *shifty eyes*
		default:
			return MOD_VMMAIN(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	}
}

int CPluginMgr::Callsyscall(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12) {
	CLinkNode<CPlugin>* p = this->plugins.first();

	//store max result
	pluginres_t maxresult = QMM_UNUSED;

	//store return value to use for the vmMain call (for QMM_OVERRIDE)
	int ret = 0;

	//temp int for return values for each plugin
	int temp = 0;
	while (p) {
		//if this plugin is paused, skip it
		if (p->data()->Paused()) {
			p = p->next();
			continue;
		}

		//call plugin's vmMain and store return value
		temp = (p->data()->syscall())(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);

		//check plugin's result flag
		switch (p->data()->Result()) {
			//unchanged: show warning
			case QMM_UNUSED:
				ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] WARNING: CPluginMgr::Callsyscall(%s): Plugin \"%s\" did not set result flag\n", ENG_MSGNAME(cmd), p->data()->PluginInfo()->name));
				break;

			//error: show error
			case QMM_ERROR:
				ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPluginMgr::Callsyscall(%s): Plugin \"%s\" resulted in ERROR\n", ENG_MSGNAME(cmd), p->data()->PluginInfo()->name));
				break;
			
			//override: set maxresult and set ret to this return value (fall-through)
			case QMM_OVERRIDE:

			//supercede: set maxresult and set ret to this return value (fall-through)
			case QMM_SUPERCEDE:
				ret = temp;

			//ignored: set maxresult
			case QMM_IGNORED:
				//set new result if applicable
				if (maxresult < p->data()->Result())
					maxresult = p->data()->Result();
				break;

			default:
				ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPluginMgr::Callsyscall(%s): Plugin \"%s\" set unknown result flag \"%d\"\n", ENG_MSGNAME(cmd), p->data()->PluginInfo()->name, p->data()->Result()));
				break;
		}

		//reset the plugin result
		p->data()->ResetResult();
			
		p = p->next();
	}

	//call vmmain function based on maxresult rules (then call plugin's post func)
	switch(maxresult) {
		//run mod's vmMain and store result
		case QMM_UNUSED:
		case QMM_ERROR:
		case QMM_IGNORED:
		case QMM_OVERRIDE:
			temp = ENG_SYSCALL(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
			if (maxresult != QMM_OVERRIDE)
				ret = temp;
		case QMM_SUPERCEDE:
			p = this->plugins.first();
			while (p) {
				//if this plugin is paused, skip it
				if (p->data()->Paused()) {
					p = p->next();
					continue;
				}

				//call plugin's post func
				(p->data()->syscall_Post())(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
				if (p->data()->Result() == QMM_ERROR)
					ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPluginMgr::Callsyscall(%s): Plugin \"%s\" resulted in ERROR\n", ENG_MSGNAME(cmd), p->data()->PluginInfo()->name));
				
				//ignore result flag, reset to UNUSED
				p->data()->ResetResult();
				
				p = p->next();
			}
			
			return ret;
		//no idea, but just act like nothing happened *shifty eyes*
		default:
			return ENG_SYSCALL(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
	}
}

CPluginMgr* CPluginMgr::GetInstance() {
	if (!CPluginMgr::instance)
		CPluginMgr::instance = new CPluginMgr;

	return CPluginMgr::instance;
}

CPluginMgr* CPluginMgr::instance = NULL;
