/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __UTIL_H__
#define __UTIL_H__

const char* get_modulename();
const char* get_modulepath();
void* get_modulehandle();
//int ismoduleloaded(void*);
int ismoduleloaded(void*);
void setmoduleloaded(void*);
void setmoduleunloaded(void*);
int byteswap(int);
short byteswap(short);
char* vaf(char*, ...);
//const char* opcodename(int);
extern const char* opcodename[];
int get_int_cvar(const char*);
const char* get_str_cvar(const char*);
void log_set(int);
int log_get();
int log_write(const char*, int = -1);
const char* my_strcasestr(const char*, const char*);
int isnumber(const char*);
int write_file(const char*, const char* = NULL);

#endif //__UTIL_H__
