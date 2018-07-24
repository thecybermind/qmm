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

/* $Id: CVMMod.cpp,v 1.21 2006/01/31 00:14:54 cybermind Exp $ */

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "osdef.h"
#include "CVMMod.h"
#include "CConfigMgr.h"
#include "qmm.h"
#include "qvm.h"
#include "util.h"

CVMMod::CVMMod() {
	memset(this->file, 0, sizeof(this->file));
	this->codesegment = NULL;
	this->datasegment = NULL;
	this->stacksegment = NULL;

	this->codeseglen = 0;
	this->dataseglen = 0;
	this->stackseglen = 0;

	this->opptr = NULL;
	this->stackptr = NULL;
	this->argbase = 0;

	this->memory = NULL;
	this->memorysize = 0;

	this->swapped = 0;
	this->filesize = 0;

	memset(&this->header, 0, sizeof(this->header));
}

CVMMod::~CVMMod() {
	if (this->memory)
		free(this->memory);
}

// - file is the path relative to the install directory
int CVMMod::LoadMod(const char* file) {
	if (!file || !*file)
		return 0;
	
	//ignore the homepath stuff here, since we are using the engine's file functions
	//these will automatically check the various paths
	strncpy(this->file, file, sizeof(this->file));

	int fqvm;
	byte temp;
	int k;

	//open QVM file (use engine calls so we can easily read into .pk3)
	this->filesize = ENG_SYSCALL(ENG_MSG(QMM_G_FS_FOPEN_FILE), this->file, &fqvm, ENG_MSG(QMM_FS_READ));

	if (this->filesize <= 0) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CVMMod::LoadMod(\"%s\"): Failed to open QVM file for reading\n", this->file));
		return 0;
	}




	//load file as needed, do not load all at once
	
	ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), &this->header, sizeof(this->header), fqvm);
	ENG_SYSCALL(ENG_MSG(QMM_G_FS_FCLOSE_FILE), fqvm);

	//if we are a big-endian machine, need to swap everything around
	if (this->header.magic == QVM_MAGIC_BIG) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] WARNING: CVMMod::LoadMod(): Big-endian magic number detected, will byteswap during load.\n", this->file));
		this->swapped = 1;
		this->header.magic = byteswap(this->header.magic);
		this->header.numops = byteswap(this->header.numops);
		this->header.codeoffset = byteswap(this->header.codeoffset);
		this->header.codelen = byteswap(this->header.codelen);
		this->header.dataoffset = byteswap(this->header.dataoffset);
		this->header.datalen = byteswap(this->header.datalen);
		this->header.litlen = byteswap(this->header.litlen);
		this->header.bsslen = byteswap(this->header.bsslen);
	}

	//check file
	if (	this->header.magic != QVM_MAGIC ||
		this->header.numops <= 0 ||
		this->header.codelen <= 0 ||
		(unsigned int)this->filesize != (sizeof(this->header) + this->header.codelen + this->header.datalen + this->header.litlen) ||
		this->header.codeoffset < sizeof(this->header) ||
		this->header.dataoffset < sizeof(this->header) || 
		this->header.codeoffset > this->filesize ||
		this->header.dataoffset > this->filesize
		) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CVMMod::LoadMod(): QVM file \"%s\" is not the correct format\n", this->file));
		return 0;
	}

	//each opcode is 8 bytes long, calculate total size of opcodes
	this->codeseglen = this->header.numops * sizeof(qvmop_t);
	//just add each data segment up
	this->dataseglen = this->header.datalen + this->header.litlen + this->header.bsslen;
	//stack size is from config file (in MB), defaults to 1
	int cfgstack = g_ConfigMgr->GetInt(vaf("%s/qvmstacksize", g_EngineMgr->GetModDir()), 1);
	if (cfgstack <= 0) cfgstack = 1;
	this->stackseglen = cfgstack * (1<<20);

	//get total memory size
	this->memorysize = this->codeseglen + this->dataseglen + this->stackseglen;

	//load memory code block (freed in destructor)
	this->memory = (byte*)malloc(this->memorysize);
	//malloc failed
	if (!this->memory) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CVMMod::LoadMod(): Unable to allocate memory chunk (size=%d) to store virtual machine for \"%s\"\n", this->memorysize, this->file));
		return 0;
	}

	//clear the memory
	memset(this->memory, 0, this->memorysize);

	// set pointers
	this->codesegment = (qvmop_t*)this->memory;
	this->datasegment = (byte*)(this->memory + this->codeseglen);
	this->stacksegment = (byte*)(this->datasegment + this->dataseglen);

	//setup registers
	//op is the code pointer, simple enough
	this->opptr = NULL;
	//stack is backwards (starts at end of buffer and goes -4 when pushing)
	this->stackptr = (int*)(this->stacksegment + this->stackseglen);
	//the 4 controls the local var and argument heap (also backwards)
	this->argbase = this->dataseglen + this->stackseglen / 2;
	//NOTE: OP_LOCAL grabs the offset into the datasegment for an argbase value
	//for use with OP_LOADx. The actual argstack resides in the first half of
	//the stack segment

	//move file pointer to the code offset
	ENG_SYSCALL(ENG_MSG(QMM_G_FS_FOPEN_FILE), this->file, &fqvm, ENG_MSG(QMM_FS_READ));
	for (k = 0; k < this->header.codeoffset; ++k) {
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), &temp, 1, fqvm);
	}

	//start loading ops from file to memory
	int i;

	//loop through each op
	for (i = 0; i < this->header.numops; ++i) {
		//get the opcode
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), &temp, 1, fqvm);
	
		//write opcode (to qvmop_t)
		this->codesegment[i].op = (qvmopcode_t)temp;

		switch(temp) {
			//these ops all have full 4-byte 'param's, which may need to be byteswapped
			case OP_ENTER:
			case OP_LEAVE:
			case OP_CONST:
			case OP_LOCAL:
			case OP_EQ:
			case OP_NE:
			case OP_LTI:
			case OP_LEI:
			case OP_GTI:
			case OP_GEI:
			case OP_LTU:
			case OP_LEU:
			case OP_GTU:
			case OP_GEU:
			case OP_EQF:
			case OP_NEF:
			case OP_LTF:
			case OP_LEF:
			case OP_GTF:
			case OP_GEF:
			case OP_BLOCK_COPY:
				ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), &this->codesegment[i].param, 4, fqvm);
				if (this->swapped)
					this->codesegment[i].param = byteswap(this->codesegment[i].param);
				break;
			//this op has a 1-byte 'param'
			case OP_ARG:
				ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), &temp, 1, fqvm);
				this->codesegment[i].param = (int)temp;
				break;
			//remaining ops require no 'param'
			default:
				this->codesegment[i].param = 0;
				break;
		}
	}
	ENG_SYSCALL(ENG_MSG(QMM_G_FS_FCLOSE_FILE), fqvm);

	//move file pointer to the data offset
	ENG_SYSCALL(ENG_MSG(QMM_G_FS_FOPEN_FILE), this->file, &fqvm, ENG_MSG(QMM_FS_READ));
	for (k = 0; k < this->header.dataoffset; ++k) {
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), &temp, 1, fqvm);
	}

	//start loading data segment from file to memory (requires byteswapping if neccesary)
	int* ddst = (int*)(this->datasegment);

	//loop through each 4-byte data block
	for (i = 0; i < this->header.datalen / (signed int)sizeof(int); ++i) {
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), ddst, 4, fqvm);
		if (this->swapped)
			*ddst = byteswap(*ddst);
		++ddst;
	}

	//copy remaining data into the lit segment as-is
	ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), ddst, this->header.litlen, fqvm);

	ENG_SYSCALL(ENG_MSG(QMM_G_FS_FCLOSE_FILE), fqvm);

	//a winner is us
	return 1;




