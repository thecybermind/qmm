/*

QMMVM - VM Wrapper Tool
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
    along with phpUA; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Created By:
    Kevin Masterson a.k.a. CyberMind <cybermind@users.sourceforge.net>

Special Thanks:
    Q3Fusion (http://www.sourceforge.net/projects/q3fusion/)
    VM info from Phaethon (http://www.icculus.org/~phaethon/q3mc/q3vm_specs.html)
    Code3Arena (http://www.planetquake.com/code3arena/)

*/

/* $Id: qmmvm.h,v 1.4 2004/08/23 07:32:48 cybermind Exp $ */

/* This file should be included in q_shared.h in the VM-only section, under the
   inclusion of bg_lib.h. This file defines function prototypes for extra functions
   that QMMVM provides to QVMs.

*/
#ifndef __QMMVM_H
#define __QMMVM_H

#ifdef Q3_VM

//standard C malloc()
void* malloc(int size);
//standard C realloc()
void* realloc(void* addr, int newsize);
//standard C free()
void free(void* addr);

// Q3_VM
#endif

// __QMMVM_H
#endif
