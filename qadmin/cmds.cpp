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

/* $Id: cmds.cpp,v 1.6 2006/03/08 08:25:01 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_local.h>
#include <qmmapi.h>
#include <time.h>
#include "main.h"
#include "vote.h"
#include "util.h"

void reload() {
	//erase all user entries and re-exec the config file
	g_maxuserinfo = 0;
	g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("exec %s\n", QMM_GETSTRCVAR("admin_config_file")));

	//grab new default access value
	g_defaultAccess = g_syscall(G_CVAR_VARIABLE_INTEGER_VALUE, "admin_default_access");

	//reassign default access
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (!g_playerinfo[i].authed)
			g_playerinfo[i].access = g_defaultAccess;
	}

	//deallocate previous gagged command array
	tok_free(g_gaggedCmds);

	//refresh gagged command list
	const char* gagcmds = QMM_GETSTRCVAR("admin_gagged_cmds");
	if (gagcmds && *gagcmds)
		g_gaggedCmds = tok_parse(gagcmds, ',');

	g_syscall(G_PRINT, "[QADMIN] Configs/cvars (re)loaded\n");
}

//server command to add a new user
int admin_adduser(addusertype_t type) {
	if (g_syscall(G_ARGC) < 4) {
		g_syscall(G_PRINT, QMM_VARARGS("[QADMIN] Not enough parameters for admin_adduser_%s (<name|ip|id> <pass> <access>)\n", usertype(type)));
		QMM_RET_SUPERCEDE(1);
	}
	char user[MAX_USER_LENGTH], pass[MAX_PASS_LENGTH], access[MAX_NUMBER_LENGTH];
	g_syscall(G_ARGV, 1, user, sizeof(user));
	g_syscall(G_ARGV, 2, pass, sizeof(pass));
	g_syscall(G_ARGV, 3, access, sizeof(access));

	int i = 0;
	while (i <= g_maxuserinfo) {
		if (!strcmp(user, g_userinfo[i].user)) {
			g_syscall(G_PRINT, QMM_VARARGS("[QADMIN] User %s entry already exists for \"%s\"\n", usertype(type), user));
			QMM_RET_SUPERCEDE(1);
		}
		++i;
	}
	if (++g_maxuserinfo >= MAX_USER_ENTRIES) {
		g_syscall(G_PRINT, QMM_VARARGS("[QADMIN] Maximum user entries reached, ignoring new user %s \"%s\"\n", usertype(type), user));
		QMM_RET_SUPERCEDE(1);
	}

	g_userinfo[g_maxuserinfo].type = type;
	g_userinfo[g_maxuserinfo].access = atoi(access);
	strncpy(g_userinfo[g_maxuserinfo].pass, pass, sizeof(g_userinfo[g_maxuserinfo].pass));
	strncpy(g_userinfo[g_maxuserinfo].user, user, sizeof(g_userinfo[g_maxuserinfo].user));

	g_syscall(G_PRINT, QMM_VARARGS("[QADMIN] New user %s entry added for \"%s\" (access=%d)\n", usertype(g_userinfo[g_maxuserinfo].type), g_userinfo[g_maxuserinfo].user, g_userinfo[g_maxuserinfo].access));

	QMM_RET_SUPERCEDE(1);
}

//main command handler
//clientnum = client that did the command
//cmdnum = value to pass to ARGV to get the command
int handlecommand(int clientnum, int cmdnum) {
	int i = 0;
	char command[MAX_COMMAND_LENGTH];
	g_syscall(G_ARGV, cmdnum, command, sizeof(command));

	//loop through all registered commands
	while(g_admincmds[i].cmd && g_admincmds[i].func) {

		//if we found the command
		if (!strcasecmp(g_admincmds[i].cmd, command)) {

			//if the client has access, run handler func (get return value, func will set result flag)
			if (has_access(clientnum, g_admincmds[i].reqaccess)) {
				//only run handler func if we provided enough args
				//otherwise, show the help entry
				if (g_syscall(G_ARGC) < (g_admincmds[i].minargs + 1)) {
					ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Missing parameters, usage: %s\n", g_admincmds[i].usage));
					QMM_RET_SUPERCEDE(1);
				} else
					return (g_admincmds[i].func)(clientnum, cmdnum + 1, g_admincmds[i].reqaccess);
			}

			//if client doesn't have access, give warning message
			g_syscall(G_SEND_SERVER_COMMAND, clientnum, QMM_VARARGS("print \"[QADMIN] You do not have access to that command: '%s'\n\"", command));
			QMM_RET_SUPERCEDE(1);
		}

		++i;
	}

	//check for gagged commands (but only for gagged users)
	if (g_playerinfo[clientnum].gagged) {
		i = 0;
		while(g_gaggedCmds[i]) {
			if (!strcasecmp(command, g_gaggedCmds[i])) {
				ClientPrint(clientnum, "[QADMIN] Sorry, you have been gagged.\n");
				QMM_RET_SUPERCEDE(1);
			}
			++i;
		}
	}

	if (clientnum == SERVER_CONSOLE) {
		QMM_RET_SUPERCEDE(1);
	} else {
		QMM_RET_IGNORED(0);
	}
}

//clientnum = client that did the command
//datanum = value to pass to ARGV to get the first data arg
//access = access required to run this command
int admin_help(int clientnum, int datanum, int access) {
	char num[MAX_NUMBER_LENGTH];
	g_syscall(G_ARGV, datanum, num, sizeof(num));
	int start = atoi(num);
	if (start <= 0 || start > (arrsize(g_admincmds) + arrsize(g_saycmds)))
		start = 1;
	
	ClientPrint(clientnum, QMM_VARARGS("[QADMIN] admin_help listing for %d-%d\n", start, start+9));

	//i is the command array index
	int i = 0;
	//j is the counter for valid commands
	int j = 1;
	//k is the display counter
	int k = start;

	//loop until we run out of commands
	while (g_admincmds[i].cmd) {
		//make sure the command help exists and the user has enough access
		if (g_admincmds[i].usage && g_admincmds[i].usage[0] && g_admincmds[i].help && g_admincmds[i].help[0] && has_access(clientnum, g_admincmds[i].reqaccess)) {
			//if the command is within our display range
			if (j >= start && j <= (start+9)) {
				ClientPrint(clientnum, QMM_VARARGS("[QADMIN] %d. %s - %s\n", k, g_admincmds[i].usage, g_admincmds[i].help));
				++k;
			}
			++j;
		}
		++i;
	}

	QMM_RET_SUPERCEDE(1);
}

int admin_login(int clientnum, int datanum, int access) {
	if (clientnum == SERVER_CONSOLE) {
		ClientPrint(clientnum, "[QADMIN] Trying to login from the server console, eh?\n");
		QMM_RET_SUPERCEDE(1);
	}

	if (g_playerinfo[clientnum].authed) {
		ClientPrint(clientnum, "[QADMIN] Trying to login multiple times, eh?\n");
		QMM_RET_SUPERCEDE(1);
	}

	char password[MAX_DATA_LENGTH];
	
	//if the user logged in with command
	if (datanum > 0) {
		g_syscall(G_ARGV, datanum, password, sizeof(password));
	
	//if the user logged in by saying admin_login, use tok_parse
	} else {
		g_syscall(G_ARGV, -datanum, password, sizeof(password));
		char** array = tok_parse(password);
		strncpy(password, array[1], sizeof(password));
		tok_free(array);
	}

	userinfo_t* info = NULL;
	int i = 0;
	while (i <= g_maxuserinfo) {
		info = &g_userinfo[i];

		if (!strcmp(info->user, (info->type==au_name?g_playerinfo[clientnum].name:(info->type==au_ip?g_playerinfo[clientnum].ip:g_playerinfo[clientnum].guid))) && !strcmp(info->pass, password)) {
			g_playerinfo[clientnum].access = info->access | g_defaultAccess;
			g_playerinfo[clientnum].authed = 1;
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] You have successfully authenticated. You now have %d access.\n", g_playerinfo[clientnum].access));
			QMM_RET_SUPERCEDE(1);
		}
		++i;
	}

	QMM_RET_SUPERCEDE(1);
}

/*int admin_ban(int clientnum, int datanum, int access) {
	char bancmd[MAX_COMMAND_LENGTH], user[MAX_USER_LENGTH];
	g_syscall(G_ARGV, datanum - 1, bancmd, sizeof(bancmd));
	g_syscall(G_ARGV, datanum, user, sizeof(user));

	char* message = concatargs(datanum+1);

	//if we are banning a player name, grab his ID or IP and then store in the arg strings
	//so the admin_banid/admin_banip checks will do the actual work
	if (!strcmp(bancmd, "admin_ban")) {
		int slotid = namematch(user);
		if (slotid < 0) {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Ambiguous match or match not found for '%s'\n", user));
			QMM_RET_SUPERCEDE(1);
		}

		char guidban[MAX_NUMBER_LENGTH];

		if (g_syscall(G_ARGC) > 2)
			g_syscall(G_ARGV, datanum + 1, guidban, sizeof(guidban));

		//'guid' was given
		if (g_syscall(G_ARGC) > 2 && !strcmp(guidban, "guid")) {
			strncpy(user, g_playerinfo[slotid].guid, sizeof(user));
			strncpy(bancmd, "admin_banid", sizeof(bancmd));
			message = concatargs(datanum+2);
		} else {
			strncpy(user, g_playerinfo[slotid].name, sizeof(user));
			strncpy(bancmd, "admin_banip", sizeof(bancmd));
		}
		g_syscall(G_DROP_CLIENT, slotid, message);
	}

	if (!strcmp(bancmd, "admin_banid")) {
		g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("addid %s \"%s\"\n", user, message));
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Banned ID %s\n", user));
	} else if (!strcmp(bancmd, "admin_banip")) {
		g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("addip %s \"%s\"\n", user, message));
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Banned IP %s\n", user));
	}
	

	QMM_RET_SUPERCEDE(1);
}

//TODO: addid and removeid commands to emulate addip/removeip for GUIDs
int admin_unban(int clientnum, int datanum, int access) {
	char unbancmd[MAX_COMMAND_LENGTH], user[MAX_USER_LENGTH];
	g_syscall(G_ARGV, datanum - 1, unbancmd, sizeof(unbancmd));
	g_syscall(G_ARGV, datanum, user, sizeof(user));

	//if the user used admin_unban, autodetect the usertype by checking for "."
	if (!strcmp(unbancmd, "admin_unban")) {
		if (strstr(user, "."))
			strncpy(unbancmd, "admin_unbanip", sizeof(unbancmd));
		else
			strncpy(unbancmd, "admin_unbanid", sizeof(unbancmd));
	}

	if (!strcmp(unbancmd, "admin_unbanid")) {
		g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("removeid %s\n", user));
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Unbanned ID %s\n", user));
	} else if (!strcmp(unbancmd, "admin_unbanip")) {
		g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("removeip %s\n", user));
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Unbanned IP %s\n", user));
	}

	QMM_RET_SUPERCEDE(1);
}
*/

