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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/include/list.h,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */

 
#ifndef _LIST_H
#define _LIST_H

enum list_add_seq {
	ADD_BEFORE,
	ADD_AFTER
};

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*list_func_ptr)(void* dptr);

/*	Individual Node
	Within The List		*/
struct linkNode {
	struct linkNode* prev;
	struct linkNode* next;
	void* data;
};

/*	List Structure		*/
struct linkList {
	struct linkNode* root;
	struct linkNode* last;
};

struct linkList* list_create();
int list_free(struct linkList* lptr, int free_data, list_func_ptr free_callback);
int list_delink_node(struct linkList* lptr, struct linkNode* nptr);
int list_free_node(struct linkList* lptr, struct linkNode* nptr,
	int free_data, list_func_ptr free_callback);
int list_free_front(struct linkList* lptr, list_func_ptr free_callback);
int list_free_last(struct linkList* lptr, list_func_ptr free_callback);

void* list_create_node(void* dptr);

void* list_add_node_front(struct linkList* lptr, void* dptr);
void* list_add_node_end(struct linkList* lptr, void* dptr);
void* list_add_node_middle(struct linkList* lptr,
	struct linkNode* lnptr, void* dptr, int position);

void* list_get_data(struct linkNode* nptr);
	
void list_shuffle(struct linkList* lptr);

#ifdef __cplusplus
}
#endif

#endif /* _LIST_H */
