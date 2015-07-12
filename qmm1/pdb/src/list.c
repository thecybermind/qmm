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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/src/list.c,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include "list.h"

/*
 *	Allocate Memory For a
 *	New List Structure
 */
struct linkList* list_create() {
	struct linkList* lptr = NULL;

	lptr = (struct linkList*)malloc(sizeof(struct linkList));
	if (lptr != NULL) {
		lptr->root = NULL;
		lptr->last = NULL;
	}
	return lptr;
}


/*
 *	Free All Previously
 *	Allocated Memory For
 *	The Passed List,
 *	Including All Nodes
 *	and the Data Contained
 *	At Those Nodes.
 *
 *	If free_data is set
 *	to 0, the data will not
 *	be released.
 *
 *	If free_callback is not
 *	NULL and free_data is 1,
 *	use the given callback
 *	to free the data.
 */
int list_free(struct linkList* lptr, int free_data,
	list_func_ptr free_callback) {

	struct linkNode* tptr;
	struct linkNode* nptr;

	if (lptr == NULL)
		return 0;
	
	tptr = lptr->root;
	nptr = NULL;
	
	while (tptr) {
		nptr = tptr->next;
		/*	Free node.	*/
		if (free_data) {
			if (free_callback)
				/*
				 *	Use the given callback
				 *	to free the data.
				 */
				free_callback(tptr->data);
			else
				free(tptr->data);
		}
		free(tptr);
		tptr = nptr;
	}

	lptr->root = NULL;
	lptr->last = NULL;
	free(lptr);
	return 1;
}


/*
 *	DeLink The Passed Node
 *	From the List It Is Linked
 */
int list_delink_node(struct linkList* lptr, struct linkNode* nptr) {
	if (nptr == NULL)
		return 0;

	if (nptr->prev != NULL)
		nptr->prev->next = nptr->next;
	if (nptr->next != NULL)
		nptr->next->prev = nptr->prev;
	
	/* 	Correct Root and Last
	 	Node Pointers Of List
	 	If Required.			 */
	if (lptr->root == nptr)
		/*	Set Root Node as
			Removed Node's Next
			Node Pointer		*/
		lptr->root = nptr->next;
	if (lptr->last == nptr)
		/*	Set Last Node as
			Removed Node's Previous
			Node Pointer		*/
		lptr->last = nptr->prev;

	/*	Set the Node's Link
		Properties to NULL		*/
	nptr->next = NULL;
	nptr->prev = NULL;

	return 1;
}


/*
 *	DeLink Node and Free Memory
 *	Previously Allocated for
 *	the Data it Contains
 *
 *	If free_data is set
 *	to 0, the data will not
 *	be released.
 *
 *	If free_callback is not
 *	NULL and free_data is 1,
 *	use the given callback
 *	to free the data.
 */
int list_free_node(struct linkList* lptr, struct linkNode* nptr,
	int free_data, list_func_ptr free_callback) {
	
 	if (!list_delink_node(lptr, nptr))
		return 0;
	
	if (free_data) {
		if (free_callback)
			/*
			 *	Use the given callback
			 *	to free the data.
			 */
			free_callback(nptr->data);
		else
			free(nptr->data);
	}

	free(nptr);
	return 1;
}


/*
 *	Free the First Node in the
 *	Passed Link List
 */
int list_free_front(struct linkList* lptr, list_func_ptr free_callback) {
	return (list_free_node(lptr, lptr->root, 1, free_callback));
}


/*
 *	Free the Last Node in the
 *	Passed Link List
 */
int list_free_last(struct linkList* lptr, list_func_ptr free_callback) {
	return (list_free_node(lptr, lptr->last, 1, free_callback));
}


/*
 *	Initialize a Node:
 *	Allocate Memory
 *	Set Pointers as NULL
 *	Set Data Pointer as Passed
 */
void* list_create_node(void* dptr) {
	struct linkNode* nptr = NULL;
	
	nptr = (struct linkNode*)malloc(sizeof(struct linkNode));
	if (nptr == NULL)
		/*	Out of Memory	*/
		return NULL;

	/*	Set Node Data			*/
	nptr->data = dptr;
	nptr->prev = NULL;
	nptr->next = NULL;

	return nptr;
}


