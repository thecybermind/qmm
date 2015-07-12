/*

QADMIN_QMM - QMM Administration Plugin
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

*/

/* $Id: main.h,v 1.5 2006/03/08 08:25:01 cybermind Exp $ */

#ifndef __MAIN_H__
#define __MAIN_H__

#define MAX_STRING_LENGTH 1024
#define MAX_DATA_LENGTH 200
#define MAX_COMMAND_LENGTH 64
#define MAX_NUMBER_LENGTH 11

#define MAX_USER_ENTRIES 64

//MAX_NETNAME = (35 + 1)
#define MAX_USER_LENGTH MAX_NETNAME
#define MAX_GUID_LENGTH 32 + 1
#define MAX_PASS_LENGTH 32 + 1
#define MAX_IP_LENGTH 15 + 1

#define SERVER_CONSOLE -2

typedef enum addusertype_e {
	au_ip = 1,
	au_name = 2,
	au_id = 3
} addusertype_t;

#ifdef WIN32
 #define strcasecmp stricmp
#endif

#define LEVEL_0		0
#define LEVEL_1		(1<<0)
#define LEVEL_2		(1<<1)
#define LEVEL_4		(1<<2)
#define LEVEL_8		(1<<3)
#define LEVEL_16	(1<<4)
#define LEVEL_32	(1<<5)
#define LEVEL_64	(1<<6)
#define LEVEL_128	(1<<7)
#define LEVEL_256	(1<<8)
#define LEVEL_512	(1<<9)
#define LEVEL_1024	(1<<10)
#define LEVEL_2048	(1<<11)
#define LEVEL_4096	(1<<12)
#define LEVEL_8192	(1<<13)
#define LEVEL_16384	(1<<14)
#define LEVEL_32768	(1<<15)
#define LEVEL_65536	(1<<16)
#define LEVEL_131072	(1<<17)

#define ACCESS_IMMUNITY	LEVEL_1024

typedef int (*pfnAdminCmd)(int,int,int);		//signature of a command handler

//command handler info
typedef struct admincmd_s {
	char* cmd;
	pfnAdminCmd func;
	int reqaccess;
	int minargs;
	char* usage;
	char* help;
} admincmd_t;

extern admincmd_t g_admincmds[];
extern admincmd_t g_saycmds[];

typedef struct playerinfo_s {
	char guid[MAX_GUID_LENGTH];
	char ip[MAX_IP_LENGTH];
	char name[MAX_NETNAME];
	char stripname[MAX_NETNAME];
	int access;
	bool authed;
	bool gagged;
	bool connected;
} playerinfo_t;
extern playerinfo_t g_playerinfo[];

typedef struct userinfo_s {
	char user[MAX_USER_LENGTH];
	char pass[MAX_PASS_LENGTH];
	int access;
	addusertype_t type;
} userinfo_t;
#define usertype(x) (x==au_ip?"IP":(x==au_name?"name":"ID"))

extern userinfo_t g_userinfo[];
extern int g_maxuserinfo;

extern int g_defaultAccess;

extern time_t g_mapstart;
extern int g_levelTime;

extern char** g_gaggedCmds;

void reload();
int handlecommand(int,int);
int admin_adduser(addusertype_t type);

#endif //__MAIN_H__
