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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/api_src/pdb.c,v 1.2 2005/03/03 18:57:06 cybermind Exp $
 *
 *
 *
 *	This header is used in conjunction with pdb
 *	compiled as a shared library.
 *	You can download pdb at http://pdatabase.sourceforge.net/
 *
 */

#define __PDB_C

#include <stdio.h>
#include "pdb.h"

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

static void* _pdb_obj;
static struct _pdb_funcs_t _pdb_funcs = {
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL
};

/*
 *	Load the pdb shared object.
 */
int load_pdb_lib(char* file) {
	#ifdef _PDB_VERBOSE
	printf("INFO: pdb.c: Loading pdb library \"%s\"... ", file);
	#endif

	_pdb_obj = pdb_dlopen(file, RTLD_NOW);

	#ifdef _PDB_VERBOSE
	printf("%s\n", (_pdb_obj ? "success" : "failed"));
	if (!_pdb_obj)
		printf("dlerror(): %s\n", dlerror());
	#endif

	recache_pdb_lib();

	return (_pdb_obj ? 1 : 0);
}


/*
 *	Unload the pdb shared object.
 */
int unload_pdb_lib() {
	int r;

	if (!_pdb_obj)	return 0;

	#ifdef _PDB_VERBOSE
	printf("INFO: pdb.c: Unloading pdb library... ");
	#endif

	r = pdb_dlclose(_pdb_obj);

	#ifdef _PDB_VERBOSE
	printf("%s\n", (r ? "failed" : "success"));
	#endif

	return (r ? 0 : 1);
}


/*
 *	(Re)cache the pdb shared object function pointers.
 */
