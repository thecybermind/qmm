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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/src/binarytree.c,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */

 /*
  *	IMPORTANT:
  *		Data added should be allocated prior to adding
  *		via malloc() as upon freeing the tree data
  *		will be released back via free().
  *
  *		All keys are character arrays.
  */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "list.h"
#include "binarytree.h"

static void tree_branch_count_children(struct treeNode* nptr, int* i);

/*
 *	Create a binary tree.
 */
struct binaryTree* tree_create() {
	struct binaryTree* tptr = (struct binaryTree*)
		malloc(sizeof(struct binaryTree));

	if (tptr == NULL) {
		/*	Out of Memory	*/
		#ifdef TREE_DEBUG
			printf("binarytree.c: tree_create(): Failed creating tree; "
					"out of memory.\n");
		#endif
		return NULL;
	}
	tptr->root = NULL;
	tptr->flags = TREE_FLAG_DEFAULT;
	tptr->_tree_str_cmp = (void*)&strcmp;
	return tptr;	
}


/*
 *	Free All Previously
 *	Allocated Memory For
 *	The Passed Tree,
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
void tree_free(struct binaryTree* tptr, int free_data,
	tree_func_ptr free_callback) {

	if (tptr != NULL)
		tree_free_subtree(tptr->root, free_data, free_callback);
	free(tptr);
}


/*
 *	Free Previously Allocated
 *	Memory for The Passed Node
 *	and all Nodes which stemmed
 *	from that Node (resursive)
 *	and the Data Contained At
 *	those Nodes.
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
void tree_free_subtree(struct treeNode* nptr, int free_data,
	tree_func_ptr free_callback) {

	if (nptr == NULL)
		return;
	
	tree_free_subtree(nptr->right, free_data, free_callback);
	tree_free_subtree(nptr->left, free_data, free_callback);
	
	free(nptr->key);
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
}


/*
 *	Add a node to the tree.
 *	The data pointer at the added
 *	node will be set to the data
 *	pointer passed.
 *	Return 1 on success, 0 on failure.
 *	Notes:
 *		NO _DATA_ ALLOCATION WILL BE DONE
 */
int tree_add_node(struct binaryTree* tptr, char* key, void* data) {
	struct treeNode* pptr;
	struct treeNode* nptr;

	if (tptr == NULL)
		return 0;
	
	pptr = tptr->root;
	nptr = NULL;

	/*	locate parent node	*/
	while (pptr != NULL) {
		if (tptr->_tree_str_cmp(key, pptr->key) < 0) {
			if (pptr->left == NULL)
				break;
			pptr = pptr->left;
		} else if (tptr->_tree_str_cmp(key, pptr->key) > 0) {
			if (pptr->right == NULL)
				break;
			pptr = pptr->right;
		} else
			return 0;
	}

	if (pptr == NULL) {
		/*	root node	*/
		tptr->root = (struct treeNode*)malloc(sizeof(struct treeNode));
		if (tptr->root == NULL) {
			/*	out of memory	*/
			#ifdef TREE_DEBUG
				printf("binarytree.c: tree_add_node(): "
					"Failed allocating root node; "
					"out of memory.\n");
			#endif			
			return 0;
		}
		tptr->root->parent = NULL;
		nptr = tptr->root;
	} else if (tptr->_tree_str_cmp(key, pptr->key) < 0) {
		/*	left child	*/
		pptr->left = (struct treeNode*)malloc(sizeof(struct treeNode));
		if (pptr->left == NULL) {
			/*	out of memory	*/
			#ifdef TREE_DEBUG
				printf("binarytree.c: tree_create(): \
					Failed allocating left child; \
					 out of memory.\n");
			#endif
			return 0;
		}
		pptr->left->parent = pptr;
		nptr = pptr->left;
	} else {
		/*	right child	*/
		pptr->right = (struct treeNode*)malloc(sizeof(struct treeNode));
		if (pptr->right == NULL) {
			/*	out of memory	*/
			#ifdef TREE_DEBUG
				printf("binarytree.c: tree_create(): "
					"Failed allocating right child; "
					"out of memory.\n");
			#endif
			return 0;
		}
		pptr->right->parent = pptr;
		nptr = pptr->right;
	}
	
	nptr->right = NULL;
	nptr->left = NULL;
	nptr->data = data;

	nptr->key = (char*)malloc(strlen(key) * sizeof(char) + sizeof(char));
	if (nptr->key == NULL) {
		/*	out of memory	*/
		#ifdef TREE_DEBUG
			printf("binarytree.c: tree_create(): "
				"Failed allocating node key; "
				"out of memory.\n");
		#endif
		exit(1);
	}
	strcpy(nptr->key, key);
	
	return 1;
}


