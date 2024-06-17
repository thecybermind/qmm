/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

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
