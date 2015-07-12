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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/src/pdb_types.c,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */
 
/*
 *	NOTE
 *
 *	Be aware if PDB_THREAD_SAFE is enabled for the given database,
 *	most functions in pdb.c are mutex protected -- watch out for
 *	multipule locks.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include "list.h"
#include "binarytree.h"
#include "hash.h"
#include "str.h"
#include "pdb.h"
#include "pdb_parse.h"
#include "pdb_types.h"

/*
 *	Node type definitions.
 *
 *	These must be in the same order as
 *	the bitmask definitions.
 *		Bitmask 1 -> Entry 0
 *		Bitmask 2 -> Entry 1
 *		Bitmask 4 -> Entry 2 ... etc
 *
 *	Opening and closing tokens CAN NOT
 *	be larger than two characters.
 */
struct pdb_node_types_t pdb_types[] = {
	{
		/*	Binary Tree */
		TREE_NODE_TYPE,
		"TREE_NODE",
		"{",
		"}",
		pdb_create_tree_node_cb,
		pdb_load_tree_node_cb,
		NULL,							/*	trees cannot be "set"			*/
		pdb_add_tree_node_child_cb,
		pdb_free_tree_node_cb,
		pdb_query_tree_node_cb,
		pdb_del_child_tree_node_cb,
		pdb_write_tree_node_cb,
		pdb_count_children_tree_cb,
		1								/*	write to disk in block format	*/
	},
	{
		/*	Link List	*/
		LIST_NODE_TYPE,
		"LIST_NODE",
		"(",
		")",
		pdb_create_list_node_cb,
		pdb_load_list_node_cb,
		NULL,							/*	lists cannot be "set"			*/
		pdb_add_list_node_child_cb,
		pdb_free_list_node_cb,
		pdb_query_list_node_cb,
		pdb_del_child_list_node_cb,
		pdb_write_list_node_cb,
		pdb_count_children_list_cb,
		1								/*	write to disk in block format	*/
	},
	{
		/*	Hash table	*/
		HASH_NODE_TYPE,
		"HASH_NODE",
		"[",
		"]",
		pdb_create_hash_node_cb,
		pdb_load_hash_node_cb,
		pdb_set_hash_node_cb,			/*	hash setting "creates" the hash	*/
		pdb_add_hash_node_child_cb,
		pdb_free_hash_node_cb,
		pdb_query_hash_node_cb,
		pdb_del_child_hash_node_cb,
		pdb_write_hash_node_cb,
		pdb_count_children_hash_cb,
		1								/*	write to disk in block format	*/
	},
	{
		/*	String	*/
		STRING_NODE_TYPE,
		"STRING_NODE",
		NULL,							/*	standard load type (fallback)	*/
		NULL,							/*	standard load type (fallback)	*/
		pdb_create_string_node_cb,
		NULL,							/*	load done by creation			*/
		pdb_set_string_node_cb,
		NULL,							/*	no child nodes supported		*/
		pdb_free_string_node_cb,
		NULL,
		NULL,							/*	string has no child				*/
		pdb_write_string_node_cb,
		NULL,							/*	strings do not have children	*/
		0								/*	write to disk in line format	*/
	},
	{
		/*	Integer type	*/
		INT_NODE_TYPE,
		"INT_NODE",
		"'",
		"'",
		pdb_create_int_node_cb,
		pdb_load_int_node_cb,
		pdb_set_int_node_cb,
		NULL,							/*	integers have no children		*/
		pdb_free_int_node_cb,
		NULL,
		NULL,
		pdb_write_int_node_cb,
		NULL,							/*	integers do not have children	*/
		0								/*	write to disk in line format	*/
	},
	{
		/*	Abstract type	*/
		ABSTRACT_NODE_TYPE,
		"ABSTRACT_NODE",
		NULL,							/*	no loading from disk			*/
		NULL,							/*	no loading from disk			*/
		pdb_create_abstract_node_cb,
		NULL,
		pdb_set_abstract_node_cb,
		NULL,							/*	abstracts have no children		*/
		pdb_free_abstract_node_cb,
		NULL,
		NULL,
		NULL,							/*	not written to disk				*/
		NULL,							/*	abstracts do not have children	*/
		0								/*	not written to disk				*/
	},
	{
		/*
		 *	Link Node
		 *	For efficiency, this should remain
		 *	 the last type in the array.
		 */
		LINK_NODE_TYPE,
		"LINK_NODE",
		NULL,							/*	no loading from disk			*/
		NULL,							/*	no loading from disk			*/
		pdb_create_link_node_cb,
		NULL,
		NULL,							/*	links cannot be "set"			*/
		NULL,							/*	links have no real children		*/
		pdb_free_link_node_cb,
		pdb_query_link_node_cb,
		NULL,							/*	children are ignored on free	*/
		NULL,							/*	links not written to disk		*/
		NULL,							/*	links do not have children		*/
		0								/*	links not written to disk		*/
	},
	{
		/*	Terminating Entry	*/
		NO_NODE_TYPE,	NULL, NULL,	NULL,	NULL,
		NULL,			NULL,	NULL,	NULL,
		NULL,			NULL,	NULL,	NULL,	0
	}
};


