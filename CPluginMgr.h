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

/* $Id: CPluginMgr.h,v 1.5 2005/10/12 22:57:57 cybermind Exp $ */

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
