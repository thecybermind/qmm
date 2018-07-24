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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/src/pdb_parse.c,v 1.3 2005/09/22 01:02:15 cybermind Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "str.h"
#include "pdb.h"
#include "pdb_types.h"
#include "pdb_parse.h"

static int is_end_of_token(char byte);
static struct pdb_node_types_t* is_opening_tok(FILE* fptr, char byte);
static struct pdb_node_types_t* is_closing_tok(FILE* fptr, char byte);
static int is_all_whitespaces(char* str);
static char* remove_chasing_whitespaces(char* str);

/*
 *	Return the next token from the loaded file.
 *	A token will be the entire string, excluding any
 *	padding whitespaces.  It will also not include
 *	the type token, this will be returned by the "type"
 *	parameter.
 *
 *	If EOF or critical error, return NULL.
 */
char* pdb_get_token(FILE* fptr, int* type, int* line) {
	int buf_size = BLOCK_SIZE;
	int used = 1;
	char* sbuf;
	char* buf;
	int ws_end = 0;
	char byte;
	int quotes = 0;		/*	total amount of quotes went through	*/
	struct pdb_node_types_t* tptr = NULL;
	
	sbuf = (char*)malloc(sizeof(char) * buf_size);
	memset(sbuf, 0, sizeof(char) * buf_size);
	buf = sbuf;
	
	while (!feof(fptr)) {
		byte = fgetc(fptr);
		
		if (byte == '\n')
			(*line)++;
		
		/*
		 *	Skip prepended whitespaces.
		 */
		if (!ws_end) {
			if (is_whitespace(byte))
				continue;
			ws_end = 1;
		}
		
		/*
		 *	If comment (and enabled), skip token.
		 */
		#ifdef PDB_ENABLE_COMMENTS
		if (!(quotes % 2) && (pdb_parse_comment(fptr, line, byte))) {
			/*
			 *  Since comments cannot be in string literals,
			 *  we can safely skip any following whitespaces.
			 */
			ws_end = 0;
			continue;
		}
		#endif /* PDB_ENABLE_COMMENTS */
		

		if (byte == '\"')
			++quotes;

		/*
		 *	Is a block opening?
		 */
		if (!(quotes % 2)) {
			tptr = is_opening_tok(fptr, byte);
			if (tptr) {
				/*
				 *	Make sure we have a token.
				 */
				if (is_all_whitespaces(sbuf)) {
					free(sbuf);
					return pdb_get_token(fptr, type, line);
				}

				*type = tptr->bitmask;
				return remove_chasing_whitespaces(sbuf);
			}
		}
		
		/*
		 *	Is a block closing?
		 */
		if (!(quotes % 2) && (*type != NO_NODE_TYPE)) {
			tptr = is_closing_tok(fptr, byte);
			if (tptr) {
				if (*type != tptr->bitmask) {
					fprintf(stderr, "pdb_parse.c: pdb_get_token(): Invalid "
						"closing block token on line %i. Closing type %s for "
						"type %s.\n", *line, pdb_node_type_name(tptr->bitmask),
						pdb_node_type_name(*type));
					free(sbuf);
					return NULL;
				}

				*type |= BLOCK_CLOSE;
				return sbuf;
			}
		}
		
		/*
		 *	Stop when the token has ended -- only if outside of quotes.
		 */
		if (!(quotes % 2)) {
			if (is_end_of_token(byte)) {
				if (feof(fptr)) {
					free(sbuf);
					return NULL;
				}
				
				/*
				 *	Make sure we have a token.
				 */
				if (is_all_whitespaces(sbuf)) {
					free(sbuf);
					return pdb_get_token(fptr, type, line);
				}

				/*
				 *	We assume the node type is default.
				 */
				*type = DEFAULT_NODE_TYPE;
				return remove_chasing_whitespaces(sbuf);
			}
		}

		*buf = byte;
		++buf;
		++used;
		
		if (used >= buf_size) {
			/*
			 *	Block needs to be expanded; double size.
			 */
			buf_size *= 2;
			sbuf = (char*)realloc(sbuf, sizeof(char) * buf_size);
			buf = (sbuf + (sizeof(char) * (used - 1)));
			memset(buf, 0, (sizeof(char) * (buf_size - used)));
		}
	}
	
	if (feof(fptr)) {
		/*
		 *	EOF
		 */
		free(sbuf);
		return NULL;
	}
	return remove_chasing_whitespaces(sbuf);
}


