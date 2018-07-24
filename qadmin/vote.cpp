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

/* $Id: vote.cpp,v 1.2 2005/10/13 02:09:25 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>
#include <g_local.h>
#include <qmmapi.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "vote.h"
#include "util.h"

voteinfo_t g_vote;

//initiate a vote
void vote_start(int clientnum, pfnVoteFunc callback, int seconds, int choices, void* param) {
	if (g_vote.inuse) {
		ClientPrint(clientnum, "[QADMIN] A vote is already running\n");
		return;
	}
	g_vote.clientnum = clientnum;
	g_vote.votefunc = callback;
	g_vote.finishtime = g_levelTime + (seconds * 1000);
	g_vote.choices = choices;
	g_vote.param = param;
	g_vote.inuse = 1;
	memset(g_vote.votes, 0, sizeof(g_vote.votes));
}

//someone has voted (vote should be 1-9)
void vote_add(int clientnum, int vote) {
	if (!g_vote.inuse) {
		ClientPrint(clientnum, "[QADMIN] There is no vote currently running\n");
		return;
	}

	if (g_vote.votes[clientnum]) {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] You have already voted for %d\n", g_vote.votes[clientnum]));
		return;
	}

	if (vote <= 0 || vote > g_vote.choices) {
		ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Invalid vote option, choose from 1-%d\n", g_vote.choices));
		return;
	}

	g_vote.votes[clientnum] = vote;
	ClientPrint(clientnum, QMM_VARARGS("[QADMIN] Vote counted for %d\n", vote));
}

//vote has ended
void vote_finish() {
	int winner = 0, totalvotes = 0, i;
	int votecount[MAX_CHOICES];
	memset(votecount, 0, sizeof(votecount));

	//tally up the votes
	//g_vote.votes[] stores 1-9, so subtract 1 to get proper votecount index
	for (i = 0; i < MAX_CLIENTS; ++i) {
		if (!g_vote.votes[i])
			continue;
		
		++totalvotes;
		++votecount[g_vote.votes[i] - 1];
	}

	//figure out which choice won
	for (i = 0; i < MAX_CHOICES; ++i) {
		if (votecount[i] > votecount[winner])
			winner = i;
	}

	//if the winning votecount is 0 (no one voted), then give winner as 0
	g_vote.votefunc(votecount[winner] ? winner + 1 : 0, votecount[winner], totalvotes, g_vote.param);
	g_vote.inuse = 0;
}
