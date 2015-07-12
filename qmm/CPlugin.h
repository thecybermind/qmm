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

/* $Id: CPlugin.h,v 1.12 2004/08/30 02:34:53 cybermind Exp $ */

#ifndef __CPLUGIN_H__
#define __CPLUGIN_H__

#include "CDLL.h"
#include "qmm.h"

class CPlugin {
	public:
		CPlugin();
		~CPlugin();
		bool LoadQuery(const char*, int);
		int Attach(eng_syscall_t, mod_vmMain_t, int);
		bool Pause();
		void Unpause();
		bool Unload(int);
		
		plugin_vmmain get_QMM_vmMain() { return this->QMM_vmMain; }
		plugin_vmmain get_QMM_vmMain_Post() { return this->QMM_vmMain_Post; }
		plugin_syscall get_QMM_syscall() { return this->QMM_syscall; }
		plugin_syscall get_QMM_syscall_Post() { return this->QMM_syscall_Post; }
		plugininfo_t* get_plugininfo() { return this->plugininfo; }
		char* get_filename() { return this->filename; }
		bool get_paused() { return this->paused; }
		pluginres_t get_result() { return this->result; }
		void set_result(pluginres_t res) { this->result = res; }
		int get_iscmd() { return this->iscmd; }
	private:
		plugin_query QMM_Query;
		plugin_attach QMM_Attach;
		plugin_detach QMM_Detach;
		plugin_vmmain QMM_vmMain;
		plugin_vmmain QMM_vmMain_Post;
		plugin_syscall QMM_syscall;
		plugin_syscall QMM_syscall_Post;
		plugininfo_t* plugininfo;
		char* filename;
		bool paused;
		static pluginres_t result;
		int iscmd;
		CDLL dll;
};
#endif //__CPLUGIN_H__