static void* pdb_standard_create_node(char* id, struct pdb_node_t* parent,
	int type);
static void pdb_del_node_from_parent(struct pdb_node_t* nptr);
static int pdb_free_node_cb(void* dptr);


/******************
 ******************
 ******************		Container creation and set wrappers.
 ******************
 ******************/

		
DLLEXP struct pdb_node_t* pdb_create_tree(struct pdb* dbptr, char* path, char* key) {
	return pdb_set_node(dbptr, path, key, NULL, TREE_NODE_TYPE);
}

DLLEXP struct pdb_node_t* pdb_create_list(struct pdb* dbptr, char* path, char* key) {
	return pdb_set_node(dbptr, path, key, NULL, LIST_NODE_TYPE);
}

DLLEXP struct pdb_node_t* pdb_create_hash(struct pdb* dbptr, char* path, char* key,
	int size) {

	return pdb_set_node(dbptr, path, key, &size, HASH_NODE_TYPE);
}

DLLEXP struct pdb_node_t* pdb_create_abstract(struct pdb* dbptr, char* path,
	char* key, void* data, void* free_cb) {

	struct pdb_node_t* nptr = NULL;
	nptr = pdb_set_node(dbptr, path, key, data, ABSTRACT_NODE_TYPE);
	
	if (!nptr)
		return NULL;

	pdb_set_free_method(dbptr, nptr, free_cb);

	return nptr;	
}

DLLEXP struct pdb_node_t* pdb_set_int(struct pdb* dbptr, char* path,
	char* key, int data) {

	return pdb_set_node(dbptr, path, key, &data, INT_NODE_TYPE);
}


/******************
 ******************
 ******************		Helper functions.
 ******************
 ******************/

	
/*
 *	Return type info struct by the given bitmask id.
 */
struct pdb_node_types_t* pdb_get_type_info(int type) {
	return (&pdb_types[type - 1]);
}


/*
 *	Return type info struct by the given opening token.
 */
struct pdb_node_types_t* pdb_get_type_info_otok(char* tok) {
	int i = 0;
	while (pdb_types[i].bitmask) {
		if (pdb_types[i].open_token) {
			if (tok_starts_with(tok, pdb_types[i].open_token))
				return (&pdb_types[i]);
		}
		++i;
	}
	return NULL;
}


/*
 *	Return type info struct by the given closing token.
 */
struct pdb_node_types_t* pdb_get_type_info_ctok(char* tok) {
	int i = 0;
	while (pdb_types[i].bitmask) {
		if (pdb_types[i].close_token) {
			if (tok_starts_with(tok, pdb_types[i].close_token))
				return (&pdb_types[i]);
		}
		++i;
	}
	return NULL;
}


/*
 *	Standard node creation function to be used for
 *	node creation callbacks if desired.
 *	This will create and set everything but the data pointer.
 *	It will also add the newly created child to the parent structure.
 */
