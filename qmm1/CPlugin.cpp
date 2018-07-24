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

/* $Id: CPlugin.cpp,v 1.8 2005/10/12 22:57:57 cybermind Exp $ */

#include "CPlugin.h"
#include "qmm.h"
#include "util.h"

CPlugin::CPlugin() {
	this->QMM_Query = NULL;
	this->QMM_Attach = NULL;
	this->QMM_Detach = NULL;
	this->QMM_vmMain = NULL;
	this->QMM_vmMain_Post = NULL;
	this->QMM_syscall = NULL;
	this->QMM_syscall_Post = NULL;
	this->plugininfo = NULL;
	this->paused = 0;
	this->iscmd = 0;
}

CPlugin::~CPlugin() {
	this->dll.Unload();
}


//load the given file, and call the QMM_Query function
//if the plugin is not able to be loaded at runtime and iscmd==1, cancel

// - file is the path relative to the mod directory
// - iscmd determines if the file was loaded via server command (or from config file)
int CPlugin::LoadQuery(const char* file, int iscmd) {
	if (!file || !*file)
		return 0;

	//load DLL (prepend special homepath)
	int x = this->dll.Load(vaf("%s%s/%s", g_EngineMgr->GetHomepath(), g_EngineMgr->GetModDir(), file));
	if (!x) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): DLL load failed for plugin: %s\n", file, dlerror()));
		return 0;
	} else if (x == -1) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): DLL load failed for plugin: module already loaded\n", file));
		return 0;
	}

	//find QMM_Query() or fail
	if ((this->QMM_Query = (plugin_query)this->dll.GetProc("QMM_Query")) == NULL) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Unable to find \"QMM_Query\" function in plugin\n", file));
		return 0;
	}

	//call QMM_Query() func to get the plugininfo
	(this->QMM_Query)(&(this->plugininfo));

	if (!this->plugininfo) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Plugininfo NULL for plugin", file));
		return 0;
	}

	//check for plugin interface versions

	//if the plugin's major version is higher, don't load and suggest to upgrade QMM
	if (this->plugininfo->pifv_major > QMM_PIFV_MAJOR) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Plugin's major interface version (%d) is greater than QMM's (%d), suggest upgrading QMM.\n", file, this->plugininfo->pifv_major, QMM_PIFV_MAJOR));
		return 0;
	}
	//if the plugin's major version is lower, don't load and suggest to upgrade plugin
	if (this->plugininfo->pifv_major < QMM_PIFV_MAJOR) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Plugin's major interface version (%d) is less than QMM's (%d), suggest upgrading plugin.\n", file, this->plugininfo->pifv_major, QMM_PIFV_MAJOR));
		return 0;
	}
	//if the plugin's minor version is higher, don't load and suggest to upgrade QMM
	if (this->plugininfo->pifv_minor > QMM_PIFV_MINOR) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Plugin's minor interface version (%d) is greater than QMM's (%d), suggest upgrading QMM.\n", file, this->plugininfo->pifv_minor, QMM_PIFV_MINOR));
		return 0;
	}
	//if the plugin's minor version is lower, load, but suggest to upgrade plugin anyway
	if (this->plugininfo->pifv_minor < QMM_PIFV_MINOR)
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] WARNING: CPlugin::LoadQuery(\"%s\"): Plugin's minor interface version (%d) is less than QMM's (%d), suggest upgrading plugin.\n", file, this->plugininfo->pifv_minor, QMM_PIFV_MINOR));

	//if this is loaded via a command, but the plugin does not allow it, fail
	if (iscmd && !this->plugininfo->loadcmd) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Plugin cannot be loaded via command\n", file));
		return 0;
	}

	//find remaining neccesary functions or fail
	if ((this->QMM_Attach = (plugin_attach)this->dll.GetProc("QMM_Attach")) == NULL) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Unable to find \"QMM_Attach\" function in plugin\n", file));
		return 0;
	}
	if ((this->QMM_Detach = (plugin_detach)this->dll.GetProc("QMM_Detach")) == NULL) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Unable to find \"QMM_Detach\" function in plugin\n", file));
		return 0;
	}
	if ((this->QMM_vmMain = (plugin_vmmain)this->dll.GetProc("QMM_vmMain")) == NULL) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Unable to find \"QMM_vmMain\" function in plugin\n", file));
		return 0;
	}
	if ((this->QMM_syscall = (plugin_syscall)this->dll.GetProc("QMM_syscall")) == NULL) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Unable to find \"QMM_syscall\" function in plugin\n", file));
		return 0;
	}
	if ((this->QMM_vmMain_Post = (plugin_vmmain)this->dll.GetProc("QMM_vmMain_Post")) == NULL) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Unable to find \"QMM_vmMain_Post\" function in plugin\n", file));
		return 0;
	}
	if ((this->QMM_syscall_Post = (plugin_syscall)this->dll.GetProc("QMM_syscall_Post")) == NULL) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CPlugin::LoadQuery(\"%s\"): Unable to find \"QMM_syscall_Post\" function in plugin\n", file));
		return 0;
	}

	return 1;
}

//call plugin's QMM_Attach() function, pass real engine syscall, mod vmMain, and iscmd
int CPlugin::Attach(eng_syscall_t eng_syscall, mod_vmMain_t mod_vmMain, pluginfuncs_t* pluginfuncs, int vmbase, int iscmd) {
	//store iscmd for "qmm info"
	this->iscmd = iscmd;

	//call QMM_Attach() func with the engine syscall, mod vmMain, result ptr, and iscmd
	return (this->QMM_Attach)(eng_syscall, mod_vmMain, &this->result, pluginfuncs, vmbase, iscmd);
}

//pause plugin if it allows
int CPlugin::Pause() {
	if (!this->plugininfo->canpause || this->paused)
		return 0;

	this->paused = 1;

	return 1;
}

//unpause plugin
int CPlugin::Unpause() {
	if (!this->paused)
		return 0;

	this->paused = 0;

	return 1;
}

//unload plugin (with iscmd)
int CPlugin::Unload(int iscmd) {
	if (iscmd && (this->plugininfo && !this->plugininfo->unloadcmd))
		return 0;

	//call QMM_Detach() with iscmd
	if (this->QMM_Detach)
		(this->QMM_Detach)(iscmd);

	//unload DLL
	this->dll.Unload();

	return 1;
}

int CPlugin::Paused() {
	return this->paused;
}

int CPlugin::IsCmd() {
	return this->iscmd;
}

plugin_vmmain CPlugin::vmMain() {
	return this->QMM_vmMain;
}

plugin_vmmain CPlugin::vmMain_Post() {
	return this->QMM_vmMain_Post;
}

plugin_syscall CPlugin::syscall() {
	return this->QMM_syscall;
}

plugin_syscall CPlugin::syscall_Post() {
	return this->QMM_syscall_Post;
}

const plugininfo_t* CPlugin::PluginInfo() {
	return this->plugininfo;
}

pluginres_t CPlugin::Result() {
	return this->result;
}

void CPlugin::ResetResult() {
	this->result = QMM_UNUSED;
}

pluginres_t CPlugin::result = QMM_UNUSED;
