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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/include/pdb_types.h,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */
 
#ifndef _PDB_TYPES_H
#define _PDB_TYPES_H

/*
 *	PDB types.
 */
enum {
	NO_NODE_TYPE,
	TREE_NODE_TYPE,
	LIST_NODE_TYPE,
	HASH_NODE_TYPE,
	STRING_NODE_TYPE,
	INT_NODE_TYPE,
	ABSTRACT_NODE_TYPE,
	LINK_NODE_TYPE
};

/*
 *	This needs to be the bit which is higher
 *	than any bit used in the above enumeration.
 */
#define BLOCK_CLOSE			64

/*
 *	The type of token that pdb should consider
 *	a token that does not have an opening block token.
 */
#define DEFAULT_NODE_TYPE	STRING_NODE_TYPE

/*
 *	What type of structure should the root node be?
 */
#define ROOT_NODE_TYPE		TREE_NODE_TYPE
 

/*
 *	Node callback types.
 */
typedef void* (*create_cb_t)(char* id, struct pdb_node_t* parent,
	char** tok_arr);
typedef int (*load_cb_t)(FILE* fptr, struct pdb_node_t* pptr, char** tok_arr,
	int* line);
typedef void (*set_cb_t)(struct pdb_node_t* nptr, void* data);
typedef int (*add_node_cb_t)(struct pdb_node_t* parent, char* id,
	struct pdb_node_t* child);
typedef int (*free_cb_t)(struct pdb_node_t* nptr);
typedef struct pdb_node_t* (*query_cb_t)(struct pdb_node_t* nptr, char* id);
typedef void (*del_child_cb_t)(struct pdb_node_t* pptr,
	struct pdb_node_t* nptr);
typedef int (*write_cb_t)(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs);
typedef int (*count_children_cb_t)(struct pdb_node_t* nptr);
	

/*
 *	Information for a single node type.
 */
struct pdb_node_types_t {
	int bitmask;
	char* str;				/* string name */
	char* open_token;
	char* close_token;
	create_cb_t create_cb;
	load_cb_t load_cb;
	set_cb_t set_cb;
	add_node_cb_t add_child_cb;
	free_cb_t free_cb;
	query_cb_t query_cb;
	del_child_cb_t del_child_cb;
	write_cb_t write_cb;
	count_children_cb_t count_children_cb;
	int write_as_block;		/* use multiplue lines while writing to disk?	*/
};


/*
 *	Handy macros.
 */
#define round_i(x)		((int)(x + .5))


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
 #define DLLEXP __declspec(dllexport)
#else
 #define DLLEXP
#endif

DLLEXP struct pdb_node_t* pdb_create_tree(struct pdb* dbptr, char* path, char* key);
DLLEXP struct pdb_node_t* pdb_create_list(struct pdb* dbptr, char* path, char* key);
DLLEXP struct pdb_node_t* pdb_create_hash(struct pdb* dbptr, char* path, char* key,
	int size);
DLLEXP struct pdb_node_t* pdb_create_abstract(struct pdb* dbptr, char* path,
	char* key, void* data, void* free_cb);
DLLEXP struct pdb_node_t* pdb_set_int(struct pdb* dbptr, char* path,
	char* key, int data);

struct pdb_node_types_t* pdb_get_type_info(int type);
struct pdb_node_types_t* pdb_get_type_info_otok(char* tok);
struct pdb_node_types_t* pdb_get_type_info_ctok(char* tok);

int pdb_standard_load_node(FILE* fptr, struct pdb_node_t* pptr,
	char** tok_arr, int* line);
DLLEXP int pdb_free_node(struct pdb_node_t* nptr);
int pdb_standard_write_node(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs);
char* pdb_node_type_name(int type);

/*
 *	Binary tree
 */
void* pdb_create_tree_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr);
int pdb_load_tree_node_cb(FILE* fptr, struct pdb_node_t* pptr, char** tok_arr,
	int* line);
int pdb_add_tree_node_child_cb(struct pdb_node_t* parent, char* id,
	struct pdb_node_t* child);
int pdb_free_tree_node_cb(struct pdb_node_t* nptr);
struct pdb_node_t* pdb_query_tree_node_cb(struct pdb_node_t* nptr, char* id);
void pdb_del_child_tree_node_cb(struct pdb_node_t* pptr,
	struct pdb_node_t* nptr);
int pdb_write_tree_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs);
int pdb_count_children_tree_cb(struct pdb_node_t* nptr);

/*
 *	Link list
 */
void* pdb_create_list_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr);
int pdb_load_list_node_cb(FILE* fptr, struct pdb_node_t* pptr, char** tok_arr,
	int* line);
int pdb_add_list_node_child_cb(struct pdb_node_t* parent, char* id,
	struct pdb_node_t* child);
int pdb_free_list_node_cb(struct pdb_node_t* nptr);
struct pdb_node_t* pdb_query_list_node_cb(struct pdb_node_t* nptr, char* id);
void pdb_del_child_list_node_cb(struct pdb_node_t* pptr,
	struct pdb_node_t* nptr);
int pdb_write_list_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs);
int pdb_count_children_list_cb(struct pdb_node_t* nptr);

/*
 *	Hash table
 */
void* pdb_create_hash_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr);
int pdb_load_hash_node_cb(FILE* fptr, struct pdb_node_t* pptr, char** tok_arr,
	int* line);
void pdb_set_hash_node_cb(struct pdb_node_t* nptr, void* data);
int pdb_add_hash_node_child_cb(struct pdb_node_t* parent, char* id,
	struct pdb_node_t* child);
int pdb_free_hash_node_cb(struct pdb_node_t* nptr);
struct pdb_node_t* pdb_query_hash_node_cb(struct pdb_node_t* nptr, char* id);
void pdb_del_child_hash_node_cb(struct pdb_node_t* pptr,
	struct pdb_node_t* nptr);
int pdb_write_hash_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs);
int pdb_count_children_hash_cb(struct pdb_node_t* nptr);

/*
 *	String
 */
void* pdb_create_string_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr);
void pdb_set_string_node_cb(struct pdb_node_t* nptr, void* data);
int pdb_free_string_node_cb(struct pdb_node_t* nptr);
int pdb_write_string_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs);

/*
 *	Integer
 */
void* pdb_create_int_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr);
int pdb_load_int_node_cb(FILE* fptr, struct pdb_node_t* pptr, char** tok_arr,
	int* line);
void pdb_set_int_node_cb(struct pdb_node_t* nptr, void* data);
int pdb_free_int_node_cb(struct pdb_node_t* nptr);
int pdb_write_int_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs);

/*
 *	Abstract
 */
void* pdb_create_abstract_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr);
void pdb_set_abstract_node_cb(struct pdb_node_t* nptr, void* data);
int pdb_free_abstract_node_cb(struct pdb_node_t* nptr);

/*
 *	Link
 */
void* pdb_create_link_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr);
int pdb_free_link_node_cb(struct pdb_node_t* nptr);
struct pdb_node_t* pdb_query_link_node_cb(struct pdb_node_t* nptr, char* id);

#ifdef __cplusplus
}
#endif

#endif /* _PDB_TYPES_H */
