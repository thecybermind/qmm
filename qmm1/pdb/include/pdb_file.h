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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/include/pdb_file.h,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */
 
#ifndef _PDB_FILE_H
#define _PDB_FILE_H

#ifdef __cplusplus
extern "C"
{
#endif

FILE* pdb_open_file(char* file);
void pdb_close_file(FILE* fptr);

#ifdef __cplusplus
}
#endif

#endif /* _PDB_FILE_H */
