QMM - Q3 MultiMod
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
    Code3Arena (http://www.planetquake.com/code3arena/)

NOTE: Only follow the QMMVM installation instructions if you are using QMMVM by itself, apart from QMM.

If you are unable to determine what mod type you are running (DLL/SO or QVM), use the 'vminfo' server command.
It will list all the loaded mod files ("qagame", "cgame", and/or "ui")
If "qagame" is shown as "compiled on load" or "interpreted" then you are running a QVM mod.
If "qagame" is shown as "native" then you are running a DLL/SO mod.

Each game uses a different filename for the mod DLL/SO. Refer to this list when asked to rename QMM.
	Quake 3 Arena: qagamex86.dll / qagamei386.so
	Return to Castle Wolfenstein: qagame_mp_x86.dll / qagame_mp_i386.so
	Jedi Knight 2: jk2mpgamex86.dll / jk2mpgamei386.so
	Jedi Academy: jampx86.dll / jampi386.so

Win32 Installation (QVM mod):
	1. Put qmm.dll and qmmvm.dll in the mod directory
	2. Rename qmm.dll to required <mod filename> (see above)
	3. Either in autoexec.cfg or on the server command line, set vm_game to 0
	4. Optionally set any of the QMM and QMMVM cvars below
	5. Run server

Win32 Installation (DLL mod):
	1. Put qmm.dll in the mod directory
	2. Rename <mod filename> (see above) to something else (call it "qmmvm.dll" if you want it to be auto-detected)
	3. Rename qmm.dll to <mod filename> (see above)
	4. Either in autoexec.cfg or on the server command line, set vm_game to 0
	5. Optionally set any of the QMM cvars below
	6. Run server

Linux Installation (QVM mod):
	1. Put qmmi386.so and qmmvmi386.so in the mod directory
	2. Rename qmmi386.so to <mod filename> (see above)
	3. Either in autoexec.cfg or on the server command line, set vm_game to 0
	4. Optionally set any of the QMM and QMMVM cvars below
	5. Run server

Linux Installation (SO mod):
	1. Put qmmi386.so in the mod directory
	2. Rename <mod filename> (see above) to something else (call it "qmmvmi386.so" if you want it to be auto-detected)
	3. Rename qmmi386.so to <mod filename> (see above)
	4. Either in autoexec.cfg or on the server command line, set vm_game to 0
	5. Optionally set any of the QMM cvars below
	6. Run server


QMM Cvars:
	qmm_modpath	-	sets the path to look for the game mod (default=qmmvm.dll or qmmvmi386.so)
	qmm_pluginfile	-	sets the file to read for list of plugins to load on startup (default=plugins.ini)
	qmm_execcfg	-	sets the standard Q3A .cfg file to exec after plugins are loaded (default=qmmexec.cfg)
	qmm_debug	-	sets whether or not to output function calls to a logfile (default=0)
	
	qmm_version	-	the current QMM version (readonly)

QMMVM Cvars:
	qmmvm_vmpath	-	sets the path to look for the VM (default="vm/qagame.qvm")
	qmmvm_debug	-	sets whether or not to output function calls to a logfile (default=0, log opcodes=2)
	
	qmmvm_version	-	the current QMMVM version (readonly)
	qmmvm_vmbase	-	the memory offset for the VM (readonly, used by QMM plugins)

QMM Server commands:
	qmm		-	outputs list of subcommands (status, list, load, etc.)

QMMVM Server commands:
	qmmvm_status	-	outputs version, loaded VM path, and debug logging status