int admin_ban(int clientnum, int datanum, int access) {
	char bancmd[MAX_COMMAND_LENGTH], user[MAX_USER_LENGTH];
	g_syscall(G_ARGV, datanum - 1, bancmd, sizeof(bancmd));
	g_syscall(G_ARGV, datanum, user, sizeof(user));

	char* message = concatargs(datanum+1);
	if (!*message)
		message = "Banned by Admin";

	if (!strcasecmp(bancmd, "admin_ban") || !strcasecmp(bancmd, "admin_banslot")) {
		int slotid;
		if (!strcasecmp(bancmd, "admin_ban")) {
			slotid = namematch(user);
			if (slotid < 0) {
				ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Ambiguous match or match not found for '%s'\n", user));
				QMM_RET_SUPERCEDE(1);
			}
		} else {
			slotid = atoi(user);
			if (slotid < 0 || slotid >= MAX_CLIENTS || !g_playerinfo[slotid].connected) {
				ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Invalid slot id '%d'\n", slotid));
				QMM_RET_SUPERCEDE(1);
			}
		}
		
		//check if the desired user has immunity
		if (has_access(slotid, ACCESS_IMMUNITY)) {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Cannot ban %s, user has immunity.\n", g_playerinfo[slotid].name));
			QMM_RET_SUPERCEDE(1);
		}

		//check if other users have the same IP as the desired user
		bool immunity = 0;
		
		//find users who have the given IP
		int finduser = user_with_ip(g_playerinfo[slotid].ip);

		//loop until no more users have the IP
		while (finduser != -1) {
			//if this user has immunity, set the flag
			if (has_access(finduser, ACCESS_IMMUNITY)) {
				immunity = 1;
				break;
			}

			//get next user with IP
			finduser = user_with_ip(g_playerinfo[slotid].ip, finduser);
		}

		//if no users with immunity have the IP, ban the IP and kick the user
		if (!immunity) {
			g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("addip %s \"%s\"\n", g_playerinfo[slotid].ip, message));
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Banned %s by IP (%s): '%s'\n", g_playerinfo[slotid].name, g_playerinfo[slotid].ip, message));
			g_syscall(G_DROP_CLIENT, slotid, message);
		}

		//else at least 1 user with immunity has the given IP
		else {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Cannot ban %s by IP, another user with that IP (%s) has immunity.\n", g_playerinfo[slotid].name, g_playerinfo[slotid].ip));
		}

		//but still kick the users on the IP without immunity
		finduser = user_with_ip(g_playerinfo[slotid].ip);

		//loop until no more users have the IP
		while (finduser != -1) {
			//if this user does not have immunity, kick him
			if (!has_access(finduser, ACCESS_IMMUNITY))
				g_syscall(G_DROP_CLIENT, finduser, message);

			//get next user with IP
			finduser = user_with_ip(g_playerinfo[slotid].ip, finduser);
		}

	} else if (!strcasecmp(bancmd, "admin_banip")) {
		bool immunity = 0;
		
		//find if any users who have the given IP are immune
		int finduser = user_with_ip(user);

		//loop until no more users have the IP
		while (finduser != -1) {
			//if this user has immunity, set the flag
			if (has_access(finduser, ACCESS_IMMUNITY)) {
				immunity = 1;
				break;
			}

			//get next user with IP
			finduser = user_with_ip(user, finduser);
		}
		
		//if no users with immunity have the IP, ban the IP
		if (!immunity) {
			g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("addip %s \"%s\"\n", user, message));
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Banned IP %s: '%s'\n", user, message));
		}
		
		//else at least 1 user with immunity has the given IP
		else {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Cannot ban IP %s, a user with that IP has immunity.\n", user));
		}

		//but still kick the users without immunity
		finduser = user_with_ip(user);

		//loop until no more users have the IP
		while (finduser != -1) {
			//if this user does not have immunity, kick him
			if (!has_access(finduser, ACCESS_IMMUNITY))
				g_syscall(G_DROP_CLIENT, finduser, message);

			//get next user with IP
			finduser = user_with_ip(user, finduser);
		}
	
	}	

	QMM_RET_SUPERCEDE(1);
}

