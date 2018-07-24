/*

Stripper - Dynamic Map Entity Modification
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
    Kevin Masterson a.k.a. CyberMind <cybermind@users.sourceforge.net>

*/

/* $Id: util.cpp,v 1.2 2006/01/09 12:19:07 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_local.h>
#include <qmmapi.h>
#include "util.h"

//read a single line from a file handle
const char* read_line(fileHandle_t f, int fs) {
	static char temp[1024];
	char* p = temp;
	
	//how many bytes we've read
	static int fct = 0;

	//reset flag
	if (fs == -1)
		fct = 0;

	temp[0] = '\0';
	int i = 0;

	char buf = '.';
	g_syscall(G_FS_READ, &buf, 1, f); ++fct;
	if (fct > fs)
		return NULL;

	//read text until we hit a newline
	while ((buf != '\r') && (buf != '\n') && (i < sizeof(temp) - 1)) {
		temp[i++] = buf;
		if (fct >= fs)
			break;
		g_syscall(G_FS_READ, &buf, 1, f); ++fct;
	}
	temp[i] = '\0';

	while (isspace(*p))
		++p;

	for (--i; i >= 0 && isspace(temp[i]); --i) {
		temp[i] = '\0';
	}

	return p;
}
