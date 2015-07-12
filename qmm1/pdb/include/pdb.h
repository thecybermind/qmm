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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/include/pdb.h,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */
 
#ifndef _PDB_H
#define _PDB_H

/*
 *	Handy macros.
 */
#ifdef WIN32
	#define __FUNCTION__ "FuncNA"
#endif

/*
 *	Comment this if you do not wish to compile
 *	pdb as a library (shared object).
 */
#define PDB_COMPILE_AS_LIB


#define ERROR(msg)	fprintf(stderr, "Error: %s:%s():%i: %s\n", __FILE__, \
					__FUNCTION__, __LINE__, msg);
#define WARNING(msg)	fprintf(stderr, "Warning: %s:%s():%i: %s\n", __FILE__, \
						__FUNCTION__, __LINE__, msg);

#define PDB_COMPILE_INFO	"pdb: Compiled on: " __DATE__ " at " __TIME__


/*
 *	pdb constants.
 */
#define PDB_TOKEN_TERM			';'
#define PDB_PATH_DELIM			'/'
#define PDB_COMMENT_CHAR		'#'
#define PDB_COMMENT_BLOCK_S		"/*"	/*	can only be 2 chars	*/
#define PDB_COMMENT_BLOCK_E		"*/"	/*	can only be 2 chars	*/


/*
 *	Comment to disable comment handling.
 */
#define PDB_ENABLE_COMMENTS


/*
 *	pdb setting value bitmasks.
 */
#define PDB_CASE_INSENSITIVE	1
#define PDB_WRITE_SHUFFLE		2
#define PDB_WRITE_NODE_FIRST	4
#define PDB_THREAD_SAFE			8


/*
 *	Default pdb settings.
 */
#define PDB_DEFAULT_SETTINGS	(PDB_WRITE_SHUFFLE | PDB_WRITE_NODE_FIRST)


/*
 *	Comment this if you do not wish to enable
 *	thread safe features for pdb.
 */
//#define PDB_USING_THREADS
//#define PDB_THREAD_DEBUG

#ifdef PDB_USING_THREADS
	#include <pthread.h>
#endif

/*
 *	Macros to lock or unlock a mutex if
 *	PDB_THREAD_SAFE is enabled for the given
 *	database.
 */
#ifdef PDB_USING_THREADS
	#ifdef PDB_THREAD_DEBUG
		#define __PDB_THREAD_DEBUG_OUTPUT(op) \
			{ \
				printf("%s:%s():%i MUTEX %s\n", __FILE__, \
					__FUNCTION__, __LINE__, op); \
			}
	#else
		#define __PDB_THREAD_DEBUG_OUTPUT(op) {}
	#endif

	#define __PDB_LOCK(mutex) \
		{	pthread_mutex_lock(mutex);		}
	#define __PDB_UNLOCK(mutex) \
		{	pthread_mutex_unlock(mutex);	}
#else
	#define __PDB_LOCK(mutex) {}
	#define __PDB_UNLOCK(mutex) {}
	#define __PDB_THREAD_DEBUG_OUTPUT(op) {}
#endif
#define PDB_MUTEX_LOCK(dbptr) \
	{ \
		if ((dbptr->settings & PDB_THREAD_SAFE) == PDB_THREAD_SAFE) { \
			__PDB_THREAD_DEBUG_OUTPUT("LOCK"); \
			__PDB_LOCK(dbptr->mutex); \
		} \
	}
#define PDB_MUTEX_UNLOCK(dbptr) \
	{ \
		if ((dbptr->settings & PDB_THREAD_SAFE) == PDB_THREAD_SAFE) { \
			__PDB_THREAD_DEBUG_OUTPUT("UNLOCK"); \
			__PDB_UNLOCK(dbptr->mutex); \
		} \
	}


/*
 *	If PDB_WRITE_NODE_FIRST is enabled, write the following
 *	node to disk first (if existant in the root).
 *	This is only for the root node.
 */
#define PDB_WRITE_NODE_FIRST_ID		"finop"


/*
 *	pdb's default disk write interval in seconds.
 */
#define PDB_DEFAULT_WRITE_INTERVAL		120


/*
 *	This callback can be used to override standard free
 *	callbacks for nodes.  It is especially useful for non-standard
 *	types like ABSTRACT_NODE_TYPE, where the actual data type may
 *	differ from node to node.
 */
struct pdb_node_t;
typedef int (*custom_free_cb_t)(void* nptr);


/*
 *	Single database node.
 */
struct pdb_node_t {
	int type;
	char* id;
	void* data;
	struct pdb_node_t* parent;
	custom_free_cb_t custom_free_cb;
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


#ifdef __cplusplus
extern "C"
{
#endif

#if defined(WIN32) && defined(PDB_COMPILE_AS_LIB)
	#define DLLEXP __declspec(dllexport)
#endif
#ifndef DLLEXP
	#define DLLEXP
#endif

DLLEXP struct pdb* pdb_load(char* file);
DLLEXP int pdb_unload(struct pdb* dbptr);

DLLEXP void pdb_enable(struct pdb* dbptr, int settings);
DLLEXP void pdb_disable(struct pdb* dbptr, int settings);
DLLEXP int pdb_is_set(struct pdb* dbptr, int setting);

DLLEXP int pdb_create_link(struct pdb* dbptr, char* path, char* key,
	struct pdb_node_t* tnptr);

DLLEXP struct pdb_node_t* pdb_query_node(struct pdb* dbptr, char* path);
DLLEXP void* pdb_query(struct pdb* dbptr, char* path);

DLLEXP struct pdb_node_t* pdb_set(struct pdb* dbptr, char* path, char* key,
	void* data);
DLLEXP struct pdb_node_t* pdb_set_node(struct pdb* dbptr, char* path, char* key,
	void* data, int type);
DLLEXP int pdb_del(struct pdb* dbptr, char* path);

DLLEXP char* pdb_trace(struct pdb_node_t* nptr);

DLLEXP void pdb_set_write_interval(struct pdb* dbptr, int seconds);
DLLEXP int pdb_need_write(struct pdb* dbptr);
DLLEXP int pdb_write(struct pdb* dbptr, char* file);

DLLEXP void pdb_set_free_method(struct pdb* dbptr, struct pdb_node_t* nptr,
	void* free_cb);

DLLEXP int pdb_count_children(struct pdb* dbptr, char* path);

#ifdef __cplusplus
}
#endif

/*
 *	For ease of inclusion -- include other pdb headers.
 */
#include "pdb_file.h"
#include "pdb_types.h"
#include "pdb_parse.h"

#endif /* _PDB_H */