void* pdb_standard_create_node(char* id, struct pdb_node_t* parent, int type) {
	struct pdb_node_t* nptr;
	struct pdb_node_types_t* tiptr;

	nptr = (struct pdb_node_t*)malloc(sizeof(struct pdb_node_t));
	if (!nptr) {
		ERROR("Unable to allocate memory for node structure.");
		return NULL;
	}
	
	nptr->data = NULL;
	nptr->parent = parent;
	nptr->type = type;
	nptr->custom_free_cb = NULL;
	
	if (id) {
		nptr->id = (char*)malloc(sizeof(char) * (strlen(id) + 1));
		strcpy(nptr->id, id);
	} else
		nptr->id = NULL;
	
	/*
	 *	Add child to parent structure.
	 */
	if (parent) {
		tiptr = pdb_get_type_info(parent->type);
		if (!tiptr->add_child_cb) {
			fprintf(stderr, "%s:%s():%i: Error: Unable to add child node to "
				"parent (type %i); not supported by parent.\n",
				__FILE__, __FUNCTION__, __LINE__, parent->type);
			free(nptr->id);
			free(nptr);
			return NULL;
		} else {
			if (!tiptr->add_child_cb(parent, id, nptr)) {
				fprintf(stderr, "%s:%s():%i: Error: Unable to add child node "
					"of type %i to parent node of type %i.\n",
					__FILE__, __FUNCTION__, __LINE__, tiptr->bitmask, type);
				free(nptr->id);
				free(nptr);
				return NULL;
			}
		}
	}

	return nptr;
}


/*
 *	Standard node loading function to be used for
 *	node load callbacks if desired.
 */
int pdb_standard_load_node(FILE* fptr, struct pdb_node_t* pptr,
	char** tok_arr, int* line) {
	
	int type = pptr->type;
	struct pdb_node_types_t* ctiptr = NULL;
	void* cptr = NULL;
	char* tok = pdb_get_token(fptr, &type, line);

	while (tok) {
		/*
		 *	Is the block over?
		 */
		if ((type & BLOCK_CLOSE) == BLOCK_CLOSE) {
			free(tok);
			return 1;
		}
		
		tok_arr = pdb_token_parse(tok);

		/*
		 *	Create the child node and add to parent.
		 */
		ctiptr = pdb_get_type_info(type);
		if (!ctiptr) {
			fprintf(stderr, "%s:%s():%i: Error: Unknown child type %i on line "
				"%i; halting database load.\n", __FILE__, __FUNCTION__,
				__LINE__, type, *line);
			free(tok);
			return 0;
		}
		cptr = ctiptr->create_cb(tok_arr[0], pptr, tok_arr);
		
		/*
		 *	Load the child node (if supported).
		 */
		if (ctiptr->load_cb) {
			if (!ctiptr->load_cb(fptr, cptr, tok_arr, line)) {
				fprintf(stderr, "%s:%s():%i: Error: An error occured while "
					"loading the database; halting database load on line %i.\n",
					__FILE__, __FUNCTION__, __LINE__, *line);
				token_free(tok_arr);
				free(tok);
				return 0;
			}
		}
		
		/*
		 *	Get next token.
		 */
		token_free(tok_arr);
		free(tok);
		type = pptr->type;
		tok = pdb_get_token(fptr, &type, line);
	}
	
	return 1;
}


/*
 *	Free a given node based on its type and using the corresponding
 *	free callback function.
 */
int pdb_free_node_cb(void* dptr) {
	struct pdb_node_t* nptr = dptr;
	struct pdb_node_types_t* tiptr;
	int ret = 0;

	if (nptr->custom_free_cb) {
		/*
		 *	Use the custom free callback rather than the default.
		 */
		ret = nptr->custom_free_cb(nptr->data);
	} else {
		tiptr = pdb_get_type_info(nptr->type);
		ret = tiptr->free_cb(nptr);
	}
	if (nptr->id)
		free(nptr->id);
	free(nptr);
	return ret;
}


/*
 *	Free a node and remove it from the parent.
 *
 *	Use this function and NOT pdb_node_types_t->free_cb()
 *	if you are deleting a single node from the structure
 *	(e.g. pdb_del rather than pdb_unload).
 */
DLLEXP int pdb_free_node(struct pdb_node_t* nptr) {
	struct pdb_node_types_t* tiptr;
	int ret = 0;

	if (!nptr)
		return 0;

	/*
	 *	Only free the container if it is
	 *	not of type LINK_NODE_TYPE.
	 */
	if (nptr->type != LINK_NODE_TYPE) {
		/*
		 *	Free data container.
		 */
		if (nptr->custom_free_cb) {
			/*
			 *	Use the custom free callback rather than the default.
			 */
			ret = nptr->custom_free_cb(nptr);
		} else {
			tiptr = pdb_get_type_info(nptr->type);
			ret = tiptr->free_cb(nptr);
		}
	}
	nptr->data = NULL;
	
	/*
	 *	Remove from parent.
	 */
	pdb_del_node_from_parent(nptr);

	/*
	 *	Free node.
	 */
	if (nptr->id)
		free(nptr->id);
	free(nptr);
	
	return ret;
}


