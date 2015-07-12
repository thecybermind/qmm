/*
QMM - Q3 MultiMod
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
    Kevin Masterson a.k.a. CyberMind <kevinm@planetquake.com>

*/

/* $Id: CConfigMgr.cpp,v 1.4 2005/10/13 01:02:59 cybermind Exp $ */

#include <stddef.h>		//for NULL
#include "CConfigMgr.h"
#include "pdb.h"

CConfigMgr::CConfigMgr() {
	this->db = NULL;
	this->isloaded = 0;
}

CConfigMgr::~CConfigMgr() {
	if (this->db)
		pdb_unload(this->db);
	if (this->isloaded)
		unload_pdb_lib();
}

void CConfigMgr::LoadLib(const char* pdblib) {
	if (!this->isloaded)
		this->isloaded = load_pdb_lib((char*)pdblib);
}

void CConfigMgr::LoadConf(const char* file) {
	if (this->isloaded && !this->db)
		this->db = pdb_load((char*)file);

	//set case-insensitive search mode on the config file
	pdb_enable(this->db, PDB_CASE_INSENSITIVE);
}

int CConfigMgr::IsLibLoaded() {
	return this->isloaded;
}

int CConfigMgr::IsConfLoaded() {
	return this->db ? 1 : 0;
}

void CConfigMgr::UnloadConf() {
	if (this->db)
		pdb_unload(this->db);
	this->db = NULL;
}

void CConfigMgr::UnloadLib() {
	if (this->db)
		pdb_unload(this->db);
	this->db = NULL;

	if (this->isloaded)
		unload_pdb_lib();

	this->isloaded = 0;
}

int CConfigMgr::GetInt(const char* path, int def) {
	if (!this->isloaded || !this->db)
		return def;

	int* x = (int*)pdb_query(this->db, (char*)path);
	return x ? *x : def;
}

char* CConfigMgr::GetStr(const char* path) {
	if (!this->isloaded || !this->db)
		return NULL;

	return (char*)pdb_query(this->db, (char*)path);
}

void* CConfigMgr::GetListRootNode(const char* path) {
	struct pdb_linkList* x = (struct pdb_linkList*)pdb_query(this->db, (char*)path);
	
	if (!x)
		return NULL;

	return x->root;
}

void* CConfigMgr::GetListNextNode(void* listnode) {
	if (!listnode)
		return NULL;

	return ((struct pdb_linkNode*)listnode)->next;
}

char* CConfigMgr::GetListNodeID(void* listnode) {
	if (!listnode)
		return NULL;

	//this has to grab the pdb node inside the listnode and then grab the pdb node's id
	struct pdb_node_t* x = (struct pdb_node_t*) (((struct pdb_linkNode*)listnode)->data);
	if (!x)
		return NULL;
	
	return x->id;
}

CConfigMgr* CConfigMgr::GetInstance() {
	if (!CConfigMgr::instance)
		CConfigMgr::instance = new CConfigMgr;

	return CConfigMgr::instance;
}

CConfigMgr* CConfigMgr::instance = NULL;
