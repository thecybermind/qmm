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

/* $Id: CPlugin.cpp,v 1.16 2004/08/30 02:34:53 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_public.h>
#include "qmm.h"
#include "main.h"
#include "CPlugin.h"
#include "CDLL.h"
#include "util.h"

CPlugin::CPlugin() {
	this->QMM_Query = NULL;
	this->QMM_Attach = NULL;
	this->QMM_Detach = NULL;
	this->QMM_vmMain = NULL;
	this->QMM_syscall = NULL;
	this->QMM_vmMain_Post = NULL;
	this->QMM_syscall_Post = NULL;
	this->plugininfo = NULL;
	this->filename = NULL;
	this->paused = 0;
	this->result = QMM_UNUSED;
	this->iscmd = 0;
}

CPlugin::~CPlugin() {
	this->QMM_Query = NULL;
	this->QMM_Attach = NULL;
	this->QMM_Detach = NULL;
	this->QMM_vmMain = NULL;
	this->QMM_syscall = NULL;
	this->QMM_vmMain_Post = NULL;
	this->QMM_syscall_Post = NULL;
	this->plugininfo = NULL;
	if (this->filename)
		delete [] this->filename;
	this->filename = NULL;
	this->paused = 0;
	this->result = QMM_UNUSED;
	this->iscmd = 0;
	//uncomment for a quick hack to not unload a dll thats loaded twice (causing crash)
	//unload moved to ::Unload()
	//this->dll.Unload();
}

//load the given file, and call the QMM_Query function
//if the plugin is not able to be loaded at runtime and iscmd==1, cancel
bool CPlugin::LoadQuery(const char* file, int iscmd) {
	//load DLL, or fail
	if (!this->dll.Load(file)) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: CDLL::Load(\"%s\") failed\n", file));
		return 0;
	}

	//find QMM_Query() or fail
	if ((this->QMM_Query = (plugin_query)this->dll.GetFunc("QMM_Query")) == NULL) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to find \"QMM_Query\" function in plugin file \"%s\"\n", file));
		return 0;
	}

	//call QMM_Query() func to get the plugininfo
	(this->QMM_Query)(&(this->plugininfo));

	ENGFUNC(G_PRINT, vaf("[QMM] Query successful for plugin \"%s\" (%s)\n", this->plugininfo->name, file));

	//if this is loaded via a command, but the plugin does not allow it, fail
	if (iscmd && !this->plugininfo->loadcmd) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Plugin \"%s\" cannot be loaded via command\n", this->plugininfo->name));
		return 0;
	}

	//find remaining neccesary functions or fail
	if ((this->QMM_Attach = (plugin_attach)this->dll.GetFunc("QMM_Attach")) == NULL) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to find \"QMM_Attach\" function in plugin \"%s\"\n", this->plugininfo->name));
		return 0;
	}
	if ((this->QMM_Detach = (plugin_detach)this->dll.GetFunc("QMM_Detach")) == NULL) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to find \"QMM_Detach\" function in plugin \"%s\"\n", this->plugininfo->name));
		return 0;
	}
	if ((this->QMM_vmMain = (plugin_vmmain)this->dll.GetFunc("QMM_vmMain")) == NULL) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to find \"QMM_vmMain\" function in plugin \"%s\"\n", this->plugininfo->name));
		return 0;
	}
	if ((this->QMM_syscall = (plugin_syscall)this->dll.GetFunc("QMM_syscall")) == NULL) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to find \"QMM_syscall\" function in plugin \"%s\"\n", this->plugininfo->name));
		return 0;
	}
	if ((this->QMM_vmMain_Post = (plugin_vmmain)this->dll.GetFunc("QMM_vmMain_Post")) == NULL) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to find \"QMM_vmMain_Post\" function in plugin \"%s\"\n", this->plugininfo->name));
		return 0;
	}
	if ((this->QMM_syscall_Post = (plugin_syscall)this->dll.GetFunc("QMM_syscall_Post")) == NULL) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to find \"QMM_syscall_Post\" function in plugin \"%s\"\n", this->plugininfo->name));
		return 0;
	}

	//store filename
	int slen = strlen(file) + 1;
	this->filename = (char*)new char[slen];
	memcpy(this->filename, file, slen);

	return 1;
}

//call plugin's QMM_Attach() function, pass real engine syscall, mod vmMain, and iscmd
int CPlugin::Attach(eng_syscall_t eng_syscall, mod_vmMain_t mod_vmMain, int iscmd) {
	//store iscmd for "qmm info"
	this->iscmd = iscmd;

	//call QMM_Attach() func with the engine syscall, mod vmMain, result ptr, and iscmd
	return (this->QMM_Attach)(eng_syscall, mod_vmMain, &this->result, iscmd);
}

//pause plugin if it allows
bool CPlugin::Pause() {
	if (!this->plugininfo->canpause)
		return 0;

	this->paused = 1;

	return 1;
}

//unpause plugin
void CPlugin::Unpause() {
	this->paused = 0;
}

//unload plugin (with iscmd)
bool CPlugin::Unload(int iscmd) {
	if (iscmd && !this->plugininfo->unloadcmd)
		return 0;

	//call QMM_Detach() with iscmd
	(this->QMM_Detach)(iscmd);

	//unload DLL
	this->dll.Unload();

	return 1;
}

pluginres_t CPlugin::result = QMM_UNUSED;
