Stripper - Dynamic Map Entity Modification
Copyright QMM Team 2005
http://www.q3mm.org/

Stripper is a dynamic map entity modifier. It can strip out most entities from a map (a complete list for any
map can be obtained with the stripper_dump command below) and also add new entities. The map itself is not
modified, this is all accomplished while the map entity information is being loaded.

Created By:
    Kevin Masterson a.k.a. CyberMind <kevinm@planetquake.com>

Win32 Installation:
	1. Install QMM
	2. Make a qmmaddons directory inside your mod directory if it does not exist already
	3. Unzip into the qmmaddons directory.
	4. Add "qmmaddons/stripper/dlls/stripper_qmm.dll" as an entry in the plugins list in qmm.ini

Linux Installation:
	1. Install QMM
	2. Make a qmmaddons directory inside your mod directory if it does not exist already
	3. Unzip into the qmmaddons directory.
	4. Add "qmmaddons/stripper/dlls/stripper_qmm.so" as an entry in the plugins list in qmm.ini
	
Setup:
	Server Commands:
	* stripper_dump - Dumps the current maps' default entity list to qmmaddons/stripper/dumps/<mapname>.txt
	
	Configuration Files:
	There are 2 files loaded per map. One is the global configuration file that is loaded for every map,
	and the other is specific to the current map.
	
	The global configuration file is located at qmmaddons/stripper/global.ini and the map-specific file is
	located at qmmaddons/stripper/maps/<mapname>.ini. A sample global.ini and q3dm1.ini are provided in the
	package.
	
	Configuration files have the following format:
		;this line is a comment
		#this line is a comment
		//this line is a comment
		
		filter:
		{
		   key=val
		}
		{
		   key=val
		}
		add:
		{
		   key=val
		}
		replace:
		{
		   key=val
		}
		{
		   key=val
		}
		with:
		{
		   key=val
		}
		
	- A "filter" section specifies entity masks for entities that should be removed from the map.
	  If an entity has all the keys provided in a mask, and the values associated with them match, the
	  entity is removed.

	- An "add" section specifies complete entity information to add to the map.
	  Entities are added with only the keys and values provided in the block.
	  
	- A "replace" section specifies entity masks for entities that should be replaced or modified.
	  If an entity has all the keys provided in a mask, and the values associated with them match, the
	  entity will be replaced or modified.
	  
	- A "with" section specifies what keys and values should be replaced on any associated "replace"
	  masks. Only the keys provided in the "with" block will be replaced. A "with" section will affect
	  all "replace" masks created after the previous "with" block (or the start of the file if none).
	  
	"Filter", "add", and "with" modify the entity list in the order they appear. For example, the
	following will result in no new entities being added, since the second "filter" section will cause
	the added item to be removed:
		filter:
		{
		   classname=item_health
		}
		add:
		{
		   classname=item_health
		   origin=100 100 10
		}
		filter:
		{
		   classname=item_health
		}


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
