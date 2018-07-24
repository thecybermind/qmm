/*
 *	This file is part of PDB.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/src/pdb_file.c,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */
 
#include <stdio.h>
#include "pdb.h"
 
/*
 *	Open a file for reading.
 */
FILE* pdb_open_file(char* file) {
	FILE* fptr = fopen(file, "r");

	#ifdef PDB_DEBUG
		if (fptr == NULL)
			fprintf(stderr,
				"pdb_file.c: pdb_open_file(): Unable to open file '%s'.\n",
				file);
	#endif
		
	return fptr;
}


/*
 *	Close a file.
 */
void pdb_close_file(FILE* fptr) {
	if (fptr != NULL)
		if (fclose(fptr)) {
			#ifdef PDB_DEBUG
				fprintf(stderr,
					"pdb_file.c: pdb_close_file(): Unable to close file.\n");
			#endif
		}
}
