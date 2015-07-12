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

/* $Id: CDLL.h,v 1.2 2005/03/03 16:06:02 cybermind Exp $ */

#ifndef __CDLL_H__
#define __CDLL_H__

#include <stddef.h>
#include "CLinkList.h"

class CDLL {
	public:
		CDLL();
		~CDLL();

		int Load(const char*);
		void* GetProc(const char*);
		void Unload();

	private:
		void* hDLL;
};

#endif //__CDLL_H__
