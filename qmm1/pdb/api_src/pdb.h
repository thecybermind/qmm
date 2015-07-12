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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/api_src/pdb.h,v 1.2 2005/03/03 18:57:06 cybermind Exp $
 *
 *
 *
 *	This header is used in conjunction with pdb
 *	compiled as a shared library.
 *	You can download pdb at http://pdatabase.sourceforge.net/
 *
 */

#ifndef __PDB_H
#define __PDB_H

/*
 *	Enable verbose loading/unloading messages.
 */
//#define _PDB_VERBOSE


/*
 *	Using threads? Define this ONLY if you defined it
 *	within pdb, otherwise the structures will be different.
 */
//#define PDB_USING_THREADS


/*
 *	This callback can be used to override standard free
 *	callbacks for nodes.  It is especially useful for non-standard
 *	types like ABSTRACT_NODE_TYPE, where the actual data type may
 *	differ from node to node.
 */
struct pdb_node_t;
typedef int (*pdb_custom_free_cb_t)(void* nptr);


/*
 *	Single database node.
 */
struct pdb_node_t {
	int type;
	char* id;
	void* data;
	struct pdb_node_t* parent;
	pdb_custom_free_cb_t custom_free_cb;
};


/*
 *	Database root node.
 */
struct pdb {
	struct pdb_node_t* data;
	char* file;
	int altered;
	long last_write;
	int write_interval;		/*	in seconds	*/
	int settings;
	#ifdef PDB_USING_THREADS
	pthread_mutex_t* mutex;
	#endif
};

/*
 *	PDB types.
 */
enum {
	PDB_NO_NODE_TYPE,
	PDB_TREE_NODE_TYPE,
	PDB_LIST_NODE_TYPE,
	PDB_HASH_NODE_TYPE,
	PDB_STRING_NODE_TYPE,
	PDB_INT_NODE_TYPE,
	PDB_ABSTRACT_NODE_TYPE,
	PDB_LINK_NODE_TYPE
};

/*
 *	pdb setting value bitmasks.
 */
#define PDB_CASE_INSENSITIVE	1
#define PDB_WRITE_SHUFFLE		2
#define PDB_WRITE_NODE_FIRST	4
#define PDB_THREAD_SAFE			8

#ifdef WIN32
	#define pdb_dlopen(file,x) \
		LoadLibrary(file)
	#define pdb_dlsym(obj, func) \
		GetProcAddress((HMODULE)obj, func)
	#define pdb_dlclose(obj) \
		!FreeLibrary((HMODULE)obj)
#else
	#define pdb_dlopen dlopen
	#define pdb_dlsym dlsym
	#define pdb_dlclose dlclose
#endif

struct _pdb_funcs_t {
	struct pdb* (*_pdb_load_fptr)(char* file);
	int (*_pdb_unload_fptr)(struct pdb* dbptr);

	void (*_pdb_enable_fptr)(struct pdb* dbptr, int settings);
	void (*_pdb_disable_fptr)(struct pdb* dbptr, int settings);
	int (*_pdb_is_set_fptr)(struct pdb* dbptr, int setting);

	int (*_pdb_create_link_fptr)(struct pdb* dbptr, char* path, char* key,
		struct pdb_node_t* tnptr);

	struct pdb_node_t* (*_pdb_query_node_fptr)(struct pdb* dbptr, char* path);
	void* (*_pdb_query_fptr)(struct pdb* dbptr, char* path);

	struct pdb_node_t* (*_pdb_set_fptr)(struct pdb* dbptr, char* path,
		char* key, void* data);
	struct pdb_node_t* (*_pdb_set_node_fptr)(struct pdb* dbptr, char* path,
		char* key, void* data, int type);
	int (*_pdb_del_fptr)(struct pdb* dbptr, char* path);

	char* (*_pdb_trace_fptr)(struct pdb_node_t* nptr);

	void (*_pdb_set_write_interval_fptr)(struct pdb* dbptr, int seconds);
	int (*_pdb_need_write_fptr)(struct pdb* dbptr);
	int (*_pdb_write_fptr)(struct pdb* dbptr, char* file);

	void (*_pdb_set_free_method_fptr)(struct pdb* dbptr,
		struct pdb_node_t* nptr, void* free_cb);

	int (*_pdb_count_children_fptr)(struct pdb* dbptr, char* path);

	int (*_pdb_free_node_fptr)(struct pdb_node_t* nptr);