int admin_unban(int clientnum, int datanum, int access) {
	char ip[MAX_USER_LENGTH];
	g_syscall(G_ARGV, datanum, ip, sizeof(ip));

	g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("removeip %s\n", ip));
	ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Unbanned IP %s\n", ip));

	QMM_RET_SUPERCEDE(1);
}

int admin_cfg(int clientnum, int datanum, int access) {
	char file[MAX_QPATH];
	g_syscall(G_ARGV, datanum, file, sizeof(file));
	g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("exec %s\n", file));

	QMM_RET_SUPERCEDE(1);
}

int admin_rcon(int clientnum, int datanum, int access) {
	g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("%s\n", concatargs(datanum)));

	QMM_RET_SUPERCEDE(1);
}

int admin_hostname(int clientnum, int datanum, int access) {
	setcvar("sv_hostname", datanum);

	QMM_RET_SUPERCEDE(1);
}

int admin_friendlyfire(int clientnum, int datanum, int access) {
	setcvar("g_friendlyfire", datanum);

	QMM_RET_SUPERCEDE(1);
}

int admin_gravity(int clientnum, int datanum, int access) {
	setcvar("g_gravity", datanum);

	QMM_RET_SUPERCEDE(1);
}

int admin_gametype(int clientnum, int datanum, int access) {
	setcvar("g_gametype", datanum);

	QMM_RET_SUPERCEDE(1);
}