/*
 *	Add a Node to the Front
 *	Of the Passed List
 *	Return a Pointer to the Data
 */
void* list_add_node_front(struct linkList* lptr, void* dptr) {
	struct linkNode* nptr;

	if (lptr == NULL)
		return NULL;

	/*	Create the Node			*/
	nptr = list_create_node(dptr);

	/*	Link the Node into The List	*/
	if (lptr->root == NULL) {
		/*	Set as Root Node	*/
		lptr->root = nptr;
		lptr->last = nptr;
	} else {
		/*	Link Node as Root	*/
		nptr->next = lptr->root;
		lptr->root->prev = nptr;
		lptr->root = nptr;
	}
	return nptr->data;
}


/*
 *	Add a Node to the End
 *	Of the Passed List
 *	Return a Pointer to the Data
 */
void* list_add_node_end(struct linkList* lptr, void* dptr) {
	struct linkNode* nptr;

	if (lptr == NULL)
		return NULL;

	/*	Create the Node			*/
	nptr = list_create_node(dptr);

	/*	Link the Node into The List	*/
	if (lptr->root == NULL) {
		/*	Set as Root Node	*/
		lptr->root = nptr;
		lptr->last = nptr;
	} else {
		/*	Link Node as Last	*/
		lptr->last->next = nptr;
		nptr->prev = lptr->last;
		lptr->last = nptr;
	}
	return nptr->data;
}


/*
 *	Add a Node to the Middle
 *	Of the Passed List
 *	The Node will Be Added
 *	AFTER the passed lnptr
 *	node pointer.
 *	Return a Pointer to the Data
 *
 *	position:
 *		ADD_BEFORE 	before the given lnptr node
 *		ADD_AFTER	after the given lnptr node
 */
void* list_add_node_middle(struct linkList* lptr,
	struct linkNode* lnptr, void* dptr, int position) {

	struct linkNode* nptr;

	if (lnptr == NULL)
		return NULL;
	if ((position != ADD_BEFORE) && (position != ADD_AFTER))
		return NULL;

	/*	Create the Node			*/
	nptr = list_create_node(dptr);

	/*	Link the Node into The List	*/
	if (lptr->root == NULL) {
		/*	Set as Root Node	*/
		lptr->root = nptr;
		lptr->last = nptr;
	} else if (lnptr->next != NULL) {
		/*	Link Node in Middle	*/
		if (position == ADD_AFTER) {
			/*	link before	*/
			lnptr->next->prev = nptr;
			nptr->next = lnptr->next;
			lnptr->next = nptr;
			nptr->prev = lnptr;
		} else {
			/*	link after	*/
			lnptr->prev->next = nptr;
			nptr->prev = lnptr->prev;
			nptr->next = lnptr;
			lnptr->prev = nptr;
		}
	} else {
		/*	Link Node as Last	*/
		lptr->last->next = nptr;
		nptr->prev = lptr->last;
		lptr->last = nptr;
	}
	return nptr->data;
}


/*
 *	Return void* to Data at
 *	the Node in List Passed
 */
void* list_get_data(struct linkNode* nptr) {
	if (nptr->data == NULL)
		return NULL;
	return (nptr->data);
}


/*
 *	Shuffle the list.
 */
void list_shuffle(struct linkList* lptr) {
	struct linkNode* lnptr = lptr->root;
	int nodes = 1, node = 0;
	struct linkNode** node_arr;
	int a = 0, b = 0;
	void* dptr = NULL;

	while (lnptr) {
		++nodes;
		lnptr = lnptr->next;
	}

	node_arr = (struct linkNode**)malloc(sizeof(struct linkNode*) * nodes);
	
	lnptr = lptr->root;
	node = 0;
	for (; node < (nodes - 1); node++) {
		node_arr[node] = lnptr;
		lnptr = lnptr->next;
	}
	node_arr[node] = NULL;
	
	node = (int)((nodes - 1) * .65 );
	srand(time(NULL) * node);
	for (; node > 0; node--) {
		a = (rand() % (nodes - 1));
		b = (rand() % (nodes - 1));
		dptr = node_arr[a]->data;
		node_arr[a]->data = node_arr[b]->data;
		node_arr[b]->data = dptr;
	}
	
	free(node_arr);
}
