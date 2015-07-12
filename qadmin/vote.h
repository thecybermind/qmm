/*

QADMIN_QMM - QMM Administration Plugin
Copyright QMM Team 2004
http://www.quake3mm.net/

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

/* $Id: vote.h,v 1.1.1.1 2004/12/14 05:12:39 cybermind Exp $ */

#ifndef __VOTE_H__
#define __VOTE_H__

#define MAX_CHOICES 9

typedef void (*pfnVoteFunc)(int winner, int winvotes, int totalvotes, void* param);

typedef struct voteinfo_s {
	bool inuse;
	int finishtime;
	pfnVoteFunc votefunc;
	int choices;
	void* param;
	int clientnum;
	int votes[MAX_CLIENTS];
} voteinfo_t;
extern voteinfo_t g_vote;

void vote_start(int, pfnVoteFunc, int, int, void*);
void vote_add(int, int);
void vote_finish();

#endif //__VOTE_H__