int admin_map(int clientnum, int datanum, int access) {
	char map[MAX_QPATH];
	g_syscall(G_ARGV, datanum, map, sizeof(map));
	
	g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("map %s\n", map));

	QMM_RET_SUPERCEDE(1);
}

int admin_fraglimit(int clientnum, int datanum, int access) {
	setcvar("fraglimit", datanum);

	QMM_RET_SUPERCEDE(1);
}

int admin_timelimit(int clientnum, int datanum, int access) {
	setcvar("timelimit", datanum);

	QMM_RET_SUPERCEDE(1);
}

int admin_pass(int clientnum, int datanum, int access) {
	char command[MAX_COMMAND_LENGTH];
	g_syscall(G_ARGV, datanum-1, command, sizeof(command));

	if (!strcmp(command, "admin_pass")) {
		setcvar("g_password", datanum);
		g_syscall(G_CVAR_SET, "g_needpass", "1");
	} else if (!strcmp(command, "admin_nopass")) {
		g_syscall(G_CVAR_SET, "g_needpass", "0");
	}

	QMM_RET_SUPERCEDE(1);
}

int admin_chat(int clientnum, int datanum, int access) {
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (has_access(i, access))
			ClientPrint(i, QMM_VARARGS("To Admins From %s: %s", clientnum == SERVER_CONSOLE ? "Console" : g_playerinfo[clientnum].name, concatargs(datanum)), 1);
	}

	QMM_RET_SUPERCEDE(1);
}

int admin_csay(int clientnum, int datanum, int access) {
	g_syscall(G_SEND_SERVER_COMMAND, -1, QMM_VARARGS("cp \"%s\n\"", concatargs(datanum)));

	QMM_RET_SUPERCEDE(1);
}

int admin_say(int clientnum, int datanum, int access) {
	ClientPrint(-1, QMM_VARARGS("%s", concatargs(datanum)), 1);

	QMM_RET_SUPERCEDE(1);
}

int admin_psay(int clientnum, int datanum, int access) {
	char user[MAX_NAME_LENGTH];
	g_syscall(G_ARGV, datanum, user, sizeof(user));
	int slotid = namematch(user);

	if (slotid < 0) {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Ambiguous match or match not found for '%s'\n", user));
		QMM_RET_SUPERCEDE(1);
	}

	char* text = concatargs(datanum+1);
	
	ClientPrint(clientnum, QMM_VARARGS("Private Message To %s: %s", g_playerinfo[slotid].name, text), 1);
	ClientPrint(slotid, QMM_VARARGS("Private Message From %s: %s", clientnum == SERVER_CONSOLE ? "Console" : g_playerinfo[clientnum].name, text), 1);

	QMM_RET_SUPERCEDE(1);
}

int admin_listmaps(int clientnum, int datanum, int access) {
	char dirlist[MAX_STRING_LENGTH];
	char* dirptr = dirlist;

	int numfiles = g_syscall(G_FS_GETFILELIST, "maps", ".bsp", dirlist, sizeof(dirlist));

	ClientPrint(clientnum, "[QADMIN] Listing maps...\n");
	for (int i = 1; i < numfiles; ++i) {
		dirptr += strlen(dirptr);
		*dirptr = '\n';
	}
	ClientPrint(clientnum, dirlist);
	ClientPrint(clientnum, "\n[QADMIN] End of maps list\n");

	QMM_RET_SUPERCEDE(1);
}