/*
 *	Return 1 if token has ended.
 */
int is_end_of_token(char byte) {
	return ((byte == PDB_TOKEN_TERM) || (byte == '\n'));
}


/*
 *	Return the node type info struct if a block is opening.
 */
struct pdb_node_types_t* is_opening_tok(FILE* fptr, char byte) {
	char stok[3];
	char* tok = stok;
	*tok = '\0';
	
	if (!is_whitespace(byte)) {
		*tok = byte;
		++tok;
	}
	FPEEK(tok, fptr);
	if (!is_whitespace(*tok) && (*tok != '\n'))
		++tok;
	*tok = '\0';

	return pdb_get_type_info_otok(stok);
}


/*
 *	Return the node type info struct if a block is closing.
 */
struct pdb_node_types_t* is_closing_tok(FILE* fptr, char byte) {
	char stok[3];
	char* tok = stok;
	*tok = '\0';
	
	if (!is_whitespace(byte)) {
		*tok = byte;
		++tok;
	}
	FPEEK(tok, fptr);
	if (!is_whitespace(*tok) && (*tok != '\n'))
		++tok;
	*tok = '\0';

	return pdb_get_type_info_ctok(stok);
}


/*
 *	Return 1 if the string is only whitespaces.
 */
int is_all_whitespaces(char* str) {
	while (*str) {
		if (!is_whitespace(*str))
			return 0;
		++str;
	}
	return 1;
}


/*
 *	Return a string with all chasing whitespaces removed.
 */
char* remove_chasing_whitespaces(char* str) {
	char* s = (str + (sizeof(char) * (strlen(str) - 1)));
	while (s >= str) {
		if (!is_whitespace(*s))
			break;
		*s = '\0';
		--s;
	}
	return str;
}


/*
 *	Return 1 if the given string
 *	starts with the tok substring.
 */
int tok_starts_with(char* str, char* tok) {
	if ((!str) || (!tok))
		return 0;
	while (*str) {
		if (!tok)
			return 1;
		if (!*tok)
			return 1;
		if (*str != *tok)
			return 0;
		++str;
		++tok;
	}
	if (!tok)
		return 1;
	if (!*tok)
		return 1;
	return 0;
}


/*
 *	pdb wrapper for token_parse()
 *	Execute token_parse() and
 *	remove elements with only whitespaces.
 */
char** pdb_token_parse(char* str) {
	char** tok_arr;
	int i = 0;
	int s = 0;
	
	tok_arr = token_parse(str, '\"', NULL);

	while (tok_arr[i]) {
		s = 0;
		while (tok_arr[i][s]) {
			if (!is_whitespace(tok_arr[i][s])) {
				s = -1;
				break;
			}
			++s;
		}
		if (s != -1) {
			token_rm_index(tok_arr, i);
			continue;
		}
		++i;
	}
	return tok_arr;
}


/*
 *	If the byte is a comment, skip comment.
 *	Return 1 if it is a commant, 0 if not.
 */
#ifdef PDB_ENABLE_COMMENTS
int pdb_parse_comment(FILE* fptr, int* line, char byte) {
	if (byte == PDB_COMMENT_CHAR) {
		/*
		 *	Feed to EOL.
		 */
		int i = 0;
		while (!feof(fptr)) {
			byte = fgetc(fptr);
			++i;
			if (byte == '\n') {
				(*line)++;
				return 1;
			}
		}
	} else if (byte == PDB_COMMENT_BLOCK_S[0]) {
		if (strlen(PDB_COMMENT_BLOCK_S) == 2) {
			FPEEK(&byte, fptr);
			if (byte != PDB_COMMENT_BLOCK_S[1])
				return 0;
		}
		/*
		 *	Feed to PDB_COMMENT_BLOCK_E.
		 */
		while (!feof(fptr)) {
			byte = fgetc(fptr);
			if (byte == '\n')
				(*line)++;
			else if (byte == PDB_COMMENT_BLOCK_E[0]) {
				if (strlen(PDB_COMMENT_BLOCK_E) == 2) {
					FPEEK(&byte, fptr);
					if (byte != PDB_COMMENT_BLOCK_E[1])
						continue;
				}
				fgetc(fptr);
				return 1;
			}
		}
	}
	return 0;
}
#endif /* PDB_ENABLE_COMMENTS */
