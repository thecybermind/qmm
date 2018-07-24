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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/src/str.c,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */

 
/*
 *	This file includes string
 *	related functions.
 */

#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock.h>
	#include <time.h>
#else
	#include <sys/time.h>
	#include <pthread.h>
#endif

#include "str.h"

/*
 *	Parse a token into an array.
 *	The resulting array contains
 *	character pointers to specific
 *	locations in the first token, which
 *	is a copy of the passed token.
 *	Thus, after calling this function,
 *	the passed token will no longer be
 *	needed, so it may be freed.
 *
 *	The first element of the array is
 *	a copy of the passed token.
 *	Following elements are pointers which
 *	are the individual tokens.
 *	The array is terminated by a NULL
 *	element.
 *
 *	The delim is the delimiter in which
 *	the token will be split.
 *	The dump character is that in which
 *	if encountered will terminate parsing
 *	and put the rest of the token in the
 *	next element. Pass NULL if you do not
 *	wish to use this feature.
 *
 *	Be sure to free the resulting array
 *	when it is no longer needed.
 */
char** token_parse(char* token, char delim, char* dump) {
	int size = 2;
	char* ctoken = token;
	char** tok_arr = NULL;
	char* cpy = NULL;
	int index = 0;
	char* last = NULL;

	/*
	 *	Determine the number of elements.
	 */
	while (*ctoken) {
		if (*ctoken == delim)
			++size;
		else if (dump != NULL) {
			if (*ctoken == *dump) {
				++size;
				break;
			}
		}
		++ctoken;
	}
	
	/*
	 *	Allocate memory for the array.
	 */
	tok_arr = (char**)malloc(sizeof(char*) * size + sizeof(char*));
	
	/*
	 *	Copy token to first index of array.
	 */
	cpy = (char*)malloc(sizeof(char) * strlen(token) + sizeof(char));
	strcpy(cpy, token);
	token = cpy;
	
	/*
	 *	Parse the token.
	 */
	index = 0;
	last = token;
	while (*token) {
		if (*token == delim) {
			tok_arr[index] = last;
			*token = '\0';
			last = (token + 1);
			++index;
		} else if (dump != NULL) {
			if (*token == *dump) {
				last++;
				break;
			}
		}
		++token;
	}
	tok_arr[index] = last;
	tok_arr[index + 1] = NULL;
	tok_arr[index + 2] = cpy;

	return tok_arr;
}


/*
 *	Remove a given index
 *	from a parsed token array.
 */
void token_rm_index(char** tok_arr, int index) {
	int i = 0;

	for (; i < index; i++)
		if (!tok_arr[i])
			return;
	
	while (tok_arr[i]) {
		tok_arr[i] = tok_arr[i + 1];
		++i;
	}
	tok_arr[i] = tok_arr[i + 1];
}


/*
 *	Combine all tokens
 *	including and after
 *	index.
 */
char* token_concat(char** tok_arr, int index) {
	int i = 0;
	char* ret = NULL;

	for (; i < index; i++)
		if (!tok_arr[i])
			return NULL;
	
	ret = (char*)malloc(sizeof(char));
	*ret = '\0';
		
	while (tok_arr[i]) {
		ret = (char*)realloc(ret, (sizeof(char) *
			(strlen(ret) + strlen(tok_arr[i])) + (sizeof(char))));
		strcat(ret, tok_arr[i]);
		++i;
		if (tok_arr[i]) {
			ret = (char*)realloc(ret, (sizeof(char) * (strlen(ret) + 2)));
			strcat(ret, " ");
		}
	}
	
	return ret;
}


/*
 *	Return a pointer to the
 *	token array data index.
 */
char* token_data(char** tok_arr) {
	int i = 0;

	while (tok_arr[i])
		++i;
	return tok_arr[i + 1];
}


/*
 *	Free a parsed token array
 *	created by token_parse()
 */
void token_free(char** tok_arr) {
	int i = 0;

	while (tok_arr[i])
		++i;
	free(tok_arr[i + 1]);
	free(tok_arr);
}


/*
 *	Determine if tok is in str.
 */
int str_find(char* str, char tok) {
	int i = 0;
	
	if (!str)
		return -1;

	while (*str) {
		if (*str == tok)
			return i;
		++str;
		++i;
	}
	return -1;
}


/*
 *	Remove all occurances of
 *	a given character.
 *	Example:
 *	str_rem("mississippi", 'i')
 *	returns "msssspp".
 *
 *	String size will not be changed.
 */
char* str_rem(char* str, char rem) {
	char* ret = str;
	char* sstr = NULL;

	while (*str) {
		if (*str == rem) {
			sstr = str;
			while (*sstr) {
				*sstr = *(sstr + 1);
				++sstr;
			}
			--str;
		}
		++str;
	}
	return ret;
}


