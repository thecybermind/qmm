/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

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