/*
 *	Delete a node from its parent, but don't free the node.
 */
void pdb_del_node_from_parent(struct pdb_node_t* nptr) {
	struct pdb_node_types_t* tiptr;

	if (!nptr->parent)
		return;

	tiptr = pdb_get_type_info(nptr->parent->type);
	
	if (!tiptr->del_child_cb) {
		fprintf(stderr, "%s:%s():%i: Error: Cannot remove '%s' from parent; "
			"type %i does not support operation.\n",
			__FILE__, __FUNCTION__, __LINE__, nptr->id, nptr->parent->type);
		return;
	}
	
	tiptr->del_child_cb(nptr->parent, nptr);
}


/*
 *	Write a given node to disk.
 *
 *	Types without an open_token will have their write callbacks called
 *	without a need line prepended.
 *	Types without a close_token will not have the close_token and new line
 *	appended.
 */
int pdb_standard_write_node(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs) {

	struct pdb_node_types_t* tiptr;
	int i = 0;
	int ret;
	
	tiptr = pdb_get_type_info(nptr->type);

	if (!tiptr->write_cb) {
		char* trace = pdb_trace(nptr);
		fprintf(stderr, "%s:%s():%i: Warning: Unable to write node \"%s\" to "
			"disk; operation node supported by node type %i.\n",
			__FILE__, __FUNCTION__, __LINE__, trace, nptr->type);
		free(trace);
		return 1;
	}

	/*
	 *	Write tabs -- if any (if tabs != -1 [root]).
	 */
	if (tabs != -1)
		for (; i < tabs; i++)
			fputc('\t', fptr);
	
	/*
	 *	Write key in double quotes followed by an opening token.
	 */
	if (tabs != -1) {
		fputc('\"', fptr);
		if (nptr->id)
			fputs(nptr->id, fptr);
		fputs("\" ", fptr);
		if ((tiptr->write_as_block) && (tiptr->open_token)) {
			fputs(tiptr->open_token, fptr);
			fputc('\n', fptr);
		}
	}
	
	/*
	 *	Write node to disk.
	 */
	ret = tiptr->write_cb(dbptr, fptr, nptr, tabs + 1);
	
	/*
	 *	Write tabs and tree closing token (if tabs != -1 [root]).
	 */
	if ((tabs != -1) && (tiptr->close_token) && (tiptr->write_as_block)) {
		i = 0;
		for (; i < tabs; i++)
			fputc('\t', fptr);
		fputs(tiptr->close_token, fptr);
		fputc('\n', fptr);
	}
	
	return ret;	
}



/*
 *	Return string name of the node type.
 */
char* pdb_node_type_name(int type) {
	return (pdb_types[type - 1].str);
}


/******************
 ******************
 ******************		Binary Tree Node Type
 ******************
 ******************/


/*
 *	Callback for creating a new tree node.
 */
void* pdb_create_tree_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr) {

	struct pdb_node_t* nptr;
	struct binaryTree* tptr;

	nptr = pdb_standard_create_node(id, parent, TREE_NODE_TYPE);
	if (!nptr)
		return NULL;
	
	tptr = tree_create();
	if (!tptr) {
		ERROR("Unable to allocate memory for node data structure [tree].");
		free(nptr);
		return NULL;
	}
	nptr->data = (void*)tptr;
	
	return nptr;
}


/*
 *	Callback for loading a tree node.
 */
int pdb_load_tree_node_cb(FILE* fptr, struct pdb_node_t* pptr, char** tok_arr,
	int* line) {

	return pdb_standard_load_node(fptr, pptr, tok_arr, line);
}


/*
 *	Add a child to the parent structure.
 */
int pdb_add_tree_node_child_cb(struct pdb_node_t* parent, char* id,
	struct pdb_node_t* child) {
		
	struct binaryTree* tptr = parent->data;
	return tree_add_node(tptr, id, (void*)child);
}


/*
 *	Free a tree node.
 */
int pdb_free_tree_node_cb(struct pdb_node_t* nptr) {
	struct binaryTree* tptr = nptr->data;
	tree_free(tptr, 1, (void*)&pdb_free_node_cb);
	return 1;
}


