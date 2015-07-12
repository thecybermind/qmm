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

/* $Id: ent.cpp,v 1.3 2006/02/13 20:34:24 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_local.h>
#include <qmmapi.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ent.h"
#include "CLinkList.h"
#include "util.h"

//this stores all the ents loaded from the map
CLinkList<ent_t> g_mapents;
int g_nummapents = 0;

//this stores all the ents that should be passed to the mod
//this consists of all g_mapents that aren't stripped as well as all added ents
CLinkList<ent_t> g_modents;
int g_nummodents = 0;

//this stores all info for entities that should be replaced
//the first node is grabbed and removed from the list when a "with" entity is found
CLinkList<ent_t> g_replaceents;



//gets all the entity tokens from the engine at startup
//and stores them in a list
void get_entity_tokens() {
	//this is a list of key/value pairs for a single entity
	ent_t* ent = NULL;
	ent_t* ent2 = NULL;	//for g_mapents

	//this is a single key/value pair
	keyval_t* keyval = NULL;
	keyval_t* keyval2 = NULL;	//for g_mapents

	char str[MAX_TOKEN_CHARS];

	bool insideent = 0;	//0 = between ents, 1 = inside an ent
	bool isval = 0;		//0 = expecting key, 1 = expecting val

	//go through every token
	while (1) {
		//get token, check for EOF
		if (!g_syscall(G_GET_ENTITY_TOKEN, str, sizeof(str)))
			break;

		//got an opening brace while already inside an entity, error
		if (str[0] == '{' && insideent)
			break;

		//got a closing brace when not inside an entity, error
		if (str[0] == '}' && !insideent)
			break;

		//if this is a closing brace when expecting a val, error
		if (str[0] == '}' && isval)
			break;

		//if this is a valid closing brace, continue
		if (str[0] == '}') {
			insideent = 0;
			g_modents.add(ent);
			g_mapents.add(ent2);
			ent = NULL;
			ent2 = NULL;
			++g_nummodents;
			++g_nummapents;
			continue;
		}

		//if this is a valid opening brace, make a new node
		if (str[0] == '{') {
			insideent = 1;
			isval = 0;
			ent = new ent_t;
			ent2 = new ent_t;
			continue;
		}

		//this is a key
		if (!isval) {
			keyval = new keyval_t;
			keyval2 = new keyval_t;
			keyval->key = strdup(str);
			keyval2->key = strdup(str);
		//this is a val
		} else {
			keyval->val = strdup(str);
			keyval2->val = strdup(str);
			if (!strcmp(keyval->key, "classname")) {
				ent->classname = strdup(str);
				ent2->classname = strdup(str);
			}
			ent->keyvals.add(keyval);
			ent2->keyvals.add(keyval2);
			keyval = NULL;
			keyval2 = NULL;
		}
		isval = !isval;
	} //while(1)

	if (ent) delete ent;
	if (ent2) delete ent2;
	if (keyval) delete keyval;
	if (keyval2) delete keyval2;
}

//returns 1 if "ent" contains at least the same keyvals as matchent
//otherwise returns 0
bool match_ent(ent_t* ent, ent_t* matchent) {
	bool flag = 0;
	CLinkNode<keyval_t>* matchkeyval = matchent->keyvals.first();
	
	while (matchkeyval) {
		const char* entval = ent->get_val(matchkeyval->data()->key);
		if (!entval) return 0;
		flag = 1;
		if (strcmp(entval, matchkeyval->data()->val)) return 0;

		matchkeyval = matchkeyval->next();
	}

	return flag;
}

//removes all matching entities from g_modents
void filter_ent(ent_t* filterent) {
	CLinkNode<ent_t>* modnode = g_modents.first();
	while (modnode) {
		if (match_ent(modnode->data(), filterent)) {
			CLinkNode<ent_t>* q = modnode->next();
			g_modents.del(modnode);
			--g_nummodents;
			modnode = q;
			continue;
		}

		modnode = modnode->next();
	}
}

//adds an entity to g_modents (puts worldspawn at the beginning)
void add_ent(ent_t* addent) {
	if (!strcmp(addent->classname, "worldspawn")) g_modents.insert(addent, NULL);
	else g_modents.add(addent);
	++g_nummodents;
}

//finds all entities matching all replaceents and replaces with a withent
void replace_ent(ent_t* withent) {
	CLinkNode<ent_t>* replacenode = g_replaceents.first();
	while (replacenode) {
		CLinkNode<ent_t>* modnode = g_modents.first();
		while (modnode) {
			if (match_ent(modnode->data(), replacenode->data()))
				with_ent(modnode->data(), withent);

			modnode = modnode->next();
		}

		replacenode = replacenode->next();
	}

	g_replaceents.empty();
}

//replaces all applicable keyvals on an ent
void with_ent(ent_t* replaceent, ent_t* withent) {
	CLinkNode<keyval_t>* replacekeyval = replaceent->keyvals.first();
	while (replacekeyval) {
		const char* withval = withent->get_val(replacekeyval->data()->key);
		if (withval) {
			free(replacekeyval->data()->val);
			replacekeyval->data()->val = strdup(withval);
		}

		replacekeyval = replacekeyval->next();
	}
}

//load and parse config file into delent and addent lists
//int pointers are for getting totals
void load_config(const char* file) {
	fileHandle_t f;
	int fsize = g_syscall(G_FS_FOPEN_FILE, file, &f, FS_READ);
	if (fsize == -1)
		return;

	//this is a list of key/value pairs for a single entity
	ent_t* ent = NULL;

	//this is a single key/value pair
	keyval_t* keyval = NULL;

	//each line of text
	const char* str = NULL;

	//count how many ents are loaded
	int numfiltered = 0, numadded = 0, numreplaced = 0;

	//clear the replace list
	g_replaceents.empty();

	//what the current entity mode is
	enum Mode {
		Mode_Filter,
		Mode_Add,
		Mode_Replace,
		Mode_With,
	} mode = Mode_Filter;

	bool insideent = 0;	//0 = between ents, 1 = inside an ent
	
	//reset the internal read_line counter
	read_line(0, -1);

	//go through every token
	while (1) {
		//get token, check for EOF
		if (!(str = read_line(f, fsize)))
			break;

		//skip comments and blank lines
		else if (!*str || (*str == '#') || (*str == ';') || (*str == '/' && *(str+1) == '/'))
			continue;

		//only pay attention to tags while outside of entities
		if (!strcasecmp(str, "filter:")) {
			if (!insideent) mode = Mode_Filter;
			else continue;
		} else if (!strcasecmp(str, "add:")) {
			if (!insideent) mode = Mode_Add;
			else continue;
		} else if (!strcasecmp(str, "replace:")) {
			if (!insideent) mode = Mode_Replace;
			else continue;
		} else if (!strcasecmp(str, "with:")) {
			if (!insideent) mode = Mode_With;
			else continue;
		}

		//got an opening brace while already inside an entity, error
		if (str[0] == '{' && insideent)
			continue;

		//got a closing brace when not inside an entity, error
		else if (str[0] == '}' && !insideent)
			continue;

		//if this is a valid closing brace, continue
		else if (str[0] == '}') {
			insideent = 0;
			
			//don't actually add the entity if it's empty
			if (!ent->keyvals.first()) {
				delete ent;
				ent = NULL;
				continue;
			}

			if (mode == Mode_Filter) {
				filter_ent(ent);
				delete ent;	//don't need this anymore
				++numfiltered;
			} else if (mode == Mode_Add) {
				add_ent(ent);	//ent is part of modents list
				++numadded;
			} else if (mode == Mode_Replace) {
				g_replaceents.add(ent);	//store until a "with" ent comes along
				++numreplaced;
			} else if (mode == Mode_With) {
				replace_ent(ent);
				delete ent;	//don't need this anymore
			}
			ent = NULL;
			continue;
		}

		//if this is a valid opening brace, make a new node
		else if (str[0] == '{') {
			insideent = 1;
			ent = new ent_t;
			continue;
		}

		//its a key/val pair line or something else
		if (insideent) {
			char* eq = strstr(str, "=");
			if (eq) {
				*eq = '\0';
				keyval = new keyval_t;
				keyval->key = strdup(str);
				keyval->val = strdup(eq+1);
				if (!strcmp(keyval->key, "classname"))
					ent->classname = strdup(keyval->val);
				ent->keyvals.add(keyval);
			}
		}

	} //while(1)

	if (ent) delete ent;

	g_syscall(G_FS_FCLOSE_FILE, f);
	g_syscall(G_PRINT, QMM_VARARGS("[STRIPPER] Loaded %d filters, %d adds, and %d replaces from %s\n", numfiltered, numadded, numreplaced, file));	
}



//passes the next entity token to the mod
int get_next_entity_token(char* str, int len) {
	//count how many ents we've passed to the mod
	static int numents = 0;

	//this is a node representing an entity
	static CLinkNode<ent_t>* ent = NULL;

	//this is a node representing a single key/value pair
	static CLinkNode<keyval_t>* keyval = NULL;

	static bool insideent = 0;	//0 = between ents, 1 = inside an ent
	static bool isval = 0;		//0 = expecting key, 1 = expecting val

	//if we have sent all the entities, delete lists and send an EOF
	if (numents == g_nummodents) {
		g_modents.empty();
		g_replaceents.empty(); //just to be safe
		//save g_mapents for the stripper_dump command
		return 0;
	}

	//this is only done once, just starts at the beginning of the ent list
	//if there are no ents, the above condition would have triggered
	if (!ent) ent = g_modents.first();

	//if we are just starting or done an entity, send a {
	if (!insideent) {
		insideent = 1;
		isval = 0;
		keyval = ent->data()->keyvals.first();
		strncpy(str, "{", len);
		return 1;
	}

	//if a keyval exists and we need to send a key, send it
	if (insideent && keyval && !isval) {
		isval = !isval;
		strncpy(str, keyval->data()->key, len);
		return 1;
	}
	
	//if a keyval exists and we need to send a val, send it
	if (insideent && keyval && isval) {
		isval = !isval;
		strncpy(str, keyval->data()->val, len);
		keyval = keyval->next();
		return 1;
	}

	//if we are inside an ent and no keyval exists, send a }
	if (insideent && !keyval) {
		insideent = 0;
		++numents;
		ent = ent->next();
		strncpy(str, "}", len);
		return 1;
	}

	return 0;
}



//outputs mapents list to a file
void dump_ents() {
	char* file = QMM_VARARGS("qmmaddons/stripper/dumps/%s.txt", QMM_GETSTRCVAR("mapname"));

	fileHandle_t f;
	g_syscall(G_FS_FOPEN_FILE, file, &f, FS_WRITE);
	CLinkNode<ent_t>* p = g_mapents.first();
	while (p) {
		g_syscall(G_FS_WRITE, "{\n", 2, f);
		CLinkNode<keyval_t>* q = p->data()->keyvals.first();
		while (q) {
			char* buf = QMM_VARARGS("\t%s=%s\n", q->data()->key, q->data()->val);
			g_syscall(G_FS_WRITE, buf, strlen(buf), f);
			q = q->next();
		}
		g_syscall(G_FS_WRITE, "}\n", 2, f);
		p = p->next();
	}
	g_syscall(G_FS_FCLOSE_FILE, f);
	g_syscall(G_PRINT, QMM_VARARGS("[STRIPPER] Ent dump written to %s\n", file));
}
