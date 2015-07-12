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

/* $Id: qmm_extra.h,v 1.1 2005/02/27 06:11:05 cybermind Exp $ */

#ifndef __QMM_EXTRA_H__
#define __QMM_EXTRA_H__

#ifdef Q3_VM

//standard C malloc()
void* malloc(int size);
//standard C realloc()
void* realloc(void* addr, int newsize);
//standard C free()
void free(void* addr);

#endif //Q3_VM

#endif //__QMM_EXTRA_H__