/*	//old loading method
	//reads file all at once into a single malloc block and then parses

	//allocate memory block the size of the file
	byte* qvmfile = (byte*)malloc(this->filesize);

	//malloc failed
	if (!qvmfile) {
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_FCLOSE_FILE), fqvm);
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CVMMod::LoadMod(): Failed to allocate memory chunk (size=%d) to store QVM file \"%s\"\n", this->filesize, this->file));
		return 0;
	}

	//read VM file into memory block
	ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), qvmfile, this->filesize, fqvm);
	ENG_SYSCALL(ENG_MSG(QMM_G_FS_FCLOSE_FILE), fqvm);

	//file is read entirely into memory at this point
	qvmheader_t* header;
	header = (qvmheader_t*)qvmfile;

	//if we are a big-endian machine, need to swap everything around
	if (header->magic == QVM_MAGIC_BIG) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] WARNING: CVMMod::LoadMod(): Big-endian magic number detected, will byteswap during load.\n", this->file));
		this->swapped = 1;
		header->magic = byteswap(header->magic);
		header->numops = byteswap(header->numops);
		header->codeoffset = byteswap(header->codeoffset);
		header->codelen = byteswap(header->codelen);
		header->dataoffset = byteswap(header->dataoffset);
		header->datalen = byteswap(header->datalen);
		header->litlen = byteswap(header->litlen);
		header->bsslen = byteswap(header->bsslen);
	}

	//check file
	if (header->magic != QVM_MAGIC || header->numops <= 0 || header->codelen <= 0) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CVMMod::LoadMod(): QVM file \"%s\" is not the correct format\n", this->file));
		free(qvmfile);
		return 0;
	}

	//save header info
	this->header = *header;

	//each opcode is 5 bytes long, calculate total size of opcodes
	this->codeseglen = header->numops * sizeof(qvmop_t);
	//just add each data segment up
	this->dataseglen = header->datalen + header->litlen + header->bsslen;
	//STACK SPACE IS SPLIT BETWEEN STACK AND ARG HEAP
	//stack size is from config file (in MB), defaults to 1
	this->stackseglen = 1;
	if (g_pdb) {
		char* qvmstacksize = (char*)pdb_query(g_pdb, vaf("%s/qvmstacksize", g_EngineMgr->GetModDir()));
		if (qvmstacksize && *qvmstacksize)
			this->stackseglen = atoi(qvmstacksize);
		else
			ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] WARNING: CVMMod::LoadMod(): Unable to detect \"qvmstacksize\" config option, defaulting to 1MB stack\n", this->file));
	}

	this->stackseglen *= 1<<20;

	//get total memory size
	this->memorysize = this->codeseglen + this->dataseglen + this->stackseglen;

	//load memory code block (freed in destructor)
	this->memory = (byte*)malloc(this->memorysize);
	//malloc failed
	if (!this->memory) {
		ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CVMMod::LoadMod(): Unable to allocate memory chunk (size=%d) to store virtual machine for \"%s\"\n", this->memorysize, this->file));
		free(qvmfile);
		return 0;
	}

	//clear the memory
	memset(this->memory, 0, this->memorysize);

	// set pointers
	this->codesegment = (qvmop_t*)this->memory;
	this->datasegment = (byte*)(this->memory + this->codeseglen);
	this->stacksegment = (byte*)(this->datasegment + this->dataseglen);

	//setup registers
	//op is the code pointer, simple enough
	this->opptr = NULL;
	//stack is backwards (starts at end of buffer and goes -4 when pushing)
	this->stackptr = (int*)(this->stacksegment + this->stackseglen);
	//the argbase controls the local var and argument heap (also backwards)
	this->argbase = this->dataseglen + this->stackseglen / 2;


	//start loading ops from file to memory
	register byte* src = qvmfile + header->codeoffset;
	register byte op;	//just to store opcode temporarily for each loop

	register int i;

	//loop through each op
	for (i = 0; i < header->numops; ++i) {
		//get its opcode and move src to the parameter field
		op = *src++;
		//write opcode (to qvmop_t)
		this->codesegment[i].op = (int)op;

		switch(op) {
			//these ops all have full 4-byte 'param's, which may need to be byteswapped
			case OP_ENTER:
			case OP_LEAVE:
			case OP_CONST:
			case OP_LOCAL:
			case OP_EQ:
			case OP_NE:
			case OP_LTI:
			case OP_LEI:
			case OP_GTI:
			case OP_GEI:
			case OP_LTU:
			case OP_LEU:
			case OP_GTU:
			case OP_GEU:
			case OP_EQF:
			case OP_NEF:
			case OP_LTF:
			case OP_LEF:
			case OP_GTF:
			case OP_GEF:
			case OP_BLOCK_COPY:
				this->codesegment[i].param = this->swapped ? byteswap(*(int*)src) : *(int*)src;
				src += 4;
				break;
			//this op has a 1-byte 'param'
			case OP_ARG:
				this->codesegment[i].param = *src++;
				break;
			//remaining ops require no 'param'
			default:
				this->codesegment[i].param = 0;
				break;
		}
	}

	//start loading data segment from file to memory (requires byteswapping if neccesary)
	register int* lsrc = (int*)(qvmfile + header->dataoffset);
	register int* ldst = (int*)(this->datasegment);

	//loop through each 4-byte data block (even though data may be single bytes)
	for (i = 0; i < header->datalen/(signed int)sizeof(int); ++i) {
		*ldst++ = this->swapped ? byteswap(*lsrc) : *lsrc;
		++lsrc;
	}

	//copy remaining data into the lit segment as-is
	memcpy(ldst, lsrc, header->litlen);

	//free memory space allocated to storing file
	free(qvmfile);

	//a winner is us
	return 1;
*/
}

