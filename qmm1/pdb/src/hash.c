/*
 *	This file is part of pdb.
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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/src/hash.c,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */

/*
 *	String hash table.
 *
 *	The hash table uses a link list chainer to resolve collisions.
 *	When the table is created, the lists are not.
 *	A list is created when an item needs to go into an empty
 *	cell weather the list is needed or not.
 *	If an entry is deleted, and that was the last entry in the list
 *	for that cell, the list is not deleted.  The list will be deallocated
 *	when the hash table has been explicitly freed.
 *	The idea is to create only what is needed, but keep efficiency up.
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "list.h"
#include "hash.h"

static int hash_key(char* str, int size);

/*
 *	Create a hash table of the given size.
 */
struct hash* hash_create(int size) {
	int i = 0;
	struct hash* hptr = (struct hash*)malloc(sizeof(struct hash));
		
	if (!hptr)
		return 0;
	
	if (!size) {
		/*
		 *	Cannot create hash of size 0 -- default to 1.
		 */
		fprintf(stderr, "%s: WARNING: Hash of size 0 not allowed, using "
			"1 bucket.\n", __FILE__);
		size = 1;
	}

	hptr->size = size;	
	hptr->tbl = (struct linkList**)malloc(sizeof(struct linkList*) * size);
	
	for (i = 0; i < size; i++)
		hptr->tbl[i] = NULL;
	
	return hptr;
}


/*
 *	Free a hash table.
 *	If NULLis passed for free_callback, the data will
 *	be deallocated with free(); otherwise free_callback
 *	will be invoked with the void* data as the only parameter.
 */
void hash_free(struct hash* hptr, hash_func_ptr free_callback) {
	struct linkNode* lnptr;
	struct hash_node_t* nptr;
	int i = 0;
	
	for (; i < hptr->size; i++) {
		if (hptr->tbl[i]) {
			/*
			 *	Free nodes in list.
			 *	This must be used rather than list_free()
			 *	because there is an extra data structure
			 *	specific to the hash table which must be freed.
			 */
			lnptr = hptr->tbl[i]->root;
			nptr = NULL;
			while (lnptr) {
				nptr = lnptr->data;
				if (nptr) {
					free(nptr->id);
					if (free_callback)
						free_callback(nptr->data);
					else
						free(nptr->data);
					free(nptr);
				}
				lnptr = lnptr->next;
			}
			
			/*
			 *	Free list.
			 */
			list_free(hptr->tbl[i], 0, NULL);
		}
	}
	free(hptr->tbl);
	free(hptr);
}


/*
 *	STATIC
 *	Generate a (hopefully) distributed integer between
 *	0 and size based on the passed str.
 */
int hash_key(char* str, int size) {
	unsigned int ret = 1;
	while (*str)
		ret = ((ret >> 1) + (ret * *str++));
	return (ret % size);
}


/*
 *	Add dptr to the hash table using key as the id.
 */
int hash_add(struct hash* hptr, char* key, void* dptr) {
	struct hash_node_t* nptr;
	int i = hash_key(key, hptr->size);
	
	/*
	 *	Does a list exist at this node yet?
	 *	If not create one.
	 */
	if (!hptr->tbl[i])
		hptr->tbl[i] = list_create();
	
	/*
	 *	Create the entry node.
	 */
	nptr = (struct hash_node_t*)malloc(sizeof(struct hash_node_t));
	nptr->id = (char*)malloc(sizeof(char) * (strlen(key) + 1));
	strcpy(nptr->id, key);
	nptr->data = dptr;
	
	/*
	 *	Add node to list.
	 */
	list_add_node_end(hptr->tbl[i], (void*)nptr);

	return 1;
}


/*
 *	Query an entry -- return the data pointer.
 */
void* hash_get(struct hash* hptr, char* key) {
	struct hash_node_t* nptr = hash_get_node(hptr, key);
	return (nptr ? nptr->data : NULL);
}


/*
 *	Query an entry -- return the entry node.
 */
struct hash_node_t* hash_get_node(struct hash* hptr, char* key) {
	struct linkNode* lnptr;
	struct hash_node_t* nptr;
	int i = hash_key(key, hptr->size);

	if (!hptr->tbl[i])
		return NULL;

	lnptr = hptr->tbl[i]->root;
	nptr = NULL;
	while (lnptr) {
		nptr = lnptr->data;
		if (!strcmp(nptr->id, key))
			return nptr;
		lnptr = lnptr->next;
	}
	
	return NULL;
}


/*
 *	Delete an entry.
 */
int hash_del(struct hash* hptr, char* key, hash_func_ptr free_callback) {
	struct linkNode* lnptr;
	struct hash_node_t* nptr;
	int i = hash_key(key, hptr->size);

	if (!hptr->tbl[i])
		return 0;

	/*
	 *	Find the node in the list.
	 */
	lnptr = hptr->tbl[i]->root;
	nptr = NULL;
	while (lnptr) {
		nptr = lnptr->data;
		if (!strcmp(nptr->id, key))
			break;
		lnptr = lnptr->next;
		nptr = NULL;
	}
	
	if (!nptr)
		return 0;

	/*
	 *	Remove the node from the list.
	 */
	list_free_node(hptr->tbl[i], lnptr, 0, NULL);
	
	/*
	 *	Free the node.
	 */
	free(nptr->id);
	if (free_callback)
		free_callback(nptr->data);
	else
		free(nptr->data);
	free(nptr);
	
	return 1;
}


/*
 *	Remove an entry, but do not free the data.
 */
int hash_remove(struct hash* hptr, char* key) {
	struct linkNode* lnptr;
	struct hash_node_t* nptr;
	int i = hash_key(key, hptr->size);

	if (!hptr->tbl[i])
		return 0;

	/*
	 *	Find the node in the list.
	 */
	lnptr = hptr->tbl[i]->root;
	nptr = NULL;
	while (lnptr) {
		nptr = lnptr->data;
		if (!strcmp(nptr->id, key))
			break;
		lnptr = lnptr->next;
		nptr = NULL;
	}
	
	if (!nptr)
		return 0;

	/*
	 *	Remove the node from the list.
	 */
	list_free_node(hptr->tbl[i], lnptr, 0, NULL);
	
	/*
	 *	Free the node, but not the data.
	 */
	free(nptr->id);
	free(nptr);
	
	return 1;
}


/*
 *	Return the number of children a hash table has.
 */
int hash_count_children(struct hash* hptr) {
	int i = 0;
	int b = 0;
	struct linkList* lptr;
	struct linkNode* lnptr;
	
	for (; b < hptr->size; b++) {
		if (hptr->tbl[b]) {
			lptr = hptr->tbl[b];
			lnptr = lptr->root;
			while (lnptr) {
				++i;
				lnptr = lnptr->next;
			}
		}
	}
	
	return i;
}