/*
 *	Search the tree for the passed key
 *	and return the data at that node.
 */
void* tree_get_node(struct binaryTree* tptr, char* key) {
	struct treeNode* nptr;
	int cmp;

	if (tptr == NULL)
		return NULL;
	
	nptr = tptr->root;

	/*	locate parent node	*/
	while (nptr != NULL) {
		cmp = tptr->_tree_str_cmp(key, nptr->key);
		if (cmp < 0) {
			nptr = nptr->left;
		} else if (cmp > 0) {
			nptr = nptr->right;
		} else {
			return nptr->data;
		}
	}
	return NULL;
}


/*
 *	Delink node from tree,
 *	free all the node except
 *	the data.
 *	Return a pointer to the
 *	nodes data.
 */
void* tree_delink_node(struct binaryTree* tptr, char* key) {
	struct treeNode* nptr;
	struct treeNode* rptr;
	void* dptr;

	if (tptr == NULL)
		return 0;
	
	nptr = tptr->root;

	/*	locate node	*/
	while (nptr != NULL) {
		if (tptr->_tree_str_cmp(key, nptr->key) < 0) {
			nptr = nptr->left;
		} else if (tptr->_tree_str_cmp(key, nptr->key) > 0) {
			nptr = nptr->right;
		} else
			break;
	}
	if (nptr == NULL)
		/*	node not found	*/
		return NULL;
	
	dptr = nptr->data;
	
	/*	delink node	*/
	if (nptr->left == NULL) {
		/*	no left child	*/
		if (nptr->right == NULL) {
			/*	no children	*/
			if (nptr->parent != NULL) {
				if (nptr->parent->right == nptr)
					nptr->parent->right = NULL;
				else if (nptr->parent->left == nptr)
					nptr->parent->left = NULL;
			} else
				tptr->root = NULL;
		} else {
			/*	only right child	*/
			nptr->right->parent = nptr->parent;
			if (nptr->parent != NULL) {
				if (nptr->parent->right == nptr)
					nptr->parent->right = nptr->right;
				else if (nptr->parent->left == nptr)
					nptr->parent->left = nptr->right;
			} else
				tptr->root = nptr->right;
		}
	} else if (nptr->right == NULL) {
		/*	only left child	*/
		nptr->left->parent = nptr->parent;
		if (nptr->parent != NULL) {
			if (nptr->parent->right == nptr)
				nptr->parent->right = nptr->left;
			else if (nptr->parent->left == nptr)
				nptr->parent->left = nptr->left;
		} else
			tptr->root = nptr->left;
	} else {
		/*	two children	*/
		if (nptr->right->left == NULL) {
			/*	right child takes place	*/
			nptr->right->parent = nptr->parent;
			if (nptr->parent != NULL) {
				if (nptr->parent->right == nptr)
					nptr->parent->right = nptr->right;
				else if (nptr->parent->left == nptr)
					nptr->parent->left = nptr->right;
			} else
				tptr->root = nptr->right;
			/*	follow right childs left children to NULL	*/
			rptr = nptr->right;
			while (rptr->left != NULL)
				rptr = rptr->left;
			/*	put left child here	*/
			rptr->left = nptr->left;
		} else {
			/*	follow right childs left children to NULL	*/
			rptr = nptr->right->left;
			while (rptr->left != NULL)
				rptr = rptr->left;
			if (nptr->parent != NULL) {
				if (nptr->parent->right == nptr)
					nptr->parent->right = rptr;
				else if (nptr->parent->left == nptr)
					nptr->parent->left = rptr;
			} else
				tptr->root = rptr;
			if (rptr->parent->right == rptr)
				rptr->parent->right = NULL;
			else if (rptr->parent->left == rptr)
				rptr->parent->left = NULL;
			rptr->parent = nptr->parent;
			rptr->right = nptr->right;
			rptr->left = rptr->left;
			/*	put left child here	*/
			rptr->left = nptr->left;
		}
	}

	free(nptr->key);
	free(nptr);
	
	return dptr;
}


