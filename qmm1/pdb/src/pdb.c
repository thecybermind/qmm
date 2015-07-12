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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/src/pdb.c,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */

#define _PDB_C

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
 
#include "str.h"
#include "pdb.h"
#include "pdb_file.h"
#include "pdb_parse.h"
#include "pdb_types.h"
#include "list.h"
#include "binarytree.h"

/*int main(int argc, char** argv) {
	printf(PDB_COMPILE_INFO);
	
	printf("\n--LOAD--\n\n");
	struct pdb* dbptr = pdb_load("/home/para/tmp.conf");
	
	printf("\n--DEBUG--\n\n");

	printf("\n--UNLOAD--\n\n");
	if (dbptr)
		pdb_unload(dbptr);
	return 0;
}*/


/*
 *	Load a file and return a database pointer.
 *	If file is NULL, create an empty database.
 */
DLLEXP struct pdb* pdb_load(char* file) {
	struct pdb* dbptr;
	struct pdb_node_types_t* rntptr;
	int line = 1;
	FILE* fptr = NULL;
	
	dbptr = (struct pdb*)malloc(sizeof(struct pdb));
	if (!dbptr)
		return NULL;
	
	if (file) {
		fptr = pdb_open_file(file);
		if (!fptr)
			return NULL;
	}
	
	/*
	 *	Initialize pdb structure.
	 */
	dbptr->altered = 0;
	dbptr->last_write = time(NULL);
	dbptr->write_interval = PDB_DEFAULT_WRITE_INTERVAL;
	dbptr->settings = PDB_DEFAULT_SETTINGS;
	#ifdef PDB_USING_THREADS
		dbptr->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(dbptr->mutex, NULL);
	#endif
	
	if (file) {
		dbptr->file = (char*)malloc(sizeof(char) * (strlen(file) + 1));
		strcpy(dbptr->file, file);
	} else
		dbptr->file = NULL;
	
	rntptr = pdb_get_type_info(ROOT_NODE_TYPE);
	if (!rntptr) {
		ERROR("Root node type is invalid.");
		free(dbptr->file);
		free(dbptr);
		pdb_close_file(fptr);
		return NULL;
	}		
	dbptr->data = (*rntptr->create_cb)(NULL, NULL, NULL);
	
	/*
	 *	Load database (if file was specified).
	 */
	if (file) {
		pdb_standard_load_node(fptr, dbptr->data, NULL, &line);
		pdb_close_file(fptr);
	}
	
	return dbptr;
}


/*
 *	Unload a database.
 */
DLLEXP int pdb_unload(struct pdb* dbptr) {
	int ret;

	if (!dbptr)
		return 0;
	
	PDB_MUTEX_LOCK(dbptr);

	ret = pdb_free_node(dbptr->data);

	#ifdef PDB_USING_THREADS
		pthread_mutex_destroy(dbptr->mutex);
	#endif

	free(dbptr->file);
	free(dbptr);
	return ret;
}


/*
 *	Enable pdb setting(s).
 *	settings are OR'ed togther.
 */
DLLEXP void pdb_enable(struct pdb* dbptr, int settings) {
	if (!dbptr)
		return;

	PDB_MUTEX_LOCK(dbptr);
	
	dbptr->settings |= settings;
	
	PDB_MUTEX_UNLOCK(dbptr);
}


/*
 *	Disable pdb setting(s).
 *	settings are OR'ed togther.
 */
DLLEXP void pdb_disable(struct pdb* dbptr, int settings) {
	if (!dbptr)
		return;

	PDB_MUTEX_LOCK(dbptr);
	
	dbptr->settings &= ~settings;
	
	PDB_MUTEX_UNLOCK(dbptr);
}


/*
 *	Return 1 if the given setting is set.
 */
DLLEXP int pdb_is_set(struct pdb* dbptr, int setting) {
	if (!dbptr)
		return 0;

	return ((dbptr->settings & setting) == setting);
}


/*
 *	Create a node link from path/key to tnptr.
 *	Node links will be ignored when written to disk.
 */
DLLEXP int pdb_create_link(struct pdb* dbptr, char* path, char* key,
	struct pdb_node_t* tnptr) {
		
	struct pdb_node_t* nptr;
	struct pdb_node_types_t* tiptr;
	int ret;
		
	if (!dbptr)
		return 0;

	nptr = pdb_query_node(dbptr, path);

	if (!nptr)
		return 0;

	tiptr = pdb_get_type_info(LINK_NODE_TYPE);

	PDB_MUTEX_LOCK(dbptr);
	ret = (tiptr->create_cb(key, nptr, (char**)tnptr) ? 1 : 0);
	
	PDB_MUTEX_UNLOCK(dbptr);
	return ret;
}


