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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/include/pdb_parse.h,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */
 
#ifndef _PDB_PARSE_H
#define _PDB_PARSE_H

#define BLOCK_SIZE		1024
#define is_whitespace(x)	((x == ' ') || (x == '\t'))

#define FPEEK(cptr, fptr)	{ \
								*cptr = fgetc(fptr); \
								ungetc(*cptr, fptr); \
							}
							
#ifdef __cplusplus
extern "C"
{
#endif
	
char* pdb_get_token(FILE* fptr, int* type, int* line);
int tok_starts_with(char* str, char* tok);
char** pdb_token_parse(char* str);

#ifdef PDB_ENABLE_COMMENTS	
int pdb_parse_comment(FILE* fptr, int* line, char byte);
#endif /* PDB_ENABLE_COMMENTS */

#ifdef __cplusplus
}
#endif

#endif /* _PDB_PARSE_H */