/*
 *	Delete node from tree,
 *	free all the node including
 *	the data.
 *	Return 1 if successful.
 */
int tree_free_node(struct binaryTree* tptr, char* key,
	tree_func_ptr free_callback) {
		
	void* dptr = tree_delink_node(tptr, key);
	if (dptr == NULL)
		return 0;
	
	if (free_callback)
		free_callback(dptr);
	else
		free(dptr);
	return 1;
}

void tree_debug(struct treeNode* nptr) {
	if (nptr == NULL)
		return;
	
	printf("[tree debug] address:%lx\tkey: _%s_\n",
		(unsigned long)nptr, nptr->key);

	//printf("%s right\n", nptr->key);
	tree_debug(nptr->right);
	//printf("%s left\n", nptr->key);
	tree_debug(nptr->left);
}


/*
 *	Dump a tree to a list.
 *	WARNING:
 *	  MAKE SURE THE LIST IS FREED
 *	  WITH free_data DISABLED!
 */
void tree_to_list(struct binaryTree* tptr, struct linkList* lptr) {
	tree_node_to_list(tptr->root, lptr);
}


/*
 *	Dump a tree to a list.
 */
void tree_node_to_list(struct treeNode* nptr, struct linkList* lptr) {
	if (nptr == NULL)
		return;
	
	tree_node_to_list(nptr->right, lptr);
	tree_node_to_list(nptr->left, lptr);
	
	list_add_node_end(lptr, nptr->data);
}


/*
 *	Enable tree setting(s).
 *	settings are OR'ed togther.
 */
void tree_enable(struct binaryTree* tptr, int flags) {
	tptr->flags |= flags;

	tree_set_strcmp(tptr, NULL);
}


/*
 *	Disable tree setting(s).
 *	settings are OR'ed togther.
 */
void tree_disable(struct binaryTree* tptr, int flags) {
	tptr->flags &= ~flags;

	tree_set_strcmp(tptr, NULL);
}


/*
 *	Set what string compare function to use.
 *	If _str_cmp is NULL, set dependent on tree flags.
 */
void tree_set_strcmp(struct binaryTree* tptr, tree_str_cmp_cb _strcmp) {
	if (_strcmp) {
		tptr->_tree_str_cmp = _strcmp;
		return;
	}
	
	/*
	 *	Set dependent on tree flags.
	 */
	if ((tptr->flags & TREE_FLAG_NO_CASE) == TREE_FLAG_NO_CASE)
		tptr->_tree_str_cmp = (void*)&_tree_lower_strcmp;
	else
		tptr->_tree_str_cmp = (void*)&strcmp;
}

/*
 *	Case insensitive string compare.
 */
int _tree_lower_strcmp(char* s1, char* s2) {
	if (!s1 && s2)
		return 1;
	else if (s1 && !s2)
		return -1;
	else if (!s1 && !s2)
		return 0;
	
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
 *	Return the number of children a binary tree has.
 */
int tree_count_children(struct binaryTree* tptr) {
	int i = 0;
	tree_branch_count_children(tptr->root, &i);
	return i;
}


/*
 *	Count children on a given branch.
 */
void tree_branch_count_children(struct treeNode* nptr, int* i) {
	/*
	 *	Count children on left branch.
	 */
	tree_branch_count_children(nptr->left, i);

	/*
	 *	Count children on right branch.
	 */
	tree_branch_count_children(nptr->right, i);
	
	(*i)++;
}