void recache_pdb_lib() {
	if (!_pdb_obj)	return;

	#ifdef _PDB_VERBOSE
	printf("INFO: pdb.c: Caching pdb function pointers...\n");
	#endif

	_pdb_funcs._pdb_load_fptr = (struct pdb* (*)(char*))pdb_dlsym(_pdb_obj, "pdb_load");
	_pdb_funcs._pdb_unload_fptr = (int (*)(struct pdb*))pdb_dlsym(_pdb_obj, "pdb_unload");
	_pdb_funcs._pdb_enable_fptr = (void (*)(struct pdb*, int))pdb_dlsym(_pdb_obj, "pdb_enable");
	_pdb_funcs._pdb_disable_fptr = (void (*)(struct pdb*, int))pdb_dlsym(_pdb_obj, "pdb_disable");
	_pdb_funcs._pdb_is_set_fptr = (int (*)(struct pdb*, int))pdb_dlsym(_pdb_obj, "pdb_is_set");
	_pdb_funcs._pdb_create_link_fptr = (int (*)(struct pdb*, char*, char*, struct pdb_node_t*))pdb_dlsym(_pdb_obj, "pdb_create_link");
	_pdb_funcs._pdb_query_node_fptr = (struct pdb_node_t* (*)(struct pdb*, char*))pdb_dlsym(_pdb_obj, "pdb_query_node");
	_pdb_funcs._pdb_query_fptr = (void* (*)(struct pdb*, char*))pdb_dlsym(_pdb_obj, "pdb_query");
	_pdb_funcs._pdb_set_fptr = (struct pdb_node_t* (*)(struct pdb*, char*, char*, void*))pdb_dlsym(_pdb_obj, "pdb_set");
	_pdb_funcs._pdb_set_node_fptr = (struct pdb_node_t* (*)(struct pdb*, char*, char*, void*, int))pdb_dlsym(_pdb_obj, "pdb_set_node");
	_pdb_funcs._pdb_del_fptr = (int (*)(struct pdb*, char*))pdb_dlsym(_pdb_obj, "pdb_del");
	_pdb_funcs._pdb_trace_fptr = (char* (*)(struct pdb_node_t*))pdb_dlsym(_pdb_obj, "pdb_trace");
	_pdb_funcs._pdb_set_write_interval_fptr = (void (*)(struct pdb*, int))pdb_dlsym(_pdb_obj, "pdb_set_write_interval");
	_pdb_funcs._pdb_need_write_fptr = (int (*)(struct pdb*))pdb_dlsym(_pdb_obj, "pdb_need_write");
	_pdb_funcs._pdb_write_fptr = (int (*)(struct pdb*, char*))pdb_dlsym(_pdb_obj, "pdb_write");
	_pdb_funcs._pdb_set_free_method_fptr = (void (*)(struct pdb*, struct pdb_node_t*, void*))pdb_dlsym(_pdb_obj, "pdb_set_free_method");
	_pdb_funcs._pdb_count_children_fptr = (int (*)(struct pdb*, char*))pdb_dlsym(_pdb_obj, "pdb_count_children");

	_pdb_funcs._pdb_free_node_fptr = (int (*)(struct pdb_node_t*))pdb_dlsym(_pdb_obj, "pdb_free_node");
	_pdb_funcs._pdb_create_tree_fptr = (struct pdb_node_t* (*)(struct pdb*, char*, char*))pdb_dlsym(_pdb_obj, "pdb_create_tree");
	_pdb_funcs._pdb_create_list_fptr = (struct pdb_node_t* (*)(struct pdb*, char*, char*))pdb_dlsym(_pdb_obj, "pdb_create_list");
	_pdb_funcs._pdb_create_hash_fptr = (struct pdb_node_t* (*)(struct pdb*, char*, char*, int))pdb_dlsym(_pdb_obj, "pdb_create_hash");
	_pdb_funcs._pdb_create_abstract_fptr = (struct pdb_node_t* (*)(struct pdb*, char*, char*, void*, void*))pdb_dlsym(_pdb_obj, "pdb_create_abstract");
	_pdb_funcs._pdb_set_int_fptr = (struct pdb_node_t* (*)(struct pdb*, char*, char*, int))pdb_dlsym(_pdb_obj, "pdb_set_int");


	#ifdef _PDB_VERBOSE
	printf("INFO: pdb.c: Cached pdb_load(): %s\n",
		(_pdb_funcs._pdb_load_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_unload(): %s\n",
		(_pdb_funcs._pdb_unload_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_enable(): %s\n",
		(_pdb_funcs._pdb_enable_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_disable(): %s\n",
		(_pdb_funcs._pdb_disable_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_is_set(): %s\n",
		(_pdb_funcs._pdb_is_set_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_create_link(): %s\n",
		(_pdb_funcs._pdb_create_link_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_query_node(): %s\n",
		(_pdb_funcs._pdb_query_node_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_query(): %s\n",
		(_pdb_funcs._pdb_query_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_set(): %s\n",
		(_pdb_funcs._pdb_set_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_set_node(): %s\n",
		(_pdb_funcs._pdb_set_node_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_del(): %s\n",
		(_pdb_funcs._pdb_del_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_trace(): %s\n",
		(_pdb_funcs._pdb_trace_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_set_write_interval(): %s\n",
		(_pdb_funcs._pdb_set_write_interval_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_need_write(): %s\n",
		(_pdb_funcs._pdb_need_write_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_write(): %s\n",
		(_pdb_funcs._pdb_write_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_set_free_method(): %s\n",
		(_pdb_funcs._pdb_set_free_method_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_count_children(): %s\n",
		(_pdb_funcs._pdb_count_children_fptr ? "success" : "failed"));

	printf("INFO: pdb.c: Cached pdb_free_node(): %s\n",
		(_pdb_funcs._pdb_free_node_fptr ? "success" : "failed"));

	printf("INFO: pdb.c: Cached pdb_create_tree(): %s\n",
		(_pdb_funcs._pdb_create_tree_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_create_list(): %s\n",
		(_pdb_funcs._pdb_create_list_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_create_hash(): %s\n",
		(_pdb_funcs._pdb_create_hash_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_create_abstract(): %s\n",
		(_pdb_funcs._pdb_create_abstract_fptr ? "success" : "failed"));
	printf("INFO: pdb.c: Cached pdb_set_int(): %s\n",
		(_pdb_funcs._pdb_set_int_fptr ? "success" : "failed"));
	#endif
}


/*
 *	Function wrappers from library.
 */
struct pdb* pdb_load(char* file) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_load_fptr(file);
}

int pdb_unload(struct pdb* dbptr) {
	if (!_pdb_obj)	return 0;
	return _pdb_funcs._pdb_unload_fptr(dbptr);
}

void pdb_enable(struct pdb* dbptr, int settings) {
	if (!_pdb_obj)	return;
	_pdb_funcs._pdb_enable_fptr(dbptr, settings);
}

void pdb_disable(struct pdb* dbptr, int settings) {
	if (!_pdb_obj)	return;
	_pdb_funcs._pdb_disable_fptr(dbptr, settings);
}

int pdb_is_set(struct pdb* dbptr, int setting) {
	if (!_pdb_obj)	return 0;
	return _pdb_funcs._pdb_is_set_fptr(dbptr, setting);
}

int pdb_create_link(struct pdb* dbptr, char* path, char* key,
	struct pdb_node_t* tnptr) {
	if (!_pdb_obj)	return 0;
	return _pdb_funcs._pdb_create_link_fptr(dbptr, path, key, tnptr);
}

struct pdb_node_t* pdb_query_node(struct pdb* dbptr, char* path) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_query_node_fptr(dbptr, path);
}

void* pdb_query(struct pdb* dbptr, char* path) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_query_fptr(dbptr, path);
}

struct pdb_node_t* pdb_set(struct pdb* dbptr, char* path, char* key,
	void* data) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_set_fptr(dbptr, path, key, data);
}

struct pdb_node_t* pdb_set_node(struct pdb* dbptr, char* path, char* key,
	void* data, int type) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_set_node_fptr(dbptr, path, key, data, type);
}

int pdb_del(struct pdb* dbptr, char* path) {
	if (!_pdb_obj)	return 0;
	return _pdb_funcs._pdb_del_fptr(dbptr, path);
}

char* pdb_trace(struct pdb_node_t* nptr) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_trace_fptr(nptr);
}

void pdb_set_write_interval(struct pdb* dbptr, int seconds) {
	if (!_pdb_obj)	return;
	_pdb_funcs._pdb_set_write_interval_fptr(dbptr, seconds);
}

int pdb_need_write(struct pdb* dbptr) {
	if (!_pdb_obj)	return 0;
	return _pdb_funcs._pdb_need_write_fptr(dbptr);
}

int pdb_write(struct pdb* dbptr, char* file) {
	if (!_pdb_obj)	return 0;
	return _pdb_funcs._pdb_write_fptr(dbptr, file);
}

void pdb_set_free_method(struct pdb* dbptr, struct pdb_node_t* nptr,
	void* free_cb) {
	if (!_pdb_obj)	return;
	_pdb_funcs._pdb_set_free_method_fptr(dbptr, nptr, free_cb);
}

int pdb_count_children(struct pdb* dbptr, char* path) {
	if (!_pdb_obj)	return 0;
	return _pdb_funcs._pdb_count_children_fptr(dbptr, path);
}

int pdb_free_node(struct pdb_node_t* nptr) {
	if (!_pdb_obj)	return 0;
	return _pdb_funcs._pdb_free_node_fptr(nptr);
}

/*
 *	Extra wrappers for pdb_set_node -- provided by the lib.
 */
struct pdb_node_t* pdb_create_tree(struct pdb* dbptr, char* path, char* key) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_create_tree_fptr(dbptr, path, key);
}

struct pdb_node_t* pdb_create_list(struct pdb* dbptr, char* path, char* key) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_create_list_fptr(dbptr, path, key);
}

struct pdb_node_t* pdb_create_hash(struct pdb* dbptr, char* path, char* key,
	int size) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_create_hash_fptr(dbptr, path, key, size);
}

struct pdb_node_t* pdb_create_abstract(struct pdb* dbptr, char* path,
	char* key, void* data, void* free_cb) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_create_abstract_fptr(dbptr, path, key, data,
		free_cb);
}

struct pdb_node_t* pdb_set_int(struct pdb* dbptr, char* path, char* key,
	int data) {
	if (!_pdb_obj)	return NULL;
	return _pdb_funcs._pdb_set_int_fptr(dbptr, path, key, data);
}