/*
 *	Remove a char in a string by
 *	sliding everything after it to
 *	the left.
 */
void str_rem_chr(char* chr) {
	while (*chr) {
		*chr = *(chr + 1);
		++chr;
	}
}


/*
 *	Concat two strings, expand s1
 *	before adding s2.
 */
char* str_cat(char* s1, char* s2) {
	s1 = (char*)realloc(s1, sizeof(char) * (strlen(s1) + strlen(s2) + 1));
	s1 = strcat(s1, s2);
	return s1;
}


/*
 *	Concat a character to a string, expand s
 *	before adding c.
 */
char* str_cat_c(char* s, char c) {
	s = (char*)realloc(s, sizeof(char) * (strlen(s) + 2));
	while (*s)
		++s;
	*s = c;
	*(s + 1) = 0;
	return s;
}


/*
 *	Format a string per printf()
 *	specifications.
 */
char* va(char* str, char* format, ...) {
	va_list argptr;
	int size = (sizeof(char) * strlen(format) + sizeof(char));
	int ret_size = 0;

	if (!str)
		str = (char*)malloc(size);
	else
		str = (char*)realloc(str, size);
	if (!str)
		return NULL;
	memset(str, 0, size);
	
	while (1) {
		va_start(argptr, format);
		ret_size = vsnprintf(str, size, format, argptr);
		va_end(argptr);
		
		if (!str)
			return NULL;
		
		if (ret_size >= size)
			/*	truncated	*/
			size *= 2;
		else
			/*	format done	*/
			break;
		
		str = (char*)realloc(str, size);
	}
	
	/*	drop unused memory	*/
	str = (char*)realloc(str, sizeof(char) * strlen(str) + sizeof(char));
	if (!str)
		return NULL;

	/*	be sure last character is a terminater	*/
	*(str + (sizeof(char) * strlen(str))) = 0;
	return str;
}


/*
 *	Return 1 if str matches mask.
 */
int wildcard_match(char* str, char* mask) {
	/*
	 *	Method derived from SRVX IRC Services:
	 *	http://www.srvx.org
	 */
	int multi_wild = 0;

	/*
	 *	Match str against mask
 	 *	* denotes multi-character wildcard
	 *	? denotes single-character wildcard
	 */
	while (*mask) {
	multi_wild = 0;
	switch (*mask) {
		case '*':
		case '?':
			/*	Determine wildcard situation	*/
			while (*mask) {
				if (*mask == '*')
					multi_wild = 1;
				else if (*mask == '?') {
					/*	Move ahead in str 1 byte	*/
					if (!*str)	/*	str done	*/
						return 0;
					str++;
				} else
					/*	No Longer a WildCard		*/
					break;
				mask++;
			}
			/*	Move ahead in str for multi_wild	*/
			if (multi_wild) {
				if (!*mask)
					/*	mask done, str matches	*/
					return 1;
				while (*str) {
					if ((*str == *mask) && wildcard_match((str+1), (mask+1)))
						/*	First chars are equal and
							str matches wild		*/
						return 1;
					str++;
				}
				return 0;
			}
		default:
			/*	Not a wildcard	*/
			if (tolower(*mask) != tolower(*str))
				return 0;
			while ((*mask) && (*str) && (*mask != '?') && (*mask != '*'))
				/*	If more mask and str	*/
				if (tolower(*mask++) != tolower(*str++))
					/*	If str byte does not equal mask byte	*/
					return 0;
			if ((*str) && (!*mask))
				/*	Mask Ended but More Str		*/
				return 0;
	}
	}
	return 1;
}


/*
 *	Replace an \n with \0 if the string ends with it.
 */
void strip_lf(char* str) {
	while (*str)
		++str;
	--str;
	if (*str == '\n')
		*str = *(str + 1);
}


/*
 *	Replace an \r with \0 if the string ends with it.
 */
void strip_cr(char* str) {
	while (*str)
		++str;
	--str;
	if (*str == '\r')
		*str = *(str + 1);
}


/*
 *	Replace an \r\n with \0 if the string ends with it.
 */
void strip_crlf(char* str) {
	while (*str)
		++str;
	str -= 2;
	if (*str == '\r')
		*str = *(str + 2);
}


/*
 *	Add a \n to the end of a string (if not existant.)
 */
char* add_lf(char* str) {
	char* ostr = str;

	while (*str)
		++str;
	--str;
	if (*str != '\n') {
		ostr = (char*)realloc(ostr, sizeof(char) * strlen(ostr) +
			(2 * sizeof(char)));
		str = ostr;
		while (*str)
			++str;
		*(str + 1) = *str;
		*str = '\n';
	}
	return ostr;
}


