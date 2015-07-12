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

/* $Id: osdef.cpp,v 1.3 2005/10/12 22:57:57 cybermind Exp $ */

#include "osdef.h"

#ifdef WIN32

#include "version.h"
//the \1 is so it is separate from anything before it when shown in text scanners
//using \0 generates a blank comment on the command line, causing an error
//due to the fact that it is a null terminator
#pragma comment(exestr, "\1QMM v" QMM_VERSION " (" QMM_OS ")")
#pragma comment(exestr, "\1Built: " QMM_COMPILE " by " QMM_BUILDER)
#pragma comment(exestr, "\1URL: http://www.q3mm.org/ http://www.planetquake.com/qmm/")

char* dlerror() {
	static char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, sizeof(buffer), NULL);
	
	//remove annoying newlines at the end
	buffer[strlen(buffer) - 2] = '\0';

	return buffer;
}
#endif
