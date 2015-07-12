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

/* $Id: CVMMod.h,v 1.7 2006/01/29 22:45:37 cybermind Exp $ */

#ifndef __CVMMOD_H__
#define __CVMMOD_H__

#include "CLinkList.h"
#include "osdef.h"
#include "CMod.h"
#include "qmm.h"
#include "qvm.h"

class CVMMod : public CMod {
	public:
		CVMMod();
		~CVMMod();

		int LoadMod(const char*);

		int vmMain(int, int, int, int, int, int, int, int, int, int, int, int, int);

		inline int IsVM();
		inline const char* File();

		inline int GetBase();

		void Status();

	private:
		char file[MAX_PATH];

		//segments
		qvmop_t* codesegment;	//code segment, each op is 8 bytes (4 op, 4 param)
		byte* datasegment;	//data segment, partially filled on load
		byte* stacksegment;	//stack segment

		//segment sizes
		int codeseglen;		//size of code segment
		int dataseglen;		//size of data segment
		int stackseglen;	//size of stack segment

		//"registers"
		qvmop_t* opptr;		//current op in code segment
		int* stackptr;		//pointer to current location in stack
		int argbase;		//lower end of arg heap

		//memory
		byte* memory;		//main block of memory
		int memorysize;		//size of memory block

		//extra
		int swapped;		//was this file byteswapped? (is the server big-endian)
		int filesize;		//.qvm file size

		qvmheader_t header;	//header information
};

#endif //__CVMMOD_H__
