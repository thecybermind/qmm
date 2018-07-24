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

/* $Id: CDLLMod.h,v 1.7 2006/01/29 22:45:37 cybermind Exp $ */

#ifndef __CDLLMOD_H__
#define __CDLLMOD_H__

#include "osdef.h"
#include "CMod.h"
#include "CDLL.h"
#include "qmmapi.h"

class CDLLMod : public CMod {
	public:
		CDLLMod();
		~CDLLMod();

		int LoadMod(const char*);

		int vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11);

		inline int IsVM();

		inline const char* File();
		inline int GetBase();

		void Status();

	private:
		char file[MAX_PATH];
		CDLL dll;
		mod_vmMain_t pfnvmMain;
		mod_dllEntry_t pfndllEntry;
};

#endif //__CDLLMOD_H__