int admin_kick(int clientnum, int datanum, int access) {
	char name[MAX_NAME_LENGTH];
	char kickcmd[MAX_COMMAND_LENGTH];
	int slotid = 0;
	g_syscall(G_ARGV, datanum - 1, kickcmd, sizeof(kickcmd));
	g_syscall(G_ARGV, datanum, name, sizeof(name));

	if (!strcasecmp(kickcmd, "admin_kick")) {
		slotid = namematch(name);
		if (slotid < 0) {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Ambiguous match or match not found for '%s'\n", name));
			QMM_RET_SUPERCEDE(1);
		}
	} else if (!strcasecmp(kickcmd, "admin_kickslot")) {
		slotid = atoi(name);
		if (slotid < 0 || slotid >= MAX_CLIENTS || !g_playerinfo[slotid].connected) {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Invalid slot id '%d'\n", slotid));
			QMM_RET_SUPERCEDE(1);
		}
	}

	if (has_access(slotid, ACCESS_IMMUNITY)) {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Cannot kick %s, user has immunity\n", g_playerinfo[slotid].name));
		QMM_RET_SUPERCEDE(1);
	}
	
	char* message = concatargs(datanum+1);
	if (!*message)
		message = "Kicked by Admin";
	
	ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Kicked %s: '%s'\n", g_playerinfo[slotid].name, message));
	g_syscall(G_DROP_CLIENT, slotid, message);

	QMM_RET_SUPERCEDE(1);
}

int admin_reload(int clientnum, int datanum, int access) {
	reload();

	QMM_RET_SUPERCEDE(1);
}

int admin_userlist(int clientnum, int datanum, int access) {
	int argc = g_syscall(G_ARGC) - datanum + 1;
	char name[MAX_NAME_LENGTH];

	int banaccess = has_access(clientnum, LEVEL_256);

	//if a parameter was given, only display users matching it
	if (argc >= 2) {
		g_syscall(G_ARGV, datanum, name, sizeof(name));
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Listing users matching '%s'...\n", name));
		if (banaccess)
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Slot Access   Authed %-*s Name\n", sizeof(g_playerinfo[0].ip), "IP"));
		else
			ClientPrint(clientnum, "[QADMIN] Slot Access   Authed Name\n");

		//loop through every user that matches the given string
		//(the 1 passed to namematch means to not check ambiguity)
		int i = namematch(name, 1);
		while (i != -1) {
			if (banaccess)
				ClientPrint(clientnum, QMM_VARARGS("[QADMIN] %3d: %-8d %s %-*s %s", i, g_playerinfo[i].access, g_playerinfo[i].authed ? "yes   " : "no    ", sizeof(g_playerinfo[0].ip), g_playerinfo[i].ip, g_playerinfo[i].name));
			else
				ClientPrint(clientnum, QMM_VARARGS("[QADMIN] %3d: %-8d %s %s\n", i, g_playerinfo[i].access, g_playerinfo[i].authed ? "yes   " : "no    ", g_playerinfo[i].name));
			i = namematch(name, 1, i);
		}
	} else {
		ClientPrint(clientnum, "[QADMIN] Listing users...\n");
		if (banaccess)
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Slot Access   Authed %-*s Name\n", sizeof(g_playerinfo[0].ip), "IP"));
		else
			ClientPrint(clientnum, "[QADMIN] Slot Access   Authed Name\n");

		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (!g_playerinfo[i].connected)
				continue;

			if (banaccess)
				ClientPrint(clientnum, QMM_VARARGS("[QADMIN] %3d: %-8d %s %-*s %s", i, g_playerinfo[i].access, g_playerinfo[i].authed ? "yes   " : "no    ", sizeof(g_playerinfo[0].ip), g_playerinfo[i].ip, g_playerinfo[i].name));
			else
				ClientPrint(clientnum, QMM_VARARGS("[QADMIN] %3d: %-8d %s %s\n", i, g_playerinfo[i].access, g_playerinfo[i].authed ? "yes   " : "no    ", g_playerinfo[i].name));
		}
	}

	QMM_RET_SUPERCEDE(1);
}

int admin_gag(int clientnum, int datanum, int access) {
	char gagcmd[MAX_COMMAND_LENGTH], name[MAX_NAME_LENGTH];
	int slotid = 0;

	g_syscall(G_ARGV, datanum - 1, gagcmd, sizeof(gagcmd));
	g_syscall(G_ARGV, datanum, name, sizeof(name));
	
	if (!strcasecmp(gagcmd, "admin_gag")) {
		slotid = namematch(name);
		if (slotid < 0) {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Ambiguous match or match not found for '%s'\n", name));
			QMM_RET_SUPERCEDE(1);
		}
	} else if (!strcasecmp(gagcmd, "admin_gagslot")) {
		slotid = atoi(name);
		if (slotid < 0 || slotid >= MAX_CLIENTS || !g_playerinfo[slotid].connected) {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Invalid slot id '%d'\n", slotid));
			QMM_RET_SUPERCEDE(1);
		}
	}

	if (has_access(slotid, ACCESS_IMMUNITY)) {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Cannot gag %s, user has immunity\n", g_playerinfo[slotid].name));
		QMM_RET_SUPERCEDE(1);
	}

	if (g_playerinfo[slotid].gagged) {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] %s is already gagged\n", g_playerinfo[slotid].name));
	} else {
		g_playerinfo[slotid].gagged = 1;
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] %s has been gagged\n", g_playerinfo[slotid].name));
	}

	QMM_RET_SUPERCEDE(1);
}