/*
 *	Query a child node from the given binary tree.
 */
struct pdb_node_t* pdb_query_tree_node_cb(struct pdb_node_t* nptr, char* id) {
	struct binaryTree* tptr = nptr->data;
	return (struct pdb_node_t*)tree_get_node(tptr, id);
}		


/*
 *	Delete a child from the parent tree, but do not free child.
 */
void pdb_del_child_tree_node_cb(struct pdb_node_t* pptr,
	struct pdb_node_t* nptr) {

	struct binaryTree* tptr = pptr->data;
	tree_delink_node(tptr, nptr->id);
}


/*
 *	Write a binary tree node to disk.
 */
int pdb_write_tree_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs) {

	struct binaryTree* tptr;
	struct linkList* lptr;
	struct linkNode* lnptr;

	/*
	 *	Create a list from the tree so we can iterate through it.
	 */
	tptr = nptr->data;
	lptr = list_create();
	tree_to_list(tptr, lptr);
	lnptr = lptr->root;
	
	/*
	 *	Shuffle tree (if set).
	 */
	if (pdb_is_set(dbptr, PDB_WRITE_SHUFFLE))
		list_shuffle(lptr);
	
	/*
	 *	Set PDB_WRITE_NODE_FIRST_ID as the first in the list
	 *	if PDB_WRITE_NODE_FIRST is set.
	 *	Only if root node.
	 */
	if (!tabs) {
		if (pdb_is_set(dbptr, PDB_WRITE_NODE_FIRST)) {
			while (lnptr) {
				nptr = lnptr->data;
				if (!strcmp(nptr->id, PDB_WRITE_NODE_FIRST_ID)) {
					list_free_node(lptr, lnptr, 0, NULL);
					list_add_node_front(lptr, nptr);
					break;
				}
				lnptr = lnptr->next;
			}
		}
	}
	
	/*
	 *	Write all children to disk.
	 */
	lnptr = lptr->root;
	while (lnptr) {
		nptr = lnptr->data;
		if (!pdb_standard_write_node(dbptr, fptr, nptr, tabs)) {
			list_free(lptr, 0, NULL);
			return 0;
		}
		lnptr = lnptr->next;
	}
	
	/*
	 *	Free temp list (but not data).
	 */
	list_free(lptr, 0, NULL);
	
	return 1;
}


/*
 *	Return the number of children a binary tree has.
 */
int pdb_count_children_tree_cb(struct pdb_node_t* nptr) {
	return tree_count_children(nptr->data);
}


/******************
 ******************
 ******************		List Node Type
 ******************
 ******************/


/*
 *	Callback for creating a new list node.
 */
void* pdb_create_list_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr) {

	struct pdb_node_t* nptr;
	struct linkList* lptr;

	nptr = pdb_standard_create_node(id, parent, LIST_NODE_TYPE);
	if (!nptr)
		return NULL;
	
	lptr = list_create();
	if (!lptr) {
		ERROR("Unable to allocate memory for node data structure [list].");
		free(nptr);
		return NULL;
	}
	nptr->data = (void*)lptr;
	
	return nptr;
}


/*
 *	Callback for loading a list node.
 */
int pdb_load_list_node_cb(FILE* fptr, struct pdb_node_t* pptr, char** tok_arr,
	int* line) {

	return pdb_standard_load_node(fptr, pptr, tok_arr, line);
}


/*
 *	Add a child to the parent structure.
 */
int pdb_add_list_node_child_cb(struct pdb_node_t* parent, char* id,
	struct pdb_node_t* child) {
		
	struct linkList* lptr = parent->data;
	return (list_add_node_end(lptr, (void*)child) ? 1 : 0);
}


/*
 *	Free a list node.
 */
int pdb_free_list_node_cb(struct pdb_node_t* nptr) {
	struct linkList* lptr = nptr->data;
	list_free(lptr, 1, (void*)&pdb_free_node_cb);
	return 1;
}


/*
 *	Query a child node from the given list.
 */
struct pdb_node_t* pdb_query_list_node_cb(struct pdb_node_t* nptr, char* id) {
	struct linkList* lptr = nptr->data;
	struct linkNode* lnptr = lptr->root;

	while (lnptr) {
		nptr = lnptr->data;
		if (!strcmp(nptr->id, id))
			return nptr;
		lnptr = lnptr->next;
	}
	return NULL;
}		


