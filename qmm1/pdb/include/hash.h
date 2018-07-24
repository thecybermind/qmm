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
 *	$Header: /cvsroot-fuse/qmm/qmm1/pdb/include/hash.h,v 1.2 2005/09/22 01:02:15 cybermind Exp $
 *
 */
 

#ifndef _HASH_H
#define _HASH_H


/*
 *	A hash node.
 */
struct hash_node_t {
	char* id;
	void* data;
};

 
/*
 *	The root hash type.
 */
struct hash {
	int size;
	struct linkList** tbl;
};


/*
 *	Callback which can be passed to free_hash().
 */
typedef void (*hash_func_ptr)(void* dptr);


#ifdef __cplusplus
extern "C"
{
#endif

struct hash* hash_create(int size);
void hash_free(struct hash* hptr, hash_func_ptr free_callback);

int hash_add(struct hash* hptr, char* key, void* dptr);
void* hash_get(struct hash* hptr, char* key);
struct hash_node_t* hash_get_node(struct hash* hptr, char* key);
int hash_del(struct hash* hptr, char* key, hash_func_ptr free_callback);
int hash_remove(struct hash* hptr, char* key);

int hash_count_children(struct hash* hptr);

#ifdef __cplusplus
}
#endif

#endif /* _HASH_H */