int admin_ungag(int clientnum, int datanum, int access) {
	char ungagcmd[MAX_NAME_LENGTH], name[MAX_NAME_LENGTH];
	int slotid = 0;

	g_syscall(G_ARGV, datanum - 1, ungagcmd, sizeof(ungagcmd));
	g_syscall(G_ARGV, datanum, name, sizeof(name));
	if (!strcasecmp(ungagcmd, "admin_ungag")) {
		slotid = namematch(name);
		if (slotid < 0) {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Ambiguous match or match not found for '%s'\n", name));
			QMM_RET_SUPERCEDE(1);
		}
	} else if (!strcasecmp(ungagcmd, "admin_ungagslot")) {
		slotid = atoi(name);
		if (slotid < 0 || slotid >= MAX_CLIENTS || !g_playerinfo[slotid].connected) {
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Invalid slot id '%d'\n", slotid));
			QMM_RET_SUPERCEDE(1);
		}
	}

	if (g_playerinfo[slotid].gagged) {
		g_playerinfo[slotid].gagged = 0;
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] %s has been ungagged\n", g_playerinfo[slotid].name));
	} else {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] %s is not gagged\n", g_playerinfo[slotid].name));
	}

	QMM_RET_SUPERCEDE(1);
}

int admin_currentmap(int clientnum, int datanum, int access) {
	ClientPrint(datanum > 0 ? clientnum : -1, QMM_VARARGS("[QADMIN] The current map is: %s\n", QMM_GETSTRCVAR("mapname")));
	QMM_RETURN(datanum > 0 ? QMM_SUPERCEDE : QMM_IGNORED, 1);
}

int admin_timeleft(int clientnum, int datanum, int access) {
	int timelimit = g_syscall(G_CVAR_VARIABLE_INTEGER_VALUE, "timelimit");
	if (!timelimit) {
		ClientPrint(datanum > 0 ? clientnum : -1, "[QADMIN] There is no time limit.\n");
		QMM_RETURN(datanum > 0 ? QMM_SUPERCEDE : QMM_IGNORED, 1);
	}
	time_t timeleft = (timelimit * 60) - (time(NULL) - g_mapstart);

	if (timeleft <= 0)
		ClientPrint(datanum > 0 ? clientnum : -1, "[QADMIN] Time limit has been reached\n");
	else
		ClientPrint(datanum > 0 ? clientnum : -1, QMM_VARARGS("[QADMIN] Time remaining: %lu minute(s) %lu second(s)\n", timeleft / 60, timeleft % 60));

	QMM_RETURN(datanum > 0 ? QMM_SUPERCEDE : QMM_IGNORED, 1);
}

void handle_vote_map(int winner, int winvotes, int totalvotes, void* param) {
	if (winner == 1) {
		ClientPrint(-1, QMM_VARARGS("[QADMIN] Vote to change map to %s was successful\n", param));
		g_syscall(G_SEND_CONSOLE_COMMAND, EXEC_APPEND, QMM_VARARGS("map %s\n", param));
	} else {
		ClientPrint(-1, QMM_VARARGS("[QADMIN] Vote to change map to %s has failed\n", param));
	}
}

int admin_vote_map(int clientnum, int datanum, int access) {
	//this is static so that it still exists when passed to handle_vote_map as param
	static char map[MAX_QPATH] = "";

	g_syscall(G_ARGV, datanum, map, sizeof(map));
	int time = g_syscall(G_CVAR_VARIABLE_INTEGER_VALUE, "admin_vote_map_time");

	if (!is_valid_map(map)) {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Unknown map '%s'\n", map));
		QMM_RET_SUPERCEDE(1);
	}

	ClientPrint(-1, QMM_VARARGS("[QADMIN] A %d second vote has been started to changed map to %s\n", time, map)); 
	ClientPrint(-1, "[QADMIN] Type 'castvote 1' for YES, or 'castvote 2' for NO\n");
	
	vote_start(clientnum, handle_vote_map, time, 2, (void*)map);

	QMM_RET_SUPERCEDE(1);
}

void handle_vote_kick(int winner, int winvotes, int totalvotes, void* param) {
	int slotid = (int)param;
	
	//user may have authed and gotten immunity during the vote,
	//but don't mention it, just make the vote fail
	if (has_access(slotid, ACCESS_IMMUNITY))
		winner = 0;

	if (winner == 1 && winvotes) {
		ClientPrint(-1, QMM_VARARGS("[QADMIN] Vote to kick %s was successful\n", g_playerinfo[slotid].name));
		g_syscall(G_DROP_CLIENT, slotid, "Kicked due to vote.");
	} else {
		ClientPrint(-1, QMM_VARARGS("[QADMIN] Vote to kick %s has failed\n", g_playerinfo[slotid].name));
	}
}