int CVMMod::vmMain(int cmd, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	if (!this->memory)
		return 0;

	//prepare local stack
	this->argbase -= 15 * sizeof(int);

	int* args = (int*)(this->datasegment + this->argbase);

	//push args into the new arg heap space

	args[0] = 0;	//blank for now
	//store the current code offset
	args[1] = (this->opptr - this->codesegment);
	//arguments
	args[2] = cmd;
	args[3] = arg0;
	args[4] = arg1;
	args[5] = arg2;
	args[6] = arg3;
	args[7] = arg4;
	args[8] = arg5;
	args[9] = arg6;
	args[10] = arg7;
	args[11] = arg8;
	args[12] = arg9;
	args[13] = arg10;
	args[14] = arg11;

	--this->stackptr;

	//vmMain's OP_ENTER will grab this and store it at the bottom of the arg stack
	//when it is added to this->codesegment in the final OP_LEAVE, it will result in
	//opptr being NULL, terminating the execution loop
	this->stackptr[0] = (qvmop_t*)NULL - this->codesegment;
	
	//move opptr to start of opcodes
	this->opptr = this->codesegment;

	//local versions of registers
	register qvmop_t* opptr = this->opptr;
	register int* stack = this->stackptr;

	register qvmopcode_t op;
	register int param;

//if this is linux, we can use gcc's ability to go to addresses to speed up this process
//this is just an array to store goto label addresses for all of the opcode labels
#ifndef WIN32
	static void* labelarray[] = {
		&&goto_OP_UNDEF,
		&&goto_OP_NOP,
		&&goto_OP_BREAK,
		&&goto_OP_ENTER,
		&&goto_OP_LEAVE,
		&&goto_OP_CALL,
		&&goto_OP_PUSH,
		&&goto_OP_POP,
		&&goto_OP_CONST,
		&&goto_OP_LOCAL,
		&&goto_OP_JUMP,
		&&goto_OP_EQ,
		&&goto_OP_NE,
		&&goto_OP_LTI,
		&&goto_OP_LEI,
		&&goto_OP_GTI,
		&&goto_OP_GEI,
		&&goto_OP_LTU,
		&&goto_OP_LEU,
		&&goto_OP_GTU,
		&&goto_OP_GEU,
		&&goto_OP_EQF,
		&&goto_OP_NEF,
		&&goto_OP_LTF,
		&&goto_OP_LEF,
		&&goto_OP_GTF,
		&&goto_OP_GEF,
		&&goto_OP_LOAD1,
		&&goto_OP_LOAD2,
		&&goto_OP_LOAD4,
		&&goto_OP_STORE1,
		&&goto_OP_STORE2,
		&&goto_OP_STORE4,
		&&goto_OP_ARG,
		&&goto_OP_BLOCK_COPY,
		&&goto_OP_SEX8,
		&&goto_OP_SEX16,
		&&goto_OP_NEGI,
		&&goto_OP_ADD,
		&&goto_OP_SUB,
		&&goto_OP_DIVI,
		&&goto_OP_DIVU,
		&&goto_OP_MODI,
		&&goto_OP_MODU,
		&&goto_OP_MULI,
		&&goto_OP_MULU,
		&&goto_OP_BAND,
		&&goto_OP_BOR,
		&&goto_OP_BXOR,
		&&goto_OP_BCOM,
		&&goto_OP_LSH,
		&&goto_OP_RSHI,
		&&goto_OP_RSHU,
		&&goto_OP_NEGF,
		&&goto_OP_ADDF,
		&&goto_OP_SUBF,
		&&goto_OP_DIVF,
		&&goto_OP_MULF,
		&&goto_OP_CVIF,
		&&goto_OP_CVFI,
	};
#endif

	do {
		op = (qvmopcode_t)opptr->op;
		param = opptr->param;

		++opptr;

//if this is windows, use a regular switch block
#ifdef WIN32
		switch(op) {


//if this is linux, we will use goto to speed it up
//case labels are changed into goto labels (goto_LABEL)
//breaks are changed to "goto goto_next_op" (which is located at the bottom of the loop)
//defaults are changed to the label "goto_default" (which is used if the op is invalid)
#else
		if (op < OP_UNDEF || op > OP_CVFI) goto goto_default;

		goto *labelarray[op];
		
 #define case(lbl) goto_##lbl
 #define break     goto goto_next_op
 #define default   goto_default
#endif

		//miscellaneous

			//no op - don't raise error
			case (OP_NOP):
				break;

			//undefined
			case (OP_UNDEF):
			
			//break to debugger?
			case (OP_BREAK):
			
			//anything else
			default:
				ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] ERROR: CVMMod::vmMain(%s): Unhandled opcode %s (%d)\n", ENG_MSGNAME(cmd), opcodename[op], op));
				log_write(vaf("[QMM] ERROR: CVMMod::vmMain(%s): Unhandled opcode %s (%d)\n", ENG_MSGNAME(cmd), opcodename[op], op));
				break;

		//stack

			//pushes a blank value onto the end of the stack
			case (OP_PUSH):
				--stack;
				*stack = 0;
				break;
			//pops the last value off the end of the stack
			case (OP_POP):
				++stack;
				break;
			//pushes a specified value onto the end of the stack
			case (OP_CONST):
				--stack;
				*stack = param;
				break;
			//pushes a specified local variable address onto the stack
			case (OP_LOCAL):
				--stack;
				*stack = this->argbase + param;
				break;
			//set a function-call arg (offset = param) to the value in stack[0]
			case (OP_ARG):
				*(int*)(this->datasegment + this->argbase + param) = *stack;
				++stack;
				break;

		//functions

		#define JUMP(x) opptr = this->codesegment + (x)

			case (OP_CALL):
				param = *stack;

				//param (really stack[0]) is the function address in number of ops
				//negative means an engine trap
				if (param < 0) {
					//save local registers for recursive execution
					this->stackptr = stack;
					this->opptr = opptr;

					int ret = g_EngineMgr->VMSysCall()(this->datasegment, -param - 1, (int*)(this->datasegment + this->argbase) + 2);

					//restore local registers
					stack = this->stackptr;
					opptr = this->opptr;

					*stack = ret;
					break;
				}

				//replace func id (in the stack) with code address to resume at
				//changed to the actual pointer rather than code segment offset
				*stack = opptr - this->codesegment;
				//jump to VM function at address
				JUMP(param);
				break;
			//enter a function, prepare local var heap (length=param)
			//store the return address (front of stack) in arg heap
			case (OP_ENTER):
				this->argbase -= param;
				*(int*)(this->datasegment + this->argbase) = 0;
				*((int*)(this->datasegment + this->argbase) + 1) = *stack++;
				break;
			//leave a function, move opcode pointer to previous function
			case (OP_LEAVE):
				//retrieve the return code address from bottom of the arg heap
				opptr = this->codesegment + *((int*)(this->datasegment + this->argbase) + 1);

				//offset arg heap by same as in OP_ENTER
				this->argbase += param;

				break;

		//branching

		//signed integer comparison
		#define SIF(o) if (stack[1] o *stack) JUMP(param); stack += 2
		//unsigned integer comparison
		#define UIF(o) if (*(unsigned int*)&stack[1] o *(unsigned int*)stack) JUMP(param); stack += 2
		//floating point comparison
		#define FIF(o) if (*(float*)&stack[1] o *(float*)stack) JUMP(param); stack += 2

			//jump to address in stack[0]
			case (OP_JUMP):
				JUMP(*stack++);
				break;
			//if stack[1] == stack[0], goto address in param
			case (OP_EQ):
				SIF(==);
				break;
			//if stack[1] != stack[0], goto address in param
			case (OP_NE):
				SIF(!=);
				break;
			//if stack[1] < stack[0], goto address in param
			case (OP_LTI):
				SIF(<);
				break;
			//if stack[1] <= stack[0], goto address in param
			case (OP_LEI):
				SIF(<=);
				break;
			//if stack[1] > stack[0], goto address in param
			case (OP_GTI):
				SIF(>);
				break;
			//if stack[1] >= stack[0], goto address in param
			case (OP_GEI):
				SIF(>=);
				break;
			//if stack[1] < stack[0], goto address in param (unsigned)
			case (OP_LTU):
				UIF(<);
				break;
			//if stack[1] <= stack[0], goto address in param (unsigned)
			case (OP_LEU):
				UIF(<=);
				break;
			//if stack[1] > stack[0], goto address in param (unsigned)
			case (OP_GTU):
				UIF(>);
				break;
			//if stack[1] >= stack[0], goto address in param (unsigned)
			case (OP_GEU):
				UIF(>=);
				break;
			//if stack[1] == stack[0], goto address in param (float)
			case (OP_EQF):
				FIF(==);
				break;
			//if stack[1] != stack[0], goto address in param (float)
			case (OP_NEF):
				FIF(!=);
				break;
			//if stack[1] < stack[0], goto address in param (float)
			case (OP_LTF):
				FIF(<);
				break;
			//if stack[1] <= stack[0], goto address in param (float)
			case (OP_LEF):
				FIF(<=);
				break;
			//if stack[1] > stack[0], goto address in param (float)
			case (OP_GTF):
				FIF(>);
				break;
			//if stack[1] >= stack[0], goto address in param (float)
			case (OP_GEF):
				FIF(>=);
				break;

		//memory/pointer management

			//store 1-byte value from stack[0] into address stored in stack[1]
			case (OP_STORE1):
				*(this->datasegment + stack[1]) = (byte)(*stack & 0xFF);
				stack += 2;
				break;
			//2-byte
			case (OP_STORE2):
				*(unsigned short*)(this->datasegment + stack[1]) = (unsigned short)(*stack & 0xFFFF);
				stack += 2;
				break;
			//4-byte
			case (OP_STORE4):
				*(int*)(this->datasegment + stack[1]) = *stack;
				stack += 2;
				break;
			
			//get 1-byte value at address stored in stack[0],
			//and store back in stack[0]
			//1-byte
			case (OP_LOAD1):
				*stack = *(byte*)(this->datasegment + *stack);
				break;
			//2-byte
			case (OP_LOAD2):
				*stack = *(unsigned short*)(this->datasegment + *stack);
				break;
			//4-byte
			case (OP_LOAD4):
				*stack = *(int*)(this->datasegment + *stack);
				break;
			//copy mem at address pointed to by stack[0] to address pointed to by stack[1]
			//for 'param' number of bytes
			case (OP_BLOCK_COPY): {
				byte* from = this->datasegment + *stack++;
				byte* to = this->datasegment + *stack++;

				/*if( param & 3 ) {
					//ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] WARNING: CVMMod::vmMain(%s): OP_BLOCK_COPY not DWORD aligned, truncating copy length\n", ENG_MSGNAME(cmd)));
				}

				//convert param from number of bytes -> ints
				param >>= 2;
				*/

				if (from == to) {
					//ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] WARNING: CVMMod::vmMain(%s): OP_BLOCK_COPY pointers are equal, ignoring\n", ENG_MSGNAME(cmd)));
					break;
				}

				memcpy(to, from, param);

				/*do {
					*to++ = *from++;
				} while(--param);*/

				break;
			}

		//arithmetic/operators

		//signed integer (stack[0] done to stack[1], stored in stack[1])
		#define SOP(o) stack[1] o *stack; stack++
		//unsigned integer (stack[0] done to stack[1], stored in stack[1])
		#define UOP(o) *(unsigned int*)&stack[1] o *(unsigned int*)stack; stack++
		//floating point (stack[0] done to stack[1], stored in stack[1])
		#define FOP(o) *(float*)&stack[1] o *(float*)stack; stack++
		//signed integer (done to self)
		#define SSOP(o) *stack =o *stack
		//floating point (done to self)
		#define SFOP(o) *(float*)stack =o *(float*)stack

			//negation
			case (OP_NEGI):
				SSOP(-);
				break;
			//addition
			case (OP_ADD):
				SOP(+=);
				break;
			//subtraction
			case (OP_SUB):
				SOP(-=);
				break;
			//multiplication
			case (OP_MULI):
				SOP(*=);
				break;
			//unsigned multiplication
			case (OP_MULU):
				UOP(*=);
				break;
			//division
			case (OP_DIVI):
				SOP(/=);
				break;
			//unsigned division
			case (OP_DIVU):
				UOP(/=);
				break;
			//modulation
			case (OP_MODI):
				SOP(%=);
				break;
			//unsigned modulation
			case (OP_MODU):
				UOP(%=);
				break;
			//bitwise AND
			case (OP_BAND):
				SOP(&=);
				break;
			//bitwise OR
			case (OP_BOR):
				SOP(|=);
				break;
			//bitwise XOR
			case (OP_BXOR):
				SOP(^=);
				break;
			//bitwise one's compliment
			case (OP_BCOM):
				SSOP(~);
				break;
			//unsigned bitwise LEFTSHIFT
			case (OP_LSH):
				UOP(<<=);
				break;
			//bitwise RIGHTSHIFT
			case (OP_RSHI):
				SOP(>>=);
				break;
			//unsigned bitwise RIGHTSHIFT
			case (OP_RSHU):
				UOP(>>=);
				break;
			//float negation
			case (OP_NEGF):
				SFOP(-);
				break;
			//float addition
			case (OP_ADDF):
				FOP(+=);
				break;
			//float subtraction
			case (OP_SUBF):
				FOP(-=);
				break;
			//float multiplication
			case (OP_MULF):
				FOP(*=);
				break;
			//float division
			case (OP_DIVF):
				FOP(/=);
				break;

		//sign extensions

			//8-bit
			case (OP_SEX8):
				if (*stack & 0x80)
					*stack |= 0xFFFFFF00;
				break;
			//16-bit
			case (OP_SEX16):
				if (*stack & 0x8000)
					*stack |= 0xFFFF0000;
				break;

		//format conversion

			//convert stack[0] int->float
			case (OP_CVIF):
				*(float*)stack = (float)*stack;
				break;
			//convert stack[0] float->int
			case (OP_CVFI):
				*stack = (int)*(float*)stack;
				break;

//in windows, just close the switch block
#ifdef WIN32
		} //op switch
//in linux, make a label that is called after every opcode is executed
#else
		goto_next_op:
		;	//satisfy gcc error "label must be followed by statement"

 //undefine all the replacements in linux
 #ifdef case
  #undef case
 #endif
 #ifdef break
  #undef break
 #endif
 #ifdef default
  #undef default
 #endif

#endif

	} while (opptr);

	//restore previous code pointer as well as the arg heap
	this->opptr = this->codesegment + args[1];
	this->argbase += 15 * sizeof(int);
	this->stackptr = stack;

	//return value is stored on the top of the stack (pushed just before OP_LEAVE)
	return *this->stackptr++;
}

int CVMMod::IsVM() {
	return 1;
}

const char* CVMMod::File() {
	return this->file[0] ? this->file : NULL;
}

int CVMMod::GetBase() {
	return (int)this->datasegment;
}

void CVMMod::Status() {
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] QVM byteswapped: %s\n", this->swapped ? "yes" : "no"));
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] QVM file size: %d\n", this->filesize));
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] QVM op count: %d\n", this->header.numops));
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] QVM memory offset: %p\n", this->memory));
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] QVM memory size: %d\n", this->memorysize));
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] QVM codeseg size: %d\n", this->codeseglen));
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] QVM dataseg size: %d\n", this->dataseglen));
	ENG_SYSCALL(ENG_MSG(QMM_G_PRINT), vaf("[QMM] QVM stack size: %d\n", this->stackseglen));
}
