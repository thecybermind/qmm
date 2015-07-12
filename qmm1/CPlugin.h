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

/* $Id: CPlugin.h,v 1.5 2005/09/21 22:39:12 cybermind Exp $ */

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
