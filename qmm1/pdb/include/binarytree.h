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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/include/binarytree.h,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */

 
#ifndef _BINARYTREE_H
#define _BINARYTREE_H

#include "list.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*tree_func_ptr)(void* dptr);
typedef int (*tree_str_cmp_cb)(char* s1, char* s2);
	
/*
 *	Tree flags.
 */
#define TREE_FLAG_DEFAULT	0
#define TREE_FLAG_NO_CASE	1
	
/*
 *	Individual Node
 *	Within The Tree
 */
struct treeNode {
	struct treeNode* right;
	struct treeNode* left;
	struct treeNode* parent;
	char* key;
	void* data;
};

/*
 *	Tree Structure
 */
struct binaryTree {
	struct treeNode* root;
	int flags;
	tree_str_cmp_cb _tree_str_cmp;
};

struct binaryTree* tree_create();
void tree_free(struct binaryTree* tptr, int free_data,
	tree_func_ptr free_callback);
void tree_free_subtree(struct treeNode* nptr, int free_data,
	tree_func_ptr free_callback);

int tree_add_node(struct binaryTree* tptr, char* key, void* data);
void* tree_get_node(struct binaryTree* tptr, char* key);

void* tree_delink_node(struct binaryTree* tptr, char* key);
int tree_free_node(struct binaryTree* tptr, char* key,
	tree_func_ptr free_callback);

void tree_debug(struct treeNode* nptr);
void tree_to_list(struct binaryTree* tptr, struct linkList* lptr);
void tree_node_to_list(struct treeNode* nptr, struct linkList* lptr);

void tree_enable(struct binaryTree* tptr, int flags);
void tree_disable(struct binaryTree* tptr, int flags);

void tree_set_strcmp(struct binaryTree* tptr, tree_str_cmp_cb _strcmp);
int _tree_lower_strcmp(char* s1, char* s2);

int tree_count_children(struct binaryTree* tptr);

#ifdef __cplusplus
}
#endif

#endif /* _BINARYTREE_H */