/*
 *	Delete a child from the parent list, but do not free child.
 */
void pdb_del_child_list_node_cb(struct pdb_node_t* pptr,
	struct pdb_node_t* nptr) {

	struct linkList* lptr = pptr->data;
	struct linkNode* lnptr = lptr->root;
	
	while (lnptr) {
		if (lnptr->data == nptr) {
			list_delink_node(lptr, lnptr);
			return;
		}
	}
}


/*
 *	Write a link list node to disk.
 */
int pdb_write_list_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs) {

	struct linkList* lptr = nptr->data;
	struct linkNode* lnptr = lptr->root;
	
	/*
	 *	Set PDB_WRITE_NODE_FIRST_ID as the first in the list
	 *	if PDB_WRITE_NODE_FIRST is set.
	 *	Only if root node.
	 */
	if (!tabs) {
		if (pdb_is_set(dbptr, PDB_WRITE_NODE_FIRST)) {
			while (lnptr) {
				nptr = lnptr->data;
				if (!strcmp(nptr->id, PDB_WRITE_NODE_FIRST_ID)) {
					list_delink_node(lptr, lnptr);
					list_add_node_front(lptr, nptr);
					break;
				}
				lnptr = lnptr->next;
			}
		}
	}
	
	/*
	 *	Write all children to disk.
	 */
	lnptr = lptr->root;
	while (lnptr) {
		nptr = lnptr->data;
		if (!pdb_standard_write_node(dbptr, fptr, nptr, tabs))
			return 0;
		lnptr = lnptr->next;
	}
	
	return 1;
}


/*
 *	Return the number of children a list has.
 */
int pdb_count_children_list_cb(struct pdb_node_t* nptr) {
	struct linkList* lptr;
	struct linkNode* lnptr;
	int i = 0;
	
	lptr = nptr->data;
	lnptr = lptr->root;
	
	while (lnptr) {
		++i;
		lnptr = lnptr->next;
	}
	
	return i;
}


/******************
 ******************
 ******************		Hash Node Type
 ******************
 ******************/


/*
 *	Callback for creating a new hash node.
 */
void* pdb_create_hash_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr) {

	struct pdb_node_t* nptr;
	
	nptr = pdb_standard_create_node(id, parent, HASH_NODE_TYPE);
	if (!nptr)
		return NULL;
	
	/*
	 *	Due to the nature of the hash table, at this point it
	 *	can only be created from disk.
	 *	This is because we need the size, and the only viable
	 *	method of obtaining it is from the file itself.
	 *	For now, we leave the data NULL -- it will be created
	 *	on load :(
	 */
	nptr->data = NULL;
	
	return nptr;
}


/*
 *	Callback for loading a hash node.
 */
int pdb_load_hash_node_cb(FILE* fptr, struct pdb_node_t* pptr, char** tok_arr,
	int* line) {

	char b;
	int i;
	struct pdb_node_types_t* tiptr = pdb_get_type_info(HASH_NODE_TYPE);
		
	/*
	 *	Move to the next non-white character.
	 */
	while (!feof(fptr)) {
		FPEEK(&b, fptr);
		if (!is_whitespace(b))
			break;
		if (b == '\n')
			*line++;
		fgetc(fptr);
	}
	/*
	 *	Skip over the opening token.
	 */
	for (i = 0; i < (signed)strlen(tiptr->open_token); ++i)
		fgetc(fptr);
	
	/*
	 *	Get the size of the hash table.
	 */
	if (!fscanf(fptr, "%i;", &i)) {
		char* e = va(NULL,
			"Unable to load hash from file -- no size (line %i).\n", *line);
		ERROR(e);
		free(e);
		return 0;
	}
	
	pptr->data = (void*)hash_create(i);

	return pdb_standard_load_node(fptr, pptr, tok_arr, line);
}


/*
 *	Setting a hash should only be done in pdb_set().
 *	This will create the real hash table, where as
 *	pdb_create_hash_node_cb() only setup the node in the db.
 *
 *	The void* data is an int* of the hash size.
 */
void pdb_set_hash_node_cb(struct pdb_node_t* nptr, void* data) {
	int* dptr = (int*)data;
	
	nptr->data = (void*)hash_create(*dptr);
}


/*
 *	Add a child to the parent structure.
 */
