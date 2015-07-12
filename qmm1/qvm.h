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

/* $Id: qvm.h,v 1.3 2006/01/29 22:45:37 cybermind Exp $ */

#ifndef __QVM_H__
#define __QVM_H__

#include "qmm.h"

//magic number is stored in file as 44 14 72 12
#define	QVM_MAGIC	0x12721444	//little endian
#define QVM_MAGIC_BIG	0x44147212	//big endian

typedef enum qvmopcode_s {
	OP_UNDEF,
	OP_NOP,
	OP_BREAK,
	OP_ENTER,
	OP_LEAVE,
	OP_CALL,
	OP_PUSH,
	OP_POP,
	OP_CONST,
	OP_LOCAL,
	OP_JUMP,
	OP_EQ,
	OP_NE,
	OP_LTI,
	OP_LEI,
	OP_GTI,
	OP_GEI,
	OP_LTU,
	OP_LEU,
	OP_GTU,
	OP_GEU,
	OP_EQF,
	OP_NEF,
	OP_LTF,
	OP_LEF,
	OP_GTF,
	OP_GEF,
	OP_LOAD1,
	OP_LOAD2,
	OP_LOAD4,
	OP_STORE1,
	OP_STORE2,
	OP_STORE4,
	OP_ARG,
	OP_BLOCK_COPY,
	OP_SEX8,
	OP_SEX16,
	OP_NEGI,
	OP_ADD,
	OP_SUB,
	OP_DIVI,
	OP_DIVU,
	OP_MODI,
	OP_MODU,
	OP_MULI,
	OP_MULU,
	OP_BAND,
	OP_BOR,
	OP_BXOR,
	OP_BCOM,
	OP_LSH,
	OP_RSHI,
	OP_RSHU,
	OP_NEGF,
	OP_ADDF,
	OP_SUBF,
	OP_DIVF,
	OP_MULF,
	OP_CVIF,
	OP_CVFI
} qvmopcode_t;

//a single opcode in memory
typedef struct qvmop_s {
	qvmopcode_t op;
	int param;
} qvmop_t;

typedef struct qvmheader_s {
	int magic;
	int numops;
	int codeoffset;
	int codelen;
	int dataoffset;
	int datalen;
	int litlen;			// ( datalen - litlen ) should be byteswapped on load
	int bsslen;			// zero filled memory appended to datalen
} qvmheader_t;

#endif //__QVM_H__