	/*
	 *	Extra wrappers for pdb_set_node()
	 */
	struct pdb_node_t* (*_pdb_create_tree_fptr)(struct pdb* dbptr, char* path,
		char* key);
	struct pdb_node_t* (*_pdb_create_list_fptr)(struct pdb* dbptr, char* path,
		char* key);
	struct pdb_node_t* (*_pdb_create_hash_fptr)(struct pdb* dbptr, char* path,
		char* key, int size);
	struct pdb_node_t* (*_pdb_create_abstract_fptr)(struct pdb* dbptr,
		char* path, char* key, void* data, void* free_cb);
	struct pdb_node_t* (*_pdb_set_int_fptr)(struct pdb* dbptr, char* path,
		char* key, int data);
};


/**********************************************
 *
 *	BEGIN - INTERNAL DATA STRUCTURES
 *
 **********************************************/
/*
 *	HASH TABLE
 */
/*
 *	A hash node.
 */
struct pdb_hash_node_t {
	char* id;
	void* data;
};
/*
 *	The root hash type.
 */
struct pdb_hash {
	int size;
	struct pdb_linkList** tbl;
};
/*
 *	Callback which can be passed to free_hash().
 */
typedef void (*pdb_hash_func_ptr)(void* dptr);

/*
 *	LINKED LIST
 */
enum pdb_list_add_seq {
	ADD_BEFORE,
	ADD_AFTER
};
typedef void (*pdb_list_func_ptr)(void* dptr);

/*	Individual Node
	Within The List		*/
struct pdb_linkNode {
	struct pdb_linkNode* prev;
	struct pdb_linkNode* next;
	void* data;
};
/*	List Structure		*/
struct pdb_linkList {
	struct pdb_linkNode* root;
	struct pdb_linkNode* last;
};

/*
 *	BINARY TREE
 */
typedef void (*pdb_tree_func_ptr)(void* dptr);
typedef int (*pdb_tree_str_cmp_cb)(char* s1, char* s2);
/*
 *	Individual Node
 *	Within The Tree
 */
struct pdb_treeNode {
	struct pdb_treeNode* right;
	struct pdb_treeNode* left;
	struct pdb_treeNode* parent;
	char* key;
	void* data;
};
/*
 *	Tree Structure
 */
struct pdb_binaryTree {
	struct pdb_treeNode* root;
	int flags;
	pdb_tree_str_cmp_cb _tree_str_cmp;
};

/**********************************************
 *
 *	END - INTERNAL DATA STRUCTURES
 *
 **********************************************/

#ifdef __cplusplus
extern "C"
{
#endif

int load_pdb_lib(char* file);
int unload_pdb_lib();
void recache_pdb_lib();

/*
 *	Function wrappers from library.
 */
struct pdb* pdb_load(char* file);
int pdb_unload(struct pdb* dbptr);

void pdb_enable(struct pdb* dbptr, int settings);
void pdb_disable(struct pdb* dbptr, int settings);
int pdb_is_set(struct pdb* dbptr, int setting);

int pdb_create_link(struct pdb* dbptr, char* path, char* key,
	struct pdb_node_t* tnptr);

struct pdb_node_t* pdb_query_node(struct pdb* dbptr, char* path);
void* pdb_query(struct pdb* dbptr, char* path);

struct pdb_node_t* pdb_set(struct pdb* dbptr, char* path, char* key,
	void* data);
struct pdb_node_t* pdb_set_node(struct pdb* dbptr, char* path, char* key,
	void* data, int type);
int pdb_del(struct pdb* dbptr, char* path);

char* pdb_trace(struct pdb_node_t* nptr);

void pdb_set_write_interval(struct pdb* dbptr, int seconds);
int pdb_need_write(struct pdb* dbptr);
int pdb_write(struct pdb* dbptr, char* file);

void pdb_set_free_method(struct pdb* dbptr, struct pdb_node_t* nptr,
	void* free_cb);

int pdb_count_children(struct pdb* dbptr, char* path);

int pdb_free_node(struct pdb_node_t* nptr);

/*
 *	Extra wrappers for pdb_set_node()
 */
struct pdb_node_t* pdb_create_tree(struct pdb* dbptr, char* path, char* key);
struct pdb_node_t* pdb_create_list(struct pdb* dbptr, char* path, char* key);
struct pdb_node_t* pdb_create_hash(struct pdb* dbptr, char* path, char* key,
	int size);
struct pdb_node_t* pdb_create_abstract(struct pdb* dbptr, char* path,
	char* key, void* data, void* free_cb);
struct pdb_node_t* pdb_set_int(struct pdb* dbptr, char* path, char* key,
	int data);

#ifdef __cplusplus
}
#endif

#endif /* __PDB_H */