int pdb_add_hash_node_child_cb(struct pdb_node_t* parent, char* id,
	struct pdb_node_t* child) {
		
	struct hash* hptr = parent->data;
	return hash_add(hptr, id, child);
}


/*
 *	Free a hash node.
 */
int pdb_free_hash_node_cb(struct pdb_node_t* nptr) {
	struct hash* hptr = nptr->data;
	hash_free(hptr, (void*)&pdb_free_node_cb);
	return 1;
}


/*
 *	Query a child node from the given hash.
 */
struct pdb_node_t* pdb_query_hash_node_cb(struct pdb_node_t* nptr, char* id) {
	struct hash* hptr = nptr->data;
	return ((struct pdb_node_t*)hash_get(hptr, id));
}		


/*
 *	Delete a child from the parent hash, but do not free child.
 */
void pdb_del_child_hash_node_cb(struct pdb_node_t* pptr,
	struct pdb_node_t* nptr) {

	struct hash* hptr = pptr->data;
	hash_remove(hptr, nptr->id);
}


/*
 *	Write a link list node to disk.
 */
int pdb_write_hash_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs) {

	struct hash* hptr = nptr->data;
	struct hash_node_t* hnptr = NULL;
	int i = 0;
	struct linkList* lptr = NULL;
	struct linkNode* lnptr = NULL;
	
	/*
	 *	Write the table size to disk.
	 */
	for (; i < tabs; i++)
		fputc('\t', fptr);

 	fprintf(fptr, "%i", hptr->size);
	fputc(PDB_TOKEN_TERM, fptr);
	fputc('\n', fptr);
	
	/*
	 *	Write all children to disk.
	 */
	lptr = NULL;
	lnptr = NULL;
	for (i = 0; i < hptr->size; i++) {
		lptr = hptr->tbl[i];
		if (!lptr)
			continue;
		lnptr = lptr->root;
		while (lnptr) {
			hnptr = lnptr->data;
			if (!pdb_standard_write_node(dbptr, fptr, hnptr->data, tabs))
				return 0;
			lnptr = lnptr->next;
		}
	}
	
	return 1;
}


/*
 *	Return the number of children a hash has.
 */
int pdb_count_children_hash_cb(struct pdb_node_t* nptr) {
	return hash_count_children(nptr->data);
}


/******************
 ******************
 ******************		String Node Type
 ******************
 ******************/


/*
 *	Callback for creating a new string node.
 *	If tok_arr is not NULL, tok_arr is just a char* casted to a char**.
 */
void* pdb_create_string_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr) {

	struct pdb_node_t* nptr;
		
	nptr = pdb_standard_create_node(id, parent, STRING_NODE_TYPE);
	if (!nptr)
		return NULL;

	nptr->data = NULL;
	
	if (tok_arr) {
		if (tok_arr[1]) {
			nptr->data = (char*)malloc(sizeof(char) * (strlen(tok_arr[1]) + 1));
			strcpy((char*)nptr->data, tok_arr[1]);
		}
	}

	return nptr;
}


/*
 *	(Re)set the data at a string node.
 */
void pdb_set_string_node_cb(struct pdb_node_t* nptr, void* data) {
	char* dptr = (char*)data;
	
	if (nptr->data)
		nptr->data = (char*)realloc(nptr->data, sizeof(char) *
			(strlen(dptr) + 1));
	else {
		if (!dptr)
			dptr = "";
		
		nptr->data = (char*)malloc(sizeof(char) * (strlen(dptr) + 1));
	}
	
	if (!nptr->data) {
		ERROR("Unable to allocate memory for string data.");
	} else
		strcpy(nptr->data, dptr);
}


/*
 *	Free a string node.
 */
int pdb_free_string_node_cb(struct pdb_node_t* nptr) {
	free(nptr->data);
	return 1;
}


/*
 *	Write a string node to disk.
 */
int pdb_write_string_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs) {

	/*
	 *	Write data in double quotes followed by a token terminator and new line.
	 *	If there no data, write token terminator and a new line.
	 */
	tabs = 0;			/* reuse as flag */
	if (nptr->data) {
		if (*(char*)(nptr->data)) {
			fputc('\"', fptr);
			fputs((const char*)nptr->data, fptr);
			fputc('\"', fptr);
			tabs = 1;
		}
	}
	if (!tabs)
		fseek(fptr, -1, SEEK_CUR);	/*	replace space with token terminator	*/
	fputc(PDB_TOKEN_TERM, fptr);
	fputc('\n', fptr);
	
	return 1;
}
	

