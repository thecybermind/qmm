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

/* $Id: CMod.h,v 1.6 2006/01/29 22:45:37 cybermind Exp $ */

#ifndef __CMOD_H__
#define __CMOD_H__

#include "osdef.h"

class CMod {
	public:
		virtual ~CMod() {};

		virtual int LoadMod(const char*) = 0;

		virtual int vmMain(int, int, int, int, int, int, int, int, int, int, int, int, int) = 0;
		virtual int IsVM() = 0;
		virtual const char* File() = 0;

		virtual int GetBase() = 0;

		virtual void Status() = 0;

	private:
		char file[MAX_PATH];
};

#endif //__CMOD_H__
