QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

Win32 Installation:
    * Each game uses a different filename for the mod DLL. Refer to this list
      when asked to rename QMM in step 2:
        - Quake 3 Arena                                       qagamex86.dll
        - Return to Castle Wolfenstein (Multiplayer)          qagame_mp_x86.dll
        - Return to Castle Wolfenstein: Enemy Territory       qagame_mp_x86.dll
        - Jedi Knight 2                                       jk2mpgamex86.dll
        - Jedi Academy                                        jampgamex86.dll
        - Return to Castle Wolfenstein (Singleplayer)         qagamex86.dll
        
    * Note for listen servers:
	You can get rid of the DLL "Security Warning" message by setting the
	'com_blindlyLoadDLLs' cvar to 1. You can do this by adding the
	following to <mod>/autoexec.cfg	(create it if it does not exist):

	  seta com_blindlyLoadDLLs 1

    * Note for Quake 3 Arena and Jedi Knight 2 servers:
        You must set the 'vm_game' cvar to 0 before QMM will work. You can do
        this by adding the following to <mod>/autoexec.cfg (create it if it
        does not exist):
        
          seta vm_game 0

    * Note for Jedi Academy Win32 servers:
        - Before following step 1, you must extract the jampgamex86.dll file from
          the .pk3 files. You can do this two ways:
            + Extract it manually with a ZIP program like WinZip
            + Run the server once without setting up QMM and you will find the
              jampgamex86.dll inside the mod directory.
        - Replace step 2: Place zzz_qmm_jka.pk3 into the mod directory        
        
        
    1. If a mod DLL (listed above) already exists, prefix the file with "qmm_"
    2. Place qmm.dll into the mod directory and rename it to the correct
       filename listed above
    3. Place pdb.dll and qmm.ini into the root game directory (where the .exe
       is located)
    4. Configure qmm.ini (see file for more details)

Linux Installation:
    * Each game uses a different filename for the mod SO. Refer to this list
      when asked to rename QMM in step 2.
        - Quake 3 Arena                                      qagamei386.so
        - Return to Castle Wolfenstein (Multiplayer)         qagame.mp.i386.so
        - Return to Castle Wolfenstein: Enemy Territory      qagame.mp.i386.so
        - Jedi Knight 2                                      jk2mpgamei386.so
        - Jedi Academy                                       jampgamei386.so
        - Return to Castle Wolfenstein (Singleplayer)        qagamei386.so
        
    * Note for Quake 3 Arena and Jedi Knight 2 servers:
        You must set the 'vm_game' cvar to 0 before QMM will work. You can do
        this by adding the following to <mod>/autoexec.cfg (create it if it
        does not exist):
        
          seta vm_game 0
          

    1. If a mod SO (listed above) already exists, prefix the file with "qmm_"
    2. Place qmm.so into the mod directory and rename it to the correct
       filename listed above
    3. Place pdb.so and qmm.ini into the root game directory (where the server
       binary is located)
    4. Configure qmm.ini as needed (see file for more details)
