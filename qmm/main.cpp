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

/* $Id: main.cpp,v 1.28 2004/08/30 02:34:53 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_public.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "qmm.h"
#include "CEngine.h"
#include "CMod.h"
#include "CPlugin.h"
#include "main.h"
#include "util.h"

CEngine Engine;
CMod Mod;
CLinkList<CPlugin> Plugins;

char qmm_modfile[MAX_QPATH], qmm_pluginfile[MAX_QPATH], qmm_execcfg[MAX_QPATH], fs_game[MAX_QPATH];
int qmm_debug = 0;

FILE* g_df = NULL;

//engine->mod
C_DLLEXPORT void dllEntry(eng_syscall_t psyscall) {
	Engine.set_syscall(psyscall);
	//don't call engine's syscall from here, it will crash (trust me)
	//in fact, it's best not to do anything else here other than save the pointer
}

//engine->mod
C_DLLEXPORT int vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	if (cmd == GAME_INIT) {

		ENGFUNC(G_PRINT, "[QMM] QMM v" QMM_VERSION " (" QMM_GAME "/" QMM_OS ") loaded\n");
		ENGFUNC(G_PRINT, "[QMM] Built " QMM_COMPILE " by " QMM_BUILDER "\n");
		ENGFUNC(G_PRINT, "[QMM] URL: http://www.quake3mm.net/\n");

		//make version cvar
		ENGFUNC(G_CVAR_REGISTER, NULL, "qmm_version", QMM_VERSION, CVAR_ROM | CVAR_NORESTART);

		//check cvars
		ENGFUNC(G_CVAR_VARIABLE_STRING_BUFFER, "qmm_modfile", qmm_modfile, sizeof(qmm_modfile));
		ENGFUNC(G_CVAR_VARIABLE_STRING_BUFFER, "qmm_pluginfile", qmm_pluginfile, sizeof(qmm_pluginfile));
		ENGFUNC(G_CVAR_VARIABLE_STRING_BUFFER, "qmm_execcfg", qmm_execcfg, sizeof(qmm_execcfg));
		qmm_debug = ENGFUNC(G_CVAR_VARIABLE_INTEGER_VALUE, "qmm_debug");

		//get gamedir
		ENGFUNC(G_CVAR_VARIABLE_STRING_BUFFER, "fs_game", fs_game, sizeof(fs_game));
	
		//if fs_game cvar isn't set, use default
		if (!strlen(fs_game))
			strncpy(fs_game, QMM_DEFAULT_MODDIR, sizeof(fs_game));

		if (qmm_debug) {
			g_df = fopen(vaf("%s/qmmlog.txt", fs_game), "w");
			ENGFUNC(G_PRINT, "[QMM] Cvar \"qmm_debug\" enabled, opening logfile\n");
			fprintf(g_df, "Opening logfile\n");
			fprintf(g_df, "[QMM] QMM v" QMM_VERSION " (" QMM_GAME "/" QMM_OS ") loaded\n");
			fprintf(g_df, "[QMM] Built " QMM_COMPILE " by " QMM_BUILDER "\n");
			fprintf(g_df, "[QMM] URL: http://www.quake3mm.net/\n");
		}

		//if mod path cvar isn't set, use default
		if (strlen(qmm_modfile))
			ENGFUNC(G_PRINT, vaf("[QMM] Cvar found: \"qmm_modfile\", mod path set to \"%s\"\n", qmm_modfile));
		else {
			strncpy(qmm_modfile, QMM_DEFAULT_MODFILE, sizeof(qmm_modfile));
			ENGFUNC(G_PRINT, vaf("[QMM] WARNING: Cvar not found: \"qmm_modfile\", mod path set to \"%s\"\n", qmm_modfile));
		}

		//if pluginfile cvar isn't set, use default
		if (strlen(qmm_pluginfile))
			ENGFUNC(G_PRINT, vaf("[QMM] Cvar found: \"qmm_pluginfile\", plugin file path set to \"%s\"\n", qmm_pluginfile));
		else {
			strncpy(qmm_pluginfile, "plugins.ini", sizeof(qmm_pluginfile));
			ENGFUNC(G_PRINT, vaf("[QMM] WARNING: Cvar not found: \"qmm_pluginfile\", plugin file path set to \"%s\"\n", qmm_pluginfile));
		}

		//if execcfg cvar isn't set, use default
		if (strlen(qmm_execcfg))
			ENGFUNC(G_PRINT, vaf("[QMM] Cvar found: \"qmm_execcfg\", exec cfg path set to \"%s\"\n", qmm_execcfg));
		else {
			strncpy(qmm_execcfg, "qmmexec.cfg", sizeof(qmm_execcfg));
			ENGFUNC(G_PRINT, vaf("[QMM] WARNING: Cvar not found: \"qmm_execcfg\", exec cfg path set to \"%s\"\n", qmm_execcfg));
		}

		//start loading mod
		ENGFUNC(G_PRINT, vaf("[QMM] Loading mod \"%s\"\n", qmm_modfile));
		if (!Mod.LoadMod(vaf("%s/%s", fs_game, qmm_modfile), syscall)) {
			ENGFUNC(G_ERROR, vaf("[QMM] FATAL ERROR: Unable to load mod \"%s\"\n", qmm_modfile));
			return 0;
		}
		ENGFUNC(G_PRINT, vaf("[QMM] Successfully loaded mod \"%s\"\n", qmm_modfile));

		//start loading plugins
		ENGFUNC(G_PRINT, vaf("[QMM] Loading plugin file \"%s\"\n", qmm_pluginfile));
		FILE* pluginfile = fopen(vaf("%s/%s", fs_game, qmm_pluginfile), "r");
		if (!pluginfile) {
			ENGFUNC(G_PRINT, vaf("[QMM] WARNING: Unable to open plugin file \"%s\"\n", qmm_pluginfile));
		} else {
			int loaded = 0;
			int slen = 0;
			char buf[MAX_QPATH];
			char* pbuf;
			while (!feof(pluginfile)) {
				pbuf = buf;

				//clear buffer and get new line
				memset(buf, 0, sizeof(buf));
				fgets(buf, sizeof(buf), pluginfile);

				//strip spaces off front of line
				while (*pbuf == ' ' || *pbuf == '\t')
					++pbuf;

				//ignore comments
				if (pbuf[0] == ';' || pbuf[0] == '#' || (pbuf[0] == '/' && pbuf[1] == '/'))
					continue;

				//strip off newlines
				slen = strlen(pbuf) - 1;
				while (slen >= 0 && (pbuf[slen] == '\n' || pbuf[slen] == '\r'))
					pbuf[slen--] = '\0';

				//strip whitespace off end of line
				while (slen >= 0 && (pbuf[slen] == ' ' || pbuf[slen] == '\t'))
					pbuf[slen--] = '\0';

				//check for blank line after stripping everything
				if (pbuf[0] == '\0')
					continue;

				ENGFUNC(G_PRINT, vaf("[QMM] Loading plugin \"%s\"\n", pbuf));
				//load plugin
				if (LoadPlugin(vaf("%s/%s", fs_game, pbuf), 0))
					++loaded;
			} //while(!feof(pluginfile))
			ENGFUNC(G_PRINT, vaf("[QMM] Loaded %i plugin(s) from plugin file \"%s\"\n", loaded, qmm_pluginfile));
		}	//if (pluginfile)

		//attempt to run execcfg
		ENGFUNC(G_PRINT, vaf("[QMM] Executing config file \"%s\"\n", qmm_execcfg));
		ENGFUNC(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, vaf("exec %s\n", qmm_execcfg));

		//we're done
		ENGFUNC(G_PRINT, "[QMM] Startup successful, proceeding to mod startup.\n");

	} else if (cmd == GAME_CONSOLE_COMMAND) {
		//check for qmm commands

		char buf[5], arg1[14], arg2[MAX_QPATH];
		ENGFUNC(G_ARGV, 0, buf, sizeof(buf));
		if (!strcmp("qmm",buf)) {
			
			switch (ENGFUNC(G_ARGC)) {
			case 1:
				ENGFUNC(G_PRINT, "[QMM] Usage: qmm <command> [params]\n");
				ENGFUNC(G_PRINT, "[QMM] Available commands:\n");
				ENGFUNC(G_PRINT, "[QMM] status - displays information about QMM\n");
				ENGFUNC(G_PRINT, "[QMM] list - displays information about loaded QMM plugins\n");
				ENGFUNC(G_PRINT, "[QMM] load <file> - loads a new plugin\n");
				ENGFUNC(G_PRINT, "[QMM] info <id> - outputs info on plugin with id\n");
				ENGFUNC(G_PRINT, "[QMM] pause <id> - pauses plugin with id\n");
				ENGFUNC(G_PRINT, "[QMM] unpause <id> - unpauses plugin with id\n");
				ENGFUNC(G_PRINT, "[QMM] unload <id> - unloads plugin with id\n");
				ENGFUNC(G_PRINT, "[QMM] force_unload <id> - forcefully unloads plugin with id\n");
				break;
			case 2:
				ENGFUNC(G_ARGV, 1, arg1, sizeof(arg1));
				if (!strcmp("status", arg1)) {
					ENGFUNC(G_PRINT, "[QMM] QMM v" QMM_VERSION " (" QMM_GAME "/" QMM_OS ") loaded\n");
					ENGFUNC(G_PRINT, "[QMM] Built " QMM_COMPILE " by " QMM_BUILDER "\n");
					ENGFUNC(G_PRINT, "[QMM] URL: http://www.quake3mm.net/\n");
					ENGFUNC(G_PRINT, vaf("[QMM] Loaded mod: %s\n", qmm_modfile));
					ENGFUNC(G_PRINT, vaf("[QMM] Plugin file: %s\n", qmm_pluginfile));
					ENGFUNC(G_PRINT, vaf("[QMM] Exec cfg: %s\n", qmm_execcfg));
					ENGFUNC(G_PRINT, vaf("[QMM] Debuglog: %s\n", (qmm_debug ? "on" : "off")));
				} else if (!strcmp("list", arg1)) {
					ENGFUNC(G_PRINT, "[QMM] id - plugin - loadcmd / unloadcmd / canpause\n");
					ENGFUNC(G_PRINT, "[QMM] --------------------------------------------\n");
					CLinkNode<CPlugin>* p = Plugins.get_first();
					int num = 0;
					plugininfo_t* info = NULL;
					while (p) {
						info = p->data->get_plugininfo();
						ENGFUNC(G_PRINT, vaf("[QMM] %i - %s - %s / %s / %s\n", num, info->name, info->loadcmd ? "yes" : "no", info->unloadcmd ? "yes" : "no", info->canpause ? "yes" : "no"));
						++num;
						p = p->next;
					}
				} else {
					ENGFUNC(G_PRINT, "[QMM] Invalid command or missing parameters, try \"qmm\" for more information\n");
				}
				break;
			case 3:
				ENGFUNC(G_ARGV, 1, arg1, sizeof(arg1));
				ENGFUNC(G_ARGV, 2, arg2, sizeof(arg2));
				if (!strcmp("load", arg1)) {
					LoadPlugin(vaf("%s/%s",fs_game,arg2), 1);
				} else if (!strcmp("pause", arg1)) {
					PausePlugin(atoi(arg2));
				} else if (!strcmp("unpause", arg1)) {
					UnpausePlugin(atoi(arg2));
				} else if (!strcmp("unload", arg1)) {
					UnloadPlugin(atoi(arg2), 1);
				} else if (!strcmp("force_unload", arg1)) {
					UnloadPlugin(atoi(arg2), 0);
				} else if (!strcmp("info", arg1)) {
					InfoPlugin(atoi(arg2));
				} else {
					ENGFUNC(G_PRINT, "[QMM] Invalid command or extra parameters, try \"qmm\" for more information\n");
				}
				break;
			default:
				ENGFUNC(G_PRINT, "[QMM] Invalid command, try \"qmm\" for more information\n");
				break;
			}
			return 1;	//do not pass to mod
		}

	}

	//call plugins and mod based on the result rules
	if (qmm_debug) fprintf(g_df, "vmMain(%s) call\n", modfuncname(cmd));
	int ret = CallMain(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	if (qmm_debug) fprintf(g_df, "vmMain(%s) = %i\n", modfuncname(cmd), ret);

	//if we are shutting down, unload mod, close debuglog, and unload plugins
	if (cmd == GAME_SHUTDOWN) {
		ENGFUNC(G_PRINT, "[QMM] Shutting down plugins\n");
		CLinkNode<CPlugin>* p = Plugins.get_first();
		while (p) {
			p->data->Unload(0);
			p = p->next;
		}

		//moved this after plugin unload, so plugins can call mod's vmMain while shutting down
		ENGFUNC(G_PRINT, "[QMM] Shutting down mod\n");
		Mod.UnloadMod();

		if (qmm_debug) {
			fprintf(g_df, "Closing logfile\n");
			ENGFUNC(G_PRINT, "[QMM] Closing logfile\n");
			fclose(g_df);
		}

		ENGFUNC(G_PRINT, "[QMM] Finished shutting down, prepared for unload.\n");
	}

	//return above value from mod/plugins
	return ret;
}

//mod->engine
int QDECL syscall(int cmd, ...) {
	//get all args
	va_list args;
	int arg[12];
	va_start(args, cmd);
	for (int i = 0; i < 12; ++i)
		arg[i] = va_arg(args, int);
	va_end(args);

	if (qmm_debug) fprintf(g_df, "syscall(%s)\n", engfuncname(cmd));
	int ret = CallSyscall(cmd, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9], arg[10], arg[11]);
	if (qmm_debug) fprintf(g_df, "syscall(%s) = %i\n", engfuncname(cmd), ret);
	return ret;
}


//do the QMM_vmMain plugin calls here
int CallMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	if (!MODFUNC)
		return 0;

	CLinkNode<CPlugin>* p = Plugins.get_first();

	//store max result
	pluginres_t maxresult = QMM_UNUSED;

	//store return value to use for the vmMain call (for QMM_OVERRIDE)
	int ret = 0;

	//temp int for return values for each plugin
	int temp = 0;
	while (p) {
		//if this plugin is paused, skip it
		if (p->data->get_paused())
			continue;

		//call plugin's vmMain and store return value
		temp = PLUGINMODFUNC(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);

		//check plugin's result flag
		switch (p->data->get_result()) {
		//unchanged: show warning
		case QMM_UNUSED:
			ENGFUNC(G_PRINT, vaf("[QMM] WARNING: Plugin \"%s\" did not set result flag for QMM_vmMain(%s)\n", p->data->get_plugininfo()->name, modfuncname(cmd)));
			break;
		//error: show error
		case QMM_ERROR:
			ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Plugin \"%s\" resulted in ERROR for QMM_vmMain(%s)\n", p->data->get_plugininfo()->name, modfuncname(cmd)));
			break;
		//ignored: set maxresult
		case QMM_IGNORED:
			//set new result if applicable
			if (maxresult < QMM_IGNORED)
				maxresult = QMM_IGNORED;
			break;
		//override: set maxresult and set ret to this return value
		case QMM_OVERRIDE:
			//set new result if applicable
			if (maxresult < QMM_OVERRIDE)
				maxresult = QMM_OVERRIDE;
			//set return value
			ret = temp;
			break;
		//supercede: set maxresult and set ret to this return value
		case QMM_SUPERCEDE:
			//set new result if applicable
			if (maxresult < QMM_SUPERCEDE)
				maxresult = QMM_SUPERCEDE;
			//set return value
			ret = temp;
			break;
		default:
			ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Plugin \"%s\" set unknown result flag \"%i\" for QMM_vmMain(%s)\n", p->data->get_plugininfo()->name, p->data->get_result(), modfuncname(cmd)));
			break;
		}

		//reset the plugin result
		p->data->set_result(QMM_UNUSED);
			
		p = p->next;
	}

	//call vmmain function based on maxresult rules (then call plugin's post func)
	switch(maxresult) {
	case QMM_UNUSED:
	case QMM_ERROR:
	case QMM_IGNORED:
		temp = MODFUNC(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
		p = Plugins.get_first();
		while (p) {
			//if this plugin is paused, skip it
			if (p->data->get_paused())
				continue;

			//call plugin's post func
			PLUGINMODFUNC_POST(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
			if (p->data->get_result() == QMM_ERROR)
				ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Plugin \"%s\" resulted in ERROR for QMM_vmMain_Post(%s)\n", p->data->get_plugininfo()->name, modfuncname(cmd)));
			
			//ignore result flag, reset to UNUSED
			p->data->set_result(QMM_UNUSED);
			
			p = p->next;
		}
		return temp;
	case QMM_OVERRIDE:
		MODFUNC(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	case QMM_SUPERCEDE:
		p = Plugins.get_first();
		while (p) {
			//if this plugin is paused, skip it
			if (p->data->get_paused())
				continue;

			//call plugin's post func
			PLUGINMODFUNC_POST(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
			if (p->data->get_result() == QMM_ERROR)
				ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Plugin \"%s\" resulted in ERROR for QMM_vmMain_Post(%s)\n", p->data->get_plugininfo()->name, modfuncname(cmd)));
			
			//ignore result flag, reset to UNUSED
			p->data->set_result(QMM_UNUSED);
			
			p = p->next;
		}
		return ret;
	//no idea, but just act like nothing happened *shifty eyes*
	default:
		return MODFUNC(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	}

	return 0;
}

int CallSyscall(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	CLinkNode<CPlugin>* p = Plugins.get_first();

	//store max result
	pluginres_t maxresult = QMM_UNUSED;

	//store return value to use for the engine call (for QMM_OVERRIDE)
	int ret = 0;

	//temp int for return values for each plugin
	int temp = 0;
	while (p) {
		//if this plugin is paused, skip it
		if (p->data->get_paused())
			continue;

		//call plugin's syscall and store return value
		temp = PLUGINENGFUNC(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);

		//check plugin's result flag
		switch (p->data->get_result()) {
		//unchanged: show warning
		case QMM_UNUSED:
			ENGFUNC(G_PRINT, vaf("[QMM] WARNING: Plugin \"%s\" did not set result flag for QMM_syscall(%s)\n", p->data->get_plugininfo()->name, engfuncname(cmd)));
			break;
		//error: show error
		case QMM_ERROR:
			ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Plugin \"%s\" resulted in ERROR for QMM_syscall(%s)\n", p->data->get_plugininfo()->name, engfuncname(cmd)));
			break;
		//ignored: set maxresult
		case QMM_IGNORED:
			//set new result if applicable
			if (maxresult < QMM_IGNORED)
				maxresult = QMM_IGNORED;
			break;
		//override: set maxresult and set ret to this return value
		case QMM_OVERRIDE:
			//set new result if applicable
			if (maxresult < QMM_OVERRIDE)
				maxresult = QMM_OVERRIDE;
			//set return value
			ret = temp;
			break;
		//supercede: set maxresult and set ret to this return value
		case QMM_SUPERCEDE:
			//set new result if applicable
			if (maxresult < QMM_SUPERCEDE)
				maxresult = QMM_SUPERCEDE;
			//set return value
			ret = temp;
			break;
		default:
			ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Plugin \"%s\" set unknown result flag \"%i\" for QMM_syscall(%s)\n", p->data->get_plugininfo()->name, p->data->get_result(), engfuncname(cmd)));
			break;
		}

		//reset this plugin's result
		p->data->set_result(QMM_UNUSED);
			
		p = p->next;
	}

	//call engine function based on maxresult rules (then call plugin's post func)
	switch(maxresult) {
	case QMM_UNUSED:
	case QMM_ERROR:
	case QMM_IGNORED:
		temp = ENGFUNC(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
		p = Plugins.get_first();
		while (p) {
			//if this plugin is paused, skip it
			if (p->data->get_paused())
				continue;

			//call post func
			PLUGINENGFUNC_POST(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
			if (p->data->get_result() == QMM_ERROR)
				ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Plugin \"%s\" resulted in ERROR for QMM_syscall_Post(%s)\n", p->data->get_plugininfo()->name, engfuncname(cmd)));
			
			//ignore result flag, reset to UNUSED
			p->data->set_result(QMM_UNUSED);
			
			p = p->next;
		}
		return temp;
	case QMM_OVERRIDE:
		ENGFUNC(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	case QMM_SUPERCEDE:
		p = Plugins.get_first();
		while (p) {
			//if this plugin is paused, skip it
			if (p->data->get_paused())
				continue;

			//call post func
			PLUGINENGFUNC_POST(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
			if (p->data->get_result() == QMM_ERROR)
				ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Plugin \"%s\" resulted in ERROR for QMM_syscall_Post(%s)\n", p->data->get_plugininfo()->name, engfuncname(cmd)));
			
			//ignore result flag, reset to UNUSED
			p->data->set_result(QMM_UNUSED);
			
			p = p->next;
		}
		return ret;
	//no idea, but just act like nothing happened *shifty eyes*
	default:
		return ENGFUNC(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	}
}


//plugin functions


//load a plugin with the iscmd flag
bool LoadPlugin(const char* file, int iscmd) {
	//since the filename is made with vaf(), we should copy it to avoid recursive vaf() problems
	char tempfile[MAX_QPATH];
	memcpy(tempfile, file, strlen(file) + 1);
	file = (const char*)tempfile;
	
	CPlugin* p = new CPlugin;
				
	//if it won't load, free memory and move on
	if (!p->LoadQuery(file, iscmd)) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Load failed for plugin file \"%s\"\n", file));
		delete p;
		return 0;
	}
	ENGFUNC(G_PRINT, vaf("[QMM] Attempting to attach plugin: %s v%s by %s (%s)\n", p->get_plugininfo()->name, p->get_plugininfo()->version, p->get_plugininfo()->author, p->get_plugininfo()->url));

	//if QMM_Attach() returns 0, unload
	if (!p->Attach(ENGFUNC, MODFUNC, iscmd)) {
		ENGFUNC(G_PRINT, vaf("[QMM] ERROR: Unable to attach plugin \"%s\" (%s), QMM_Attach() returned 0\n", p->get_plugininfo()->name, file));
		delete p;
		return 0;
	}
	ENGFUNC(G_PRINT, vaf("[QMM] Attached successfully, loaded plugin \"%s\" (%s) \n", p->get_plugininfo()->name, file));

	//add plugin to list
	Plugins.add(p);

	return 1;
}

//pause a plugin with given id
bool PausePlugin(int id) {
	int i = 0;
	CPlugin* p = findplugin(id);
	if (!p) return 0;

	//try to pause plugin and get return value
	bool q = p->Pause();
	if (!q)
		ENGFUNC(G_PRINT, vaf("[QMM] Plugin \"%s\" failed to pause\n", p->get_plugininfo()->name));
	else
		ENGFUNC(G_PRINT, vaf("[QMM] Plugin \"%s\" paused\n", p->get_plugininfo()->name));

	return q;
}

//unpause a plugin with given id
void UnpausePlugin(int id) {
	int i = 0;
	CPlugin* p = findplugin(id);
	if (!p) return;

	p->Unpause();
	ENGFUNC(G_PRINT, vaf("[QMM] Plugin \"%s\" unpaused\n", p->get_plugininfo()->name));

	return;
}

//unload a plugin with given id and send iscmd flag
//this is only used by the command, non-command unloading is done manually in vmMain(GAME_SHUTDOWN)
bool UnloadPlugin(int id, int iscmd) {
	int i = 0;
	CPlugin* p = findplugin(id);
	if (!p) return 0;

	//try to unload plugin and get return value
	bool q = p->Unload(iscmd);
	if (!q)
		ENGFUNC(G_PRINT, vaf("[QMM] Plugin \"%s\" failed to unload\n", p->get_plugininfo()->name));
	else {
		ENGFUNC(G_PRINT, vaf("[QMM] Plugin \"%s\" unloaded\n", p->get_plugininfo()->name));
		//remove it from the list
		Plugins.del(p);
		ENGFUNC(G_PRINT, vaf("[QMM] Plugin \"%i\" removed from plugin list\n", id));
	}
	return q;
}

//outputs info on plugin with given id
void InfoPlugin(int id) {
	int i = 0;
	CPlugin* p = findplugin(id);
	if (!p) return;

	plugininfo_t* q = p->get_plugininfo();
	ENGFUNC(G_PRINT, vaf("[QMM] Plugin Info for \"%s\":\n", p->get_filename()));
	ENGFUNC(G_PRINT, vaf("[QMM] Name: \"%s\"\n", q->name));
	ENGFUNC(G_PRINT, vaf("[QMM] Version: \"%s\"\n", q->version));
	ENGFUNC(G_PRINT, vaf("[QMM] URL: \"%s\"\n", q->url));
	ENGFUNC(G_PRINT, vaf("[QMM] Author: \"%s\"\n", q->author));
	ENGFUNC(G_PRINT, vaf("[QMM] Desc: \"%s\"\n", q->desc));
	ENGFUNC(G_PRINT, vaf("[QMM] Pausable: %s\n", q->canpause ? "yes" : "no"));
	ENGFUNC(G_PRINT, vaf("[QMM] Cmd Loadable: %s\n", q->loadcmd ? "yes" : "no"));
	ENGFUNC(G_PRINT, vaf("[QMM] Cmd Unloadable: %s\n", q->unloadcmd ? "yes" : "no"));
	ENGFUNC(G_PRINT, vaf("[QMM] Loaded from: %s\n", p->get_iscmd() ? "command" : "config"));

	return;
}