/******************
 ******************
 ******************		Integer Node Type
 ******************
 ******************/


/*
 *	Callback for creating a new integer node.
 *	If tok_arr is not NULL, tok_arr is just an int* casted to a char**.
 */
void* pdb_create_int_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr) {

	struct pdb_node_t* nptr;
	
	nptr = pdb_standard_create_node(id, parent, INT_NODE_TYPE);
	if (!nptr)
		return NULL;

	nptr->data = (void*)malloc(sizeof(int));
	
	if (tok_arr)
		*(int*)(nptr->data) = *(int*)tok_arr;
	else
		*(int*)(nptr->data) = 0;
	
	return nptr;
}


/*
 *	Callback for loading an integer node.
 */
int pdb_load_int_node_cb(FILE* fptr, struct pdb_node_t* pptr, char** tok_arr,
	int* line) {

	struct pdb_node_types_t* tiptr;
	int* i = (int*)pptr->data;

	tiptr = pdb_get_type_info(INT_NODE_TYPE);

	/*
	 *	Read the integer from the file and set it.
	 */
	fscanf(fptr, tiptr->open_token, NULL);
	fscanf(fptr, "%i", i);
	fscanf(fptr, tiptr->close_token, NULL);
	
	return 1;
}


/*
 *	(Re)set the data at an integer node.
 */
void pdb_set_int_node_cb(struct pdb_node_t* nptr, void* data) {
	*(int*)(nptr->data) = *(int*)data;
}


/*
 *	Free an integer node.
 */
int pdb_free_int_node_cb(struct pdb_node_t* nptr) {
	free(nptr->data);
	return 1;
}


/*
 *	Write an integer node to disk.
 */
int pdb_write_int_node_cb(struct pdb* dbptr, FILE* fptr,
	struct pdb_node_t* nptr, int tabs) {
		
	struct pdb_node_types_t* tiptr;
		
	tiptr = pdb_get_type_info(INT_NODE_TYPE);
	
	fprintf(fptr, "%s%i%s%c\n",
		tiptr->open_token,
		*(int*)nptr->data,
		tiptr->close_token,
		PDB_TOKEN_TERM);		
	
	return 1;
}


/******************
 ******************
 ******************		Abstract Node Type
 ******************
 ******************/


/*
 *	Callback for creating a new abstract node.
 *	If tok_arr is not NULL, tok_arr is just a void* casted to a char**.
 */
void* pdb_create_abstract_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr) {

	struct pdb_node_t* nptr;

	nptr = pdb_standard_create_node(id, parent, ABSTRACT_NODE_TYPE);
	if (!nptr)
		return NULL;

	nptr->data = (void*)tok_arr;

	return nptr;
}


/*
 *	(Re)set the data at an abstract node.
 */
void pdb_set_abstract_node_cb(struct pdb_node_t* nptr, void* data) {
	nptr->data = data;
}


/*
 *	Free an abstract node.
 */
int pdb_free_abstract_node_cb(struct pdb_node_t* nptr) {
	fprintf(stderr, "Warning: %s:%s():%i: Abstract node cannot be freed "
			"-- no custom free method defined! [%s]\n",
			__FILE__, __FUNCTION__, __LINE__,
			pdb_trace(nptr));
	
	return 1;
}


/******************
 ******************
 ******************		Link Node Type
 ******************
 ******************/


/*
 *	Callback for creating a new link node.
 *
 *	This is a little different than normal creation
 *	callbacks -- the tok_arr is a pointer target
 *	node to be linked.
 */
void* pdb_create_link_node_cb(char* id, struct pdb_node_t* parent,
	char** tok_arr) {

	struct pdb_node_t* nptr;
	
	nptr = pdb_standard_create_node(id, parent, LINK_NODE_TYPE);
	if (!nptr)
		return NULL;

	nptr->data = tok_arr;

	return nptr;
}


/*
 *	Free a link node.
 */
int pdb_free_link_node_cb(struct pdb_node_t* nptr) {
	return 1;
}


/*
 *	Follow the link node.
 */
struct pdb_node_t* pdb_query_link_node_cb(struct pdb_node_t* nptr, char* id) {
	nptr = nptr->data;
	return (nptr ? nptr->data : NULL);
}
