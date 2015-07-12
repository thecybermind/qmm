/*

Stripper - Dynamic Map Entity Modification
Copyright QMM Team 2005
http://www.q3mm.org/

Licensing:
    QMM is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    QMM is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QMM; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Created By:
    Kevin Masterson a.k.a. CyberMind <cybermind@users.sourceforge.net>

*/

/* $Id: ent.h,v 1.4 2006/02/13 20:34:24 cybermind Exp $ */

#ifndef __ENT_H__
#define __ENT_H__

#ifdef WIN32
 #define strcasecmp stricmp
#endif

#include "CLinkList.h"

//this represents a single key/value pair
struct keyval_t {
	char* key;
	char* val;
	
	keyval_t() { this->key = NULL; this->val = NULL; }
	~keyval_t() { if (this->key) free(this->key); if (this->val) free(this->val); }
};

//this represents a single entity
struct ent_t {
	CLinkList<keyval_t> keyvals;
	char* classname;	//stored here for easy checking while removing ents

	//easy way to get value for key
	const char* get_val(const char* key) {
		CLinkNode<keyval_t>* p = this->keyvals.first();
		while (p) {
			if (!strcmp(p->data()->key, key))
				return p->data()->val;
			p = p->next();
		}
		return NULL;
	}

	ent_t() { this->classname = NULL; }
	~ent_t() { if (this->classname) free(this->classname); }
};

//this stores all the ents loaded from the map
extern CLinkList<ent_t> g_mapents;
extern int g_nummapents;

//this stores all the ents that should be passed to the mod
//this consists of all g_mapents that aren't stripped as well as all added ents
extern CLinkList<ent_t> g_modents;
extern int g_nummodents;

//this stores all info for entities that should be replaced
//the first node is grabbed and removed from the list when a "with" entity is found
extern CLinkList<ent_t> g_replaceents;

//gets all the entity tokens from the engine at startup
//and stores them in a list
void get_entity_tokens();

//removes all matching entities from g_modents
void filter_ent(ent_t* filterent);
//adds an entity to g_modents (puts worldspawn at the beginning)
void add_ent(ent_t* addent);
//finds all entities matching all stored replaceents and replaces with a withent
void replace_ent(ent_t* withent);
//replaces all applicable keyvals on an ent
void with_ent(ent_t* replaceent, ent_t* withent);

//load and parse config file into delent and addent lists
void load_config(const char*);

//passes the next entity token to the mod
int get_next_entity_token(char*, int);

//outputs mapents list to a file
void dump_ents();

#endif //__ENT_H__
