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

/* $Id: CConfigMgr.h,v 1.2 2005/10/12 22:57:57 cybermind Exp $ */

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
