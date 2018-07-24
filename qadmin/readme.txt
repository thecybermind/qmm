QAdmin - Administration Plugin
Copyright QMM Team 2005
http://www.q3mm.org/

QAdmin is an administrative plugin for Quake 3-based games and mods. Commands are spread out over numerous
access levels for better distribution of powers to users.

Created By:
    Kevin Masterson a.k.a. CyberMind <kevinm@planetquake.com>

Win32 Installation:
	1. Install QMM
	2. Make a qmmaddons directory inside your mod directory if it does not exist already
	3. Unzip into the qmmaddons directory.
	4. Add "qmmaddons/qadmin/dlls/qadmin_qmm.dll" as an entry in the plugins list in qmm.ini

Linux Installation:
	1. Install QMM
	2. Make a qmmaddons directory inside your mod directory if it does not exist already
	3. Untar into the qmmaddons directory.
	4. Add "qmmaddons/qadmin/dlls/qadmin_qmm.dll" as an entry in the plugins list in qmm.ini

Setup:

	Cvars:
	* admin_version - The current QAdmin version (Read-only)
	* admin_default_access - Default access level of all non-authed users (default is 1)
	* admin_vote_kick_time - Length of time a kick vote lasts in seconds (default is 60)
	* admin_vote_map_time - Length of time a map vote lasts in seconds (default is 60)
	* admin_config_file - Path (relative to mod directory) of the QAdmin config file (default is "qmmaddons/qadmin/config/qadmin.cfg")
	* admin_gagged_cmds - Comma-separated list of commands that should not be allowed by gagged users, "say" is automatically blocked (default is "say_team,tell,vsay,vsay_team,vtell,vosay,vosay_team,votell,vtaunt")

	Server Commands:
	* admin_adduser_name "name" "password" access - Adds a new user by name
	* admin_adduser_guid "guid" "password" access - Adds a new user by Punkbuster GUID
	* admin_adduser_ip "ip" "password" access - Adds a new user by IP
	* admin_cmd <command> - Executes the given QAdmin client command
	* a_c <command> - Executes the given QAdmin client command

	Client Commands:
	When adding a new user, the access level should be the total of all the desired access levels:
	For a user to get access, he must use admin_login (see below for more information).
	
	Access Level 0:
	* admin_currentmap      admin_currentmap - Displays current map
	* admin_help            admin_help [start] - Displays commands you have access to
	* admin_listmaps        admin_listmaps - Lists all maps on the server
	* admin_login           admin_login <pass> - Logs you in to get access
	* admin_timeleft        admin_timeleft - Displays the time left on this map
	* admin_userlist        admin_userlist [name] - Lists all users on the server that match 'name'
	
	Access Level 1:
	* admin_vote_kick       admin_vote_kick <user> - Initiates a vote to kick the user
	* admin_vote_map        admin_vote_map <map> - Initiates a vote to change to the map
	* castvote              castvote <option> - Places a vote for the given option
	
	Access Level 2:
	* admin_fraglimit       admin_fraglimit <value> - Sets the server's fraglimit
	* admin_timelimit       admin_timelimit <value> - Sets the server's timelimit
	* admin_vote_abort      admin_vote_abort - Aborts the current map or kick vote
	
	Access Level 4:
	* admin_reload          admin_reload - Reloads various QAdmin configs and cvars

	Access Level 8:
	* admin_map             admin_map <map> - Changes to the given map

	Access Level 16:
	* admin_nopass          admin_nopass - Clears the server password
	* admin_pass            admin_pass <password> - Changes the server password

	Access Level 32:
	* admin_friendlyfire    admin_friendlyfire <value> - Sets the server's friendlyfire
	* admin_gametype        admin_gametype <value> - Sets the server's gametype
	* admin_gravity         admin_gravity <value> - Sets the server's gravity

	Access Level 64:
	* admin_chat            admin_chat <text> - Sends the message to all admins with admin_chat access
	* admin_csay            admin_csay <text> - Displays message to all players in center of screen
	* admin_psay            admin_psay <name> <text> - Sends the message to specified player
	* admin_say             admin_say <text> - Sends the message to all players

	Access Level 128:
	* admin_kick            admin_kick <name> [message] - Kicks name from the server
	* admin_kickslot        admin_kickslot <index> [message] - Kicks user with given slot from the server
	
	Access Level 256:
	* admin_ban             admin_ban <name> [message] - Bans the specified user by IP
	* admin_banip           admin_banip <ip> [message] - Bans the specified IP
	* admin_banslot         admin_banslot <index> [message] - Bans the user in the specified slot
	* admin_unban           admin_unban <ip> - Unbans the specified IP
	
	Access Level 512:
	* admin_cfg             admin_cfg <file.cfg> - Executes the given .cfg file on the server
	* admin_hostname        admin_hostname <new name> - Sets the server's hostname

	Access Level 1024:
	* immunity from commands

	Access Level 2048:
	* admin_gag             admin_gag <name> - Gags the specified player from speaking
	* admin_gag             admin_gagslot <index> - Gags the player in the specified slot from speaking
	* admin_ungag           admin_ungag <name> - Ungags the specified player
	* admin_ungagslot       admin_ungagslot <index> - Gags the player in the specified slot
	
	Access Level 65536:
	* admin_rcon            admin_rcon <command> - Executes the command on the server

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