int admin_vote_kick(int clientnum, int datanum, int access) {
	char user[MAX_NAME_LENGTH];
	int slotid;
	int time = g_syscall(G_CVAR_VARIABLE_INTEGER_VALUE, "admin_vote_kick_time");

	g_syscall(G_ARGV, datanum, user, sizeof(user));
	slotid = namematch(user);
	if (slotid < 0) {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Ambiguous match or match not found for '%s'\n", user));
		QMM_RET_SUPERCEDE(1);
	}
	
	//cannot votekick a user with immunity
	//the user's immunity is also checked in the vote
	//handler in case he auths before the vote ends
	if (has_access(slotid, ACCESS_IMMUNITY)) {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Cannot kick %s, user has immunity\n", g_playerinfo[slotid].name));
		QMM_RET_SUPERCEDE(1);
	}

	ClientPrint(-1, QMM_VARARGS("[QADMIN] A %d second vote has been started to kick %s\n", time, g_playerinfo[slotid].name)); 
	ClientPrint(-1, "[QADMIN] Type 'castvote 1' for YES, or 'castvote 2' for NO\n");
	vote_start(clientnum, handle_vote_kick, time, 2, (void*)slotid);

	QMM_RET_SUPERCEDE(1);
}

int admin_vote_abort(int clientnum, int datanum, int access) {
	ClientPrint(-1, "[QADMIN] The current vote has been aborted\n");
	g_vote.inuse = 0;

	QMM_RET_SUPERCEDE(1);
}

//say handler (need to handle subcommands)
int say(int clientnum, int datanum, int access) {
	if (g_playerinfo[clientnum].gagged) {
		ClientPrint(clientnum, "[QADMIN] Sorry, you have been gagged.\n");
		QMM_RET_SUPERCEDE(1);
	}

	char command[MAX_COMMAND_LENGTH];

	int argcount = g_syscall(G_ARGC) - 1;

	int temp = 0;
	
	g_syscall(G_ARGV, datanum, command, sizeof(command));

	//all parameters are in one (surrounded by quotes)
	if (g_syscall(G_ARGC) <= 2) {
		char** args = tok_parse(command);
		strncpy(command, args[0], sizeof(command));
		for (argcount = 0; args[argcount]; ++argcount);
		tok_free(args);
		temp = 1;
	//if each parameter is separate, increase datanum
	} else {
		++datanum;
	}

	//loop through all registered "say" commands
	for (int i = 0; g_saycmds[i].cmd && g_saycmds[i].func; ++i) {

		//if we found the command
		if (!strcasecmp(g_saycmds[i].cmd, command)) {

			//if the client has access, run handler func (get return value, func will set result flag)
			//pass negative datanum to differentiate between say/console commands
			if (has_access(clientnum, g_saycmds[i].reqaccess)) {
				//only run handler func if we provided enough args
				if (argcount < (g_saycmds[i].minargs + 1))
					QMM_RET_IGNORED(0);
				else
					return (g_saycmds[i].func)(clientnum, (temp ? -datanum : datanum), g_saycmds[i].reqaccess);
			}

			//if client doesn't have access, give warning message
			ClientPrint(clientnum, QMM_VARARGS("[QADMIN] You do not have access to that command: '%s'\n", g_saycmds[i].cmd));

			QMM_RET_SUPERCEDE(1);
		}
	}

	QMM_RET_IGNORED(0);
}

int castvote(int clientnum, int datanum, int access) {
	if (clientnum == SERVER_CONSOLE) {
		ClientPrint(clientnum, "[QADMIN] Trying to vote from the server console, eh?\n");
		QMM_RET_SUPERCEDE(1);
	}

	char vote[MAX_DATA_LENGTH];
	
	//use G_ARGV to get params
	if (datanum > 0) {
		g_syscall(G_ARGV, datanum, vote, sizeof(vote));
		vote_add(clientnum, atoi(vote));
	
	//use tok_parse
	} else {
		g_syscall(G_ARGV, -datanum, vote, sizeof(vote));
		char** array = tok_parse(vote);
		vote_add(clientnum, atoi(array[1]));
		tok_free(array);
	}

	QMM_RET_SUPERCEDE(1);		
}

