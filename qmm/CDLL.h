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

/* $Id: CDLL.h,v 1.1 2004/05/12 11:40:28 cybermind Exp $ */

#ifndef __CDLL_H__
#define __CDLL_H__

class CDLL {
	public:
		CDLL();
		~CDLL();

		bool Load(const char*);
		void* GetFunc(const char*);
		void Unload();

	private:
		void* hDLL;
};

#endif //__CDLL_H__