/*
 *	Return a given node from the database.
 */
DLLEXP struct pdb_node_t* pdb_query_node(struct pdb* dbptr, char* path) {
	char** tok_arr;
	struct pdb_node_t* nptr;
	struct pdb_node_types_t* tiptr;
	int i = 0;
	
	if (!dbptr)
		return 0;

	PDB_MUTEX_LOCK(dbptr);

	if (!strcmp(path, "") || !strcmp(path, "/")) {
		PDB_MUTEX_UNLOCK(dbptr);
		return dbptr->data;
	}
	
	tok_arr = token_parse(path, PDB_PATH_DELIM, NULL);
	
	nptr = dbptr->data;
	tiptr = NULL;
	
	while (tok_arr[i]) {
		tiptr = pdb_get_type_info(nptr->type);
		nptr = tiptr->query_cb(nptr, tok_arr[i]);
		if (!nptr)
			break;
		++i;
	}
	
	token_free(tok_arr);
	
	PDB_MUTEX_UNLOCK(dbptr);

	return nptr;
}


/*
 *	Return a given node's data pointer from the database.
 */
DLLEXP void* pdb_query(struct pdb* dbptr, char* path) {
	struct pdb_node_t* nptr;
	
	if (!dbptr)
		return NULL;

	nptr = pdb_query_node(dbptr, path);
	return (nptr ? nptr->data : NULL);
}


/*
 *	Set a node's data.  If it does not exist, create it.
 *	Return a pointer to the node.
 *
 *	This is for string node types.
 */
DLLEXP struct pdb_node_t* pdb_set(struct pdb* dbptr, char* path, char* key,
	void* data) {
	
	return (pdb_set_node(dbptr, path, key, data, STRING_NODE_TYPE));
}
		
/*
 *	Set a node's data.  If it does not exist, create it.
 *	Return a pointer to the node.
 */
DLLEXP struct pdb_node_t* pdb_set_node(struct pdb* dbptr, char* path, char* key,
	void* data, int type) {

	struct pdb_node_types_t* tiptr;
	struct pdb_node_t* pptr;
	struct pdb_node_types_t* ptiptr;
	struct pdb_node_t* nptr;

	if (!dbptr)
		return NULL;

	PDB_MUTEX_LOCK(dbptr);

	tiptr = pdb_get_type_info(type);
	if (!tiptr) {
		PDB_MUTEX_UNLOCK(dbptr);
		return NULL;
	}
	
	PDB_MUTEX_UNLOCK(dbptr);
	pptr = pdb_query_node(dbptr, path);

	if (!pptr)
		return NULL;

	PDB_MUTEX_LOCK(dbptr);
	
	ptiptr = pdb_get_type_info(pptr->type);
	nptr = ptiptr->query_cb(pptr, key);
		
	if (!nptr) {
		/*
		 *	Node does not exists -- create.
		 */
		nptr = tiptr->create_cb(key, pptr, NULL);
		if (!nptr) {
			fprintf(stderr, "%s:%s():%i: Error: Unable to create \"%s/%s\" in "
				"database.\n", __FILE__, __FUNCTION__, __LINE__, path, key);
			PDB_MUTEX_UNLOCK(dbptr);
			return NULL;
		}
	}
	
	/*
	 *	Schedule a disk write.
	 */	
	dbptr->altered = 1;

	if (!tiptr->set_cb) {
		/*fprintf(stderr, "%s:%s():%i: Warning: Unable to set data at \"%s/%s\" "
			"in database; type %i does not support it.\n",
			__FILE__, __FUNCTION__, __LINE__, path, key, tiptr->bitmask);*/
	} else
		tiptr->set_cb(nptr, data);

	PDB_MUTEX_UNLOCK(dbptr);
	return nptr;
}


/*
 *	Delete a node, recursively deleting all child nodes.
 */
DLLEXP int pdb_del(struct pdb* dbptr, char* path) {
	struct pdb_node_t* nptr;
	int ret;

	if (!dbptr)
		return 0;

	nptr = pdb_query_node(dbptr, path);
	if (!nptr)
		return 0;
	
	PDB_MUTEX_LOCK(dbptr);

	/*
	 *	Free the node.
	 */
	ret = pdb_free_node(nptr);
	
	/*
	 *	Schedule a disk write.
	 */	
	dbptr->altered = 1;

	PDB_MUTEX_UNLOCK(dbptr);
	return ret;
}


/*
 *	Create a backtrace of the given nodes location relative to the root.
 *	The resulting string is allocated and must be freed.
 */
