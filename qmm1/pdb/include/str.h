/*
 *	This file is part of FinOP.
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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/include/str.h,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */

 
#ifndef _STR_H
#define _STR_H

#define int_to_char(x)	((char)(x + 48))

#ifdef WIN32
#define vsnprintf _vsnprintf
#endif

#ifdef __cplusplus
extern "C"
{
#endif

char** token_parse(char* token, char delim, char* dump);
void token_rm_index(char** tok_arr, int index);
char* token_concat(char** tok_arr, int index);
char* token_data(char** tok_arr);
void token_free(char** tok_arr);

int str_find(char* str, char tok);
void str_rem_chr(char* chr);
char* str_rem(char* str, char rem);
char* str_cat(char* s1, char* s2);
char* str_cat_c(char* s, char c);

char* va(char* str, char* format, ...);

int wildcard_match(char* str, char* mask);
	
void strip_lf(char* str);
void strip_cr(char* str);
void strip_crlf(char* str);
char* add_lf(char* str);

char* str_apply(char* str, char* apply);

void chk_str(char* str);
char* rnd_str(int min, int max);
int rnd_i(int seed);

int cstrcmp(char* s1, char* s2);
char* str_dup(char* str);

void** arr_create(int size);
void arr_append(void** arr, void* data);
void arr_rem(void** arr, int i, int free_data);
void arr_free(void** arr, int size, int free_data);

int require_params(char* str, int need);

int str_starts_with(char* str, char* tok);

#ifdef __cplusplus
}
#endif

#endif /* _STR_H */
