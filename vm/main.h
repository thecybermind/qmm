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
    along with QMM; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Created By:
    Kevin Masterson a.k.a. CyberMind <cybermind@users.sourceforge.net>

Special Thanks:
    VM code from Q3Fusion (http://www.sourceforge.net/projects/q3fusion/)
    VM info from Phaethon (http://www.icculus.org/~phaethon/q3mc/q3vm_specs.html)
    Code3Arena (http://www.planetquake.com/code3arena/)

*/

/* $Id: main.h,v 1.10 2004/08/30 00:45:36 cybermind Exp $ */

#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef WIN32
 #define DLLEXPORT __declspec(dllexport)
 #ifdef linux
  #undef linux
 #endif
#else
 #define DLLEXPORT /* */
#endif
#define C_DLLEXPORT extern "C" DLLEXPORT

typedef int (QDECL *syscall_t)(int, ...);

typedef int (*pfn_t)(int, int, int, int, int, int, int, int, int, int, int, int);

extern syscall_t g_syscall;
extern vm_t g_VM;
extern FILE* g_df;

extern char qmmvm_vmpath[MAX_QPATH], qmmvm_vmbase[16];
extern int qmmvm_debug, qmmvm_stacksize;

C_DLLEXPORT int vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11);
C_DLLEXPORT void dllEntry(syscall_t psyscall);
int QDECL VM_SysCalls(byte *memoryBase, int cmd, int *args);

#endif