DLLEXP char* pdb_trace(struct pdb_node_t* nptr) {
	struct linkList* lptr;
	struct linkNode* lnptr;
	char* sbuf;
	char* buf;
	int buf_size;
	int used;
	char* str;
	
	lptr = list_create();
		
	while (nptr) {
		list_add_node_front(lptr, nptr->id);
		nptr = nptr->parent;
	}
	
	lnptr = lptr->root;
	sbuf = (char*)malloc(BLOCK_SIZE);
	buf = sbuf;
	buf_size = BLOCK_SIZE;
	used = 1;
	memset(sbuf, 0, BLOCK_SIZE);
	str = NULL;
	
	while (lnptr) {
		str = lnptr->data;
		if (!str) {
			lnptr = lnptr->next;
			continue;
		}
		if (*str) {
			*buf = '/';
			++buf;
		}
		while (*str) {
			*buf = *str;
			++buf;
			++str;
			++used;
			if (used >= buf_size) {
				buf_size *= 2;
				sbuf = (char*)realloc(sbuf, BLOCK_SIZE);
				buf = (sbuf + (sizeof(char) * (used - 1)));
				memset(buf, 0, (sizeof(char) * (buf_size - used)));
			}
		}
		lnptr = lnptr->next;
	}
	
	list_free(lptr, 0, NULL);
	return sbuf;
}


/*
 *	Set the disk write interval.
 */
DLLEXP void pdb_set_write_interval(struct pdb* dbptr, int seconds) {
	if (!dbptr)
		return;

	PDB_MUTEX_LOCK(dbptr);
	dbptr->write_interval = seconds;
	PDB_MUTEX_UNLOCK(dbptr);
}

/*
 *	Return 1 if the pdb is scheduled for a disk write.
 */
DLLEXP int pdb_need_write(struct pdb* dbptr) {
	if (!dbptr)
		return 0;

	return (time(NULL) >= (dbptr->last_write + dbptr->write_interval));
}


/*
 *	Write a loaded database structure to disk.
 */
DLLEXP int pdb_write(struct pdb* dbptr, char* file) {
	FILE* fptr;
	struct pdb_node_t* nptr;
	int ret;
	
	if (!dbptr)
		return 0;

	PDB_MUTEX_LOCK(dbptr);

	if (!dbptr) {
		PDB_MUTEX_UNLOCK(dbptr);
		return 0;
	}
	if (!dbptr->data) {
		PDB_MUTEX_UNLOCK(dbptr);
		return 0;
	}	
		
	fptr = fopen(file, "w");
	if (!fptr) {
		fprintf(stderr, "%s:%s():%i: Error: Unable to open file \"%s\" for "
			"writing.\n", __FILE__, __FUNCTION__, __LINE__, file);
		PDB_MUTEX_UNLOCK(dbptr);
		return 0;
	}
	
	nptr = dbptr->data;
	
	ret = pdb_standard_write_node(dbptr, fptr, nptr, -1);
	fclose(fptr);

	/*
	 *	Reset our writing schedule.
	 */	
	dbptr->altered = 0;
	dbptr->last_write = time(NULL);

	PDB_MUTEX_UNLOCK(dbptr);
	return ret;
}


/*
 *	Set a custom free callback for the given node.
 *	If set, this will be used rather than the types default
 *	free method.
 */
DLLEXP void pdb_set_free_method(struct pdb* dbptr, struct pdb_node_t* nptr,
	void* free_cb) {
		
	if (!dbptr)
		return;

	PDB_MUTEX_LOCK(dbptr);
	nptr->custom_free_cb = (custom_free_cb_t)free_cb;
	PDB_MUTEX_UNLOCK(dbptr);
}


/*
 *	Return the number of children a container has.
 */
DLLEXP int pdb_count_children(struct pdb* dbptr, char* path) {
	struct pdb_node_types_t* tiptr;
	struct pdb_node_t* nptr;
	int ret;
	
	if (!dbptr)
		return 0;

	nptr = pdb_query_node(dbptr, path);

	if (!nptr)
		return 0;
	
	PDB_MUTEX_LOCK(dbptr);

	tiptr = pdb_get_type_info(nptr->type);
	if (!tiptr) {
		/*
		 *	Play it safe -- container not empty.
		 */
		PDB_MUTEX_UNLOCK(dbptr);
		return 0;
	}
	if (!tiptr->count_children_cb) {
		/*
	 	 *	Again, playing it safe.
		 */
		PDB_MUTEX_UNLOCK(dbptr);
		return 0;
	}
	
	ret = tiptr->count_children_cb(nptr);
	
	PDB_MUTEX_UNLOCK(dbptr);
	return ret;	
}