/*
 *	Apply character manipulation to
 *	the passed string.
 *
 *	The first parameter will be modified.
 *
 *	Example:
 *		str_apply("foobar", "+asdf-or")
 *		changes the first parameter to:
 *		"fbasd".
 */
char* str_apply(char* str, char* apply) {
	char tswitch = ' ';
	char* estr = NULL;

	str = (char*)realloc(str, sizeof(char) *
		(strlen(str) + strlen(apply)) + sizeof(char));
	
	estr = (str + (strlen(str) * sizeof(char)));
	while (*apply) {

		switch (*apply) {
			case '+':
			case '-':
			{
				tswitch = *apply;
				break;
			}
			default:
			{
				if (tswitch == '+') {
					if (str_find(str, *apply) == -1) {
						*estr = *apply;
						++estr;
					}
				} else if (tswitch == '-') {
					str = str_rem(str, *apply);
					--estr;
				}
				break;
			}
		}
		++apply;
	}
	
	*estr = '\0';
	str = (char*)realloc(str, sizeof(char) * strlen(str) + sizeof(char));
	
	return str;
}


/*
 *	Make sure the last character of
 *	the string is a null terminater.
 */
void chk_str(char* str) {
	*(str + (sizeof(char) * strlen(str))) = 0;
}


/*
 *	Generates a random string from
 *	min to max in size.  Generally used
 *	for testing purposes.
 */
char* rnd_str(int min, int max) {
	int i = 0, r = 0;
	int s = ((rnd_i(time(NULL)) % (max - min)) + min);
	char* buf = (char*)malloc(sizeof(char) * s + sizeof(char));
	--s;
	
	for (; i < s; i++) {
		r = 0;
		while (r == '\n' || r == '\r' || r == 0 || r == '\\') {
			r = rnd_i(i + (r + 1));
			r = ((r % 126) + 1);
		}
		
		*(buf + (i * sizeof(char))) = r;
	}

	*(buf + (i * sizeof(char))) = 0;
	return buf;
}


/*
 *	Return a random number from the random
 *	number table.
 */
int rnd_i(int seed) {
	int s;
	struct timeval tv;
	#ifdef WIN32
		long y;
	#endif

	#ifdef WIN32
		y = GetTickCount();
		tv.tv_usec = ((y % 1000) * 1000);
		tv.tv_sec = (y / 1000);
	#elif defined(POSIX)
		gettimeofday(&tv, NULL);
	#endif

	s = (tv.tv_usec * tv.tv_sec * seed + 1);
	if (s < 0)
		s *= -1;
	return (s);
}


/*
 *	Case insensitive string compare.
 *	Return:
 *		-1	s1 < s2
 *		0	s1 = s2
 *		1	s1 > s2
 */
int cstrcmp(char* s1, char* s2) {
	if (!s1 && s2)			return -1;
	else if (s1 && !s2)		return 1;
	else if (!s1 && !s2)	return 0;
	
	while (*s1) {
		if (!*s2)
			return 1;
		if (tolower(*s1) > tolower(*s2))
			return 1;
		else if (tolower(*s1) < tolower(*s2))
			return -1;
		++s1;
		++s2;
	}
	return (*s2 ? -1 : 0);
}


/*
 *	Duplicate a string.
 */
char* str_dup(char* str) {
	char* ret = (char*)malloc(sizeof(char) * (strlen(str) + 1));
	strcpy(ret, str);
	return ret;
}


/*
 *	Create an array of pointers of size 'size'.
 */
void** arr_create(int size) {
	void** arr = (void**)malloc(sizeof(void*) * (size + 1));
	memset(arr, 0, sizeof(void*) * (size + 1));
	return arr;
}


/*
 *	Set the next NULL element to 'data'.
 *	No protection, might be an invalid write!
 */
void arr_append(void** arr, void* data) {
	int i = 0;

	while (arr[i])
		++i;
	arr[i] = data;
}


/*
 *	Remove an element from the given array.
 */
void arr_rem(void** arr, int i, int free_data) {
	if (free_data)
		free(arr[i]);
	while (arr[i])
		arr[i] = arr[i + 1];
}


/*
 *	Free an array.
 */
void arr_free(void** arr, int size, int free_data) {
	int i = 0;

	if (free_data) {
		for (; i < size; i++) {
			if (arr[i])
				free(arr[i]);
		}
	}
	free(arr);
}


/*
 *	Returns 1 if at least 'need' tokens
 *	are in 'str'.
 */
int require_params(char* str, int need) {
	int i = 0;
	int ws = 0;
	
	while (*str) {
		if (*str == ' ')
			++ws;
		else {
			if (ws)
				++i;
			ws = 0;
		}
		++str;
	}
	
	return (i >= need);
}


/*
 *	Return 1 if the given string
 *	starts with the tok substring.
 */
int str_starts_with(char* str, char* tok) {
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
