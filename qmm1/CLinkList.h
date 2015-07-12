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

/* $Id: CLinkList.h,v 1.2 2005/03/03 16:06:02 cybermind Exp $ */

#ifndef __CLINKLIST_H__
#define __CLINKLIST_H__

#include <malloc.h>

template <typename T> class CLinkList;

template <typename T>
class CLinkNode {
	friend class CLinkList<T>;

	public:
		CLinkNode();
		~CLinkNode();

		inline CLinkNode<T>* prev();
		inline CLinkNode<T>* next();
		inline T* data();

	private:
		CLinkNode<T>* _prev;
		CLinkNode<T>* _next;
		T* _data;
		int _del;
};

//--------------

template <typename T>
class CLinkList {
	public:
		CLinkList();
		~CLinkList();

		CLinkNode<T>* add(T*, int = 1);
		CLinkNode<T>* insert(T*, CLinkNode<T>*, int = 1);
		void del(CLinkNode<T>*);

		inline CLinkNode<T>* first();

	private:
		CLinkNode<T>* _first;
};

//==============

template<typename T>
CLinkNode<T>::CLinkNode() {
	this->_data = NULL;
	this->_prev = NULL;
	this->_next = NULL;
	this->_del = 1;
}

template <typename T>
CLinkNode<T>::~CLinkNode() {
	if (this->_del && this->_data) {
		if (this->_del == 1)
			delete this->_data;
		else if (this->_del == 2)
			free(this->_data);
	}
}

template <typename T>
CLinkNode<T>* CLinkNode<T>::prev() {
	return this->_prev;
}

template <typename T>
CLinkNode<T>* CLinkNode<T>::next() {
	return this->_next;
}

template <typename T>
T* CLinkNode<T>::data() {
	return this->_data;
}

//--------------

template <typename T>
CLinkList<T>::CLinkList() {
	this->_first = NULL;
}

template <typename T>
CLinkList<T>::~CLinkList() {
	CLinkNode<T>* p = this->_first;
	CLinkNode<T>* q = NULL;
	while (p) {
		q = p;
		p = p->_next;
		delete q;
	}

	this->_first = NULL;
}

template <typename T>
CLinkNode<T>* CLinkList<T>::add(T* data, int del) {
	CLinkNode<T>* p = this->_first;
	CLinkNode<T>* q = NULL;
	
	while (p) {
		q = p;
		p = p->_next;
	}
	
	p = new CLinkNode<T>;
	
	if (q)
		q->_next = p;
	else
		this->_first = p;

	p->_prev = q;
	p->_next = NULL;
	p->_data = data;
	p->_del = del;

	return p;
}

template <typename T>
CLinkNode<T>* CLinkList<T>::insert(T* data, CLinkNode<T>* after, int del) {
	CLinkNode<T>* p = this->_first;
	CLinkNode<T>* q = NULL;

	while (q != after) {
		q = p;
		p = p->_next;
	}
	
	CLinkNode<T>* r = new CLinkNode<T>;

	if (q)
		q->_next = r;
	else
		this->_first = r;

	if (p)
		p->_prev = r;

	r->_prev = q;
	r->_next = p;
	r->_data = data;
	r->_del = del;

	return r;
}

template <typename T>
void CLinkList<T>::del(CLinkNode<T>* node) {
	CLinkNode<T>* p = node->_prev;
	CLinkNode<T>* n = node->_next;

	if (p)
		p->_next = n;
	else
		this->_first = n;

	if (n) n->_prev = p;

	delete node;
}

template <typename T>
CLinkNode<T>* CLinkList<T>::first() {
	return this->_first;
}

#endif //__CLINKLIST_H__