//register command handlers
//moved into alphabetical order to make admin_help a bit easier
admincmd_t g_admincmds[] = {
	{ "admin_ban",		admin_ban,		LEVEL_256,	1, "admin_ban <name> [message]", "Bans the specified user by IP" },
	{ "admin_banip",	admin_ban,		LEVEL_256,	1, "admin_banip <ip> [message]", "Bans the specified IP" },
	{ "admin_banslot",	admin_ban,		LEVEL_256,	1, "admin_banslot <index> [message]", "Bans the user in the specified slot" },
	{ "admin_cfg",		admin_cfg,		LEVEL_512,	1, "admin_cfg <file.cfg>", "Executes the given .cfg file on the server" },
	{ "admin_chat",		admin_chat,		LEVEL_64,	1, "admin_chat <text>", "Sends the message to all admins with admin_chat access" },
	{ "admin_csay",		admin_csay,		LEVEL_64,	1, "admin_csay <text>", "Displays message to all players in center of screen" },
	{ "admin_currentmap",	admin_currentmap,	LEVEL_0,	0, "admin_currentmap", "Displays current map" },
	{ "admin_fraglimit",	admin_fraglimit,	LEVEL_2,	1, "admin_fraglimit <value>", "Sets the server's fraglimit" },
	{ "admin_friendlyfire",	admin_friendlyfire,	LEVEL_32,	1, "admin_friendlyfire <value>", "Sets the server's friendlyfire" },
	{ "admin_gag",		admin_gag,		LEVEL_2048,	1, "admin_gag <name>", "Gags the specified player from speaking" },
	{ "admin_gagslot",	admin_gag,		LEVEL_2048,	1, "admin_gagslot <index>", "Gags the player in the specified slot from speaking" },
	{ "admin_gametype",	admin_gametype,		LEVEL_32,	1, "admin_gametype <value>", "Sets the server's gametype" },
	{ "admin_gravity",	admin_gravity,		LEVEL_32,	1, "admin_gravity <value>", "Sets the server's gravity" },
	{ "admin_help",		admin_help,		LEVEL_0,	0, "admin_help [start]", "Displays commands you have access to" },
	{ "admin_hostname",	admin_hostname,		LEVEL_512,	1, "admin_hostname <new name>", "Sets the server's hostname" },
	{ "admin_kick",		admin_kick,		LEVEL_128,	1, "admin_kick <name> [message]", "Kicks name from the server" },
	{ "admin_kickslot",	admin_kick,		LEVEL_128,	1, "admin_kickslot <index> [message]", "Kicks user with given slot from the server" },
	{ "admin_listmaps",	admin_listmaps,		LEVEL_0,	0, "admin_listmaps", "Lists all maps on the server" },
	{ "admin_login",	admin_login,		LEVEL_0,	1, "admin_login <pass>", "Logs you in to get access" },
	{ "admin_map",		admin_map,		LEVEL_8,	1, "admin_map <map>", "Changes to the given map" },
	{ "admin_pass",		admin_pass,		LEVEL_16,	1, "admin_pass <password>", "Changes the server password" },
	{ "admin_psay",		admin_psay,		LEVEL_64,	2, "admin_psay <name> <text>", "Sends the message to specified player" },
	{ "admin_nopass",	admin_pass,		LEVEL_16,	0, "admin_nopass", "Clears the server password" },
	{ "admin_rcon",		admin_rcon,		LEVEL_65536,	1, "admin_rcon <command>", "Executes the command on the server" },
	{ "admin_reload",	admin_reload,		LEVEL_4,	0, "admin_reload", "Reloads various QAdmin configs and cvars" },
	{ "admin_say",		admin_say,		LEVEL_64,	1, "admin_say <text>", "Sends the message to all players" },
	{ "admin_timeleft",	admin_timeleft,		LEVEL_0,	0, "admin_timeleft", "Displays the time left on this map" },
	{ "admin_timelimit",	admin_timelimit,	LEVEL_2,	1, "admin_timelimit <value>", "Sets the server's timelimit" },
	{ "admin_unban",	admin_unban,		LEVEL_256,	1, "admin_unban <ip>", "Unbans the specified IP" },
	{ "admin_ungag",	admin_ungag,		LEVEL_2048,	1, "admin_ungag <name>", "Ungags the specified player" },
	{ "admin_ungagslot",	admin_ungag,		LEVEL_2048,	1, "admin_ungagslot <index>", "Ungags the player in the specified slot" },
	{ "admin_userlist",	admin_userlist,		LEVEL_0,	0, "admin_userlist [name]", "Lists all users on the server that match 'name'" },
	{ "admin_vote_abort",	admin_vote_abort,	LEVEL_2,	1, "admin_vote_abort", "Aborts the current map or kick vote" },
	{ "admin_vote_kick",	admin_vote_kick,	LEVEL_1,	1, "admin_vote_kick <user>", "Initiates a vote to kick the user" },
	{ "admin_vote_map",	admin_vote_map,		LEVEL_1,	1, "admin_vote_map <map>", "Initiates a vote to change to the map" },
	{ "castvote",		castvote,		LEVEL_1,	1, "castvote <option>", "Places a vote for the given option" },

	{ "say",		say,			LEVEL_0,	0, NULL, NULL },

//	{ "admin_ban",		admin_ban,		LEVEL_256,	1, "admin_ban <name> ['guid'] [message]", "Bans the specified user by IP, unless 'id' is specified" },
//	{ "admin_banid",	admin_ban,		LEVEL_256,	1, "admin_banid <guid> [message]", "Bans the specified GUID" },
//	{ "admin_banip",	admin_ban,		LEVEL_256,	1, "admin_banip <ip> [message]", "Bans the specified IP" },
//	{ "admin_unban",	admin_unban,		LEVEL_256,	1, "admin_unban <guid|ip>", "Unbans the specified GUID or IP" },
//	{ "admin_unbanid",	admin_unban,		LEVEL_256,	1, "admin_unbanid <guid>", "Unbans the specified GUID" },
//	{ "admin_unbanip",	admin_unban,		LEVEL_256,	1, "admin_unbanip <ip>", "Unbans the specified IP" },

	{ NULL, NULL, 0, 0, NULL, NULL }
};
admincmd_t g_saycmds[] = {
	{ "admin_login",	admin_login,		LEVEL_0,	1, NULL, NULL },
	{ "castvote",		castvote,		LEVEL_1,	1, NULL, NULL },
	{ "currentmap",		admin_currentmap,	LEVEL_0,	0, NULL, NULL },
	{ "timeleft",		admin_timeleft,		LEVEL_0,	0, NULL, NULL },

	{ NULL, NULL, 0, 0, NULL, NULL }
};
