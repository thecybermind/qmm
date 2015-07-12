/*

QMMVM - VM Wrapper Tool
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

Special Thanks:
    VM code from Q3Fusion (http://www.sourceforge.net/projects/q3fusion/)
    VM info from Phaethon (http://www.icculus.org/~phaethon/q3mc/q3vm_specs.html)
    Code3Arena (http://www.planetquake.com/code3arena/)

*/

/* $Id: vm.cpp,v 1.10 2004/08/30 00:45:36 cybermind Exp $ */

#include "version.h"
#include <q_shared.h>	//always
#include <g_public.h>	//always
#include <malloc.h>	//for alloc functions
#include "vm.h"
#include "main.h"
#include "util.h"

/* VM_Run, VM_Exec, VM_Create, VM_Destroy, and VM_Restart
 * originally from Q3Fusion (http://www.sourceforge.net/projects/q3fusion/)
 */

//executes the VM (only entry point = vmMain, start of codeSegment)
//all the opStack, opPointer, opBase, etc initialization has been done in VM_Exec
//modified to include real (non-VM) pointer support
//---
//vm = pointer to VM
static void VM_Run( vm_t *vm ) {
	vmOps_t		op;
	int		param;

	// local registers
	int		*opStack;
	int		*opPointer;

	// constants /not changed during execution/
	byte		*dataSegment;
	unsigned	dataSegmentMask;

	opStack = vm->opStack;
	opPointer = vm->opPointer;

	dataSegment = vm->dataSegment;
	dataSegmentMask = vm->dataSegmentMask;

	//keep going until opPointer is NULL
	//opPointer is set in OP_LEAVE, stored in the function stack
	//VM_Exec sets this to NULL before calling so that as soon as vmMain is done, execution stops
	do {
		//fetch opcode
		op = (vmOps_t)opPointer[0];
		//get the param
		param = opPointer[1];
		//move to the next opcode
		opPointer += 2;

		if (qmmvm_debug >= DEBUG_OPCODE) {
			if (!opparms(op))
				fprintf(g_df, "%s: param=%i\n", opcodename(op), param);
			else if (opparms(op) == 1)
				fprintf(g_df, "%s: param=%i\n", opcodename(op), opStack[0]);
			else if (opparms(op) == 2)
				fprintf(g_df, "%s: params=%i %i\n", opcodename(op), opStack[0], opStack[1]);
			else if (opparms(op) == 3)
				fprintf(g_df, "%s: params=%i %i %i\n", opcodename(op), param, opStack[0], opStack[1]);
		}

		//here's the magic
		switch( op ) {
//
// aux
//
		//undefined
		case OP_UNDEF:
		//no op?
		case OP_NOP:
		//break to debugger?
		case OP_BREAK:
		//anything else
		default:
			g_syscall(G_PRINT, vaf("[QMMVM] ERROR: VM_Run: Unhandled opcode %s (%i)", opcodename(op), op));
			break;

//
// subroutines
//
//jumps to a specific opcode
#define GOTO(x) {opPointer = vm->codeSegment + (x) * 2;}

		//enter a function, assign function parameters (length=param) from stack
		case OP_ENTER:
			vm->opBase -= param;
			*((int*)(dataSegment + vm->opBase) + 1) = *opStack++;
			break;

		//leave a function, move opcode pointer to previous function
		case OP_LEAVE:
			opPointer = vm->codeSegment + *((int*)(dataSegment + vm->opBase) + 1);
			vm->opBase += param;
			break;

		//call a function at address stored in opStack[0]
		case OP_CALL:
			param = opStack[0];
			
			//CyberMind - param(opStack[0]) is the function address, negative means a engine trap
			//added fix for external function pointers
			//if param is greater than the memorySize, it's a real function pointer, so call it
			if( param < 0 || param >= vm->memorySize) {
				int ret = 0, realfunc = 0;
				int* args = NULL;
				//int			*fp;

				// system trap or real system function

				// save local registers for recursive execution
				//vm->opBase = opBase;
				vm->opStack = opStack;
				vm->opPointer = opPointer;

				//clear hook var
				vm->hook_realfunc = 0;

				args = (int *)(dataSegment + vm->opBase) + 2;

				//if a trap function, call our local syscall, which parses each message
				if (param < 0) {
					ret = VM_SysCalls( dataSegment, -param - 1, args );
				//otherwise it's a real function call, grab args and call function
				} else {
					//cdecl calling convention says caller (us) cleans stack
					//so we can stuff the args without worry of stack corruption
					ret = ((pfn_t)param)(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10],args[11]);
				}

				// restore local registers
				//opBase = vm->opBase;
				opStack = vm->opStack;
				opPointer = vm->opPointer;

				//if we are running a VM function due to hook
				//and we have a real VM func to call, call it
				if (vm->hook_realfunc && param >= vm->memorySize) {
					//replace func address with return address
					opStack[0] = (int)(opPointer - vm->codeSegment);
					GOTO (vm->hook_realfunc)
				//otherwise we use the syscall/hook func return value
				} else {
					opStack[0] = ret;
				}
				break;
			}
			//replace func address with return address
			opStack[0] = (int)(opPointer - vm->codeSegment); // push pc /return address/
			//jump to VM function at address
			GOTO( param )
			break;

//
// stack
//
		//pushes a 0 onto the end of the stack
		case OP_PUSH : opStack--;	opStack[0] = 0;				break;
		//pops the last value off the end of the stack
		case OP_POP  : opStack++;						break;
		//pushes a specified value onto the end of the stack
		case OP_CONST: opStack--;	opStack[0] = param;			break;
		//pushes a specified
		case OP_LOCAL: opStack--;	opStack[0] = param + vm->opBase;	break;


//
// branching
//
#define SOP(operation) {if (opStack[1] operation opStack[0]) GOTO(param); opStack += 2;}
#define UOP(operation) {if (*(unsigned int*)&opStack[1] operation *(unsigned int*)&opStack[0]) GOTO(param); opStack += 2;}
#define FOP(operation) {if (*(float*)&opStack[1] operation *(float*)&opStack[0]) GOTO(param); opStack += 2;}

		//jump to address in opStack[0], and pop
		case OP_JUMP: GOTO(*opStack++)	break;
		//if opStack[1] == opStack[0], goto address in param
		case OP_EQ  : SOP(==)	break;
		//if opStack[1] != opStack[0], goto address in param
		case OP_NE  : SOP(!=)	break;
		//if opStack[1] < opStack[0], goto address in param
		case OP_LTI : SOP(< )	break;
		//if opStack[1] <= opStack[0], goto address in param
		case OP_LEI : SOP(<=)	break;
		//if opStack[1] > opStack[0], goto address in param
		case OP_GTI : SOP(> )	break;
		//if opStack[1] >= opStack[0], goto address in param
		case OP_GEI : SOP(>=)	break;
		//if opStack[1] < opStack[0], goto address in param (unsigned)
		case OP_LTU : UOP(< )	break;
		//if opStack[1] <= opStack[0], goto address in param (unsigned)
		case OP_LEU : UOP(<=)	break;
		//if opStack[1] > opStack[0], goto address in param (unsigned)
		case OP_GTU : UOP(> )	break;
		//if opStack[1] >= opStack[0], goto address in param (unsigned)
		case OP_GEU : UOP(>=)	break;
		//if opStack[1] == opStack[0], goto address in param (float)
		case OP_EQF : FOP(==)	break;
		//if opStack[1] != opStack[0], goto address in param (float)
		case OP_NEF : FOP(!=)	break;
		//if opStack[1] < opStack[0], goto address in param (float)
		case OP_LTF : FOP(< )	break;
		//if opStack[1] <= opStack[0], goto address in param (float)
		case OP_LEF : FOP(<=)	break;
		//if opStack[1] > opStack[0], goto address in param (float)
		case OP_GTF : FOP(> )	break;
		//if opStack[1] >= opStack[0], goto address in param (float)
		case OP_GEF : FOP(>=)	break;

//
// memory I/O: masks protect main memory
//

		//get value at address stored in opStack[0], store in opStack[0]
		//(do neccesary conversions)
		//this is essentially the 'dereferencing' opcode set
		//1-byte
		case OP_LOAD1:
			if (opStack[0] >= vm->memorySize)
				opStack[0] = *(byte*)(opStack[0]);
			else
				opStack[0] = dataSegment[opStack[0] & dataSegmentMask];

			break;
		
		//2-byte
		case OP_LOAD2:
			if (opStack[0] >= vm->memorySize)
				opStack[0] = *(unsigned short*)(opStack[0]);
			else
				opStack[0] = *(unsigned short*)&dataSegment[opStack[0] & dataSegmentMask];

			break;

		//4-byte
		case OP_LOAD4:
			if (opStack[0] >= vm->memorySize)
				opStack[0] = *(int*)(opStack[0]);
			else
				opStack[0] = *(int*)&dataSegment[opStack[0] & dataSegmentMask];

			break;

		//store a value from opStack[0] into address stored in opStack[1]
		//1-byte
		case OP_STORE1:
			if (opStack[1] >= vm->memorySize)
				*(byte*)(opStack[1]) = (byte)(opStack[0] & 0xFF  );
			else
				dataSegment[opStack[1] & dataSegmentMask] = (byte)(opStack[0] & 0xFF  );

			opStack += 2;
			break;
		//2-byte
		case OP_STORE2:
			if (opStack[1] >= vm->memorySize)
				*(unsigned short*)(opStack[1]) = (unsigned short)(opStack[0] & 0xFFFF);
			else
				*(unsigned short*)&dataSegment[opStack[1] & dataSegmentMask] =	(unsigned short)(opStack[0] & 0xFFFF);

			opStack += 2;
			break;
		//4-byte
		case OP_STORE4:
			if (opStack[1] >= vm->memorySize)
				*(int*)(opStack[1]) = opStack[0];
			else
				*(int*)&dataSegment[opStack[1] & dataSegmentMask] = opStack[0];

			opStack += 2;
			break;


		//set a function-call arg (offset = param) to the value in opStack[0]
		case OP_ARG   : *(int*)&dataSegment[(param + vm->opBase) & dataSegmentMask] = opStack[0]; opStack++; break;

		//copy mem at address pointed to by opStack[0] to address pointed to by opStack[1]
		//for 'param' number of bytes
		case OP_BLOCK_COPY:
			{
				int* from = (int*)&dataSegment[opStack[0] & dataSegmentMask];
				int* to	= (int*)&dataSegment[opStack[1] & dataSegmentMask];

				if( param & 3 ) {
					g_syscall(G_ERROR, "[QMMVM] VM_Run: OP_BLOCK_COPY not dword aligned" );
				}

				// FIXME: assume pointers don't overlap?
				param >>= 2;
				do {
					*to++ = *from++;
				} while( --param );

				opStack += 2;
			}
			break;

//
// arithmetic and logic
//
#undef SOP
#undef UOP
#undef FOP
//signed arithmetic
#define SOP(operation)	{opStack[1] operation opStack[0]; opStack++;}
//unsigned arithmetic
#define UOP(operation)	{*(unsigned int*)&opStack[1] operation *(unsigned int*)&opStack[0]; opStack++;}
//floating point arithmetic
#define FOP(operation)	{*(float*)&opStack[1] operation *(float*)&opStack[0]; opStack++;}
//signed arithmetic (on self)
#define SSOP(operation) {opStack[0] =operation opStack[0];}
//floating point arithmetic (on self)
#define SFOP(operation) {*(float*)&opStack[0] =operation *(float*)&opStack[0];}

		//sign extensions
		case OP_SEX8 : if(opStack[0] & 0x80)	opStack[0] |= 0xFFFFFF00; break;
		case OP_SEX16: if(opStack[0] & 0x8000)	opStack[0] |= 0xFFFF0000; break;
		//make negative
		case OP_NEGI : SSOP( - ) break;
		//add opStack[0] to opStack[1], store in opStack[1]
		case OP_ADD  : SOP( += ) break;
		//subtract opStack[0] from opStack[1], store in opStack[1]
		case OP_SUB  : SOP( -= ) break;
		//divide opStack[0] into opStack[1], store in opStack[1]
		case OP_DIVI : SOP( /= ) break;
		//divide opStack[0] into opStack[1], store in opStack[1] (unsigned)
		case OP_DIVU : UOP( /= ) break;
		//modulus opStack[0] into opStack[1], store in opStack[1]
		case OP_MODI : SOP( %= ) break;
		//modulus opStack[0] into opStack[1], store in opStack[1] (unsigned)
		case OP_MODU : UOP( %= ) break;
		//multiply opStack[0] and opStack[1], store in opStack[1]
		case OP_MULI : SOP( *= ) break;
		//multiply opStack[0] and opStack[1], store in opStack[1] (unsigned)
		case OP_MULU : UOP( *= ) break;
		//bitwise AND opStack[0] and opStack[1], store in opStack[1]
		case OP_BAND : SOP( &= ) break;
		//bitwise OR opStack[0] and opStack[1], store in opStack[1]
		case OP_BOR  : SOP( |= ) break;
		//bitwise XOR opStack[0] and opStack[1], store in opStack[1]
		case OP_BXOR : SOP( ^= ) break;
		//bitwise one's compliment opStack[0], store in opStack[1]
		case OP_BCOM : SSOP( ~ ) break;
		//bitwise LEFTSHIFT opStack[1] by opStack[0] bits, store in opStack[1]
		case OP_LSH  : UOP( <<= ) break;
		//bitwise RIGHTSHIFT opStack[1] by opStack[0] bits, store in opStack[1]
		case OP_RSHI : SOP( >>= ) break;
		//bitwise RIGHTSHIFT opStack[1] by opStack[0] bits, store in opStack[1] (unsigned)
		case OP_RSHU : UOP( >>= ) break;
		//make negative (float)
		case OP_NEGF : SFOP( - ) break;
		//add opStack[0] to opStack[1], store in opStack[1] (float)
		case OP_ADDF : FOP( += ) break;
		//subtract opStack[0] from opStack[1], store in opStack[1] (float)
		case OP_SUBF : FOP( -= ) break;
		//divide opStack[0] into opStack[1], store in opStack[1] (float)
		case OP_DIVF : FOP( /= ) break;
		//multiply opStack[0] and opStack[1], store in opStack[1] (float)
		case OP_MULF : FOP( *= ) break;

//
// format conversion
//
		//convert opStack[0] int->float
		case OP_CVIF: *(float *)&opStack[0] = (float)opStack[0]; break;
		//convert opStack[0] float->int
		case OP_CVFI: opStack[0] = (int)(*(float *)&opStack[0]); break;
		}
	} while( opPointer );

//	vm->opBase = opBase;
	vm->opStack = opStack;
//	vm->opPointer = opPointer;
}

//public function to begin the process of executing a VM
//----
//stuff args into the VM stack
//begin VM with VM_Run
//---
//vm = pointer to to VM
//command = GAME instruction to run
//arg# = args to command
int VM_Exec(vm_t *vm, int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
	int* args;

	// prepare local stack
	vm->opBase -= 15 * sizeof( int );
	args = (int *)(vm->dataSegment + vm->opBase);

	// push all params
	args[ 0] = 0;
	args[ 1] = (int)(vm->opPointer - vm->codeSegment); // save pc
	args[ 2] = command;
	args[ 3] = arg0;
	args[ 4] = arg1;
	args[ 5] = arg2;
	args[ 6] = arg3;
	args[ 7] = arg4;
	args[ 8] = arg5;
	args[ 9] = arg6;
	args[10] = arg7;
	args[11] = arg8;
	args[12] = arg9;
	args[13] = arg10;
	args[14] = arg11;
	
	vm->opPointer = NULL; //VM_Run stops execution when opPointer is NULL

	//(ready) move back in stack to save pc
	vm->opStack--;
	vm->opStack[0] = (vm->opPointer - vm->codeSegment);
	//(set) move opPointer to start of opcodes
	vm->opPointer = vm->codeSegment;

	//GO!
	VM_Run( vm );
	
	// restore previous state
	vm->opPointer = vm->codeSegment + args[1];
	vm->opBase += 15 * sizeof( int );

	// pick return value from stack
	return *vm->opStack++;
}

//load the .qvm into the vm_t
//---
//this function opens the .qvm in a file stream, stores in dynamic mem
//reads header info, and loads vm into memory
//----
//vm = pointer to vm_t to load into
//path = filename to load
//oldmem = location to use for VM memory (default NULL)
qboolean VM_Create(vm_t* vm, const char* path, byte* oldmem) {
	if( !vm || !path || !path[0] ) return qfalse;

	vmHeader_t* header;
	byte* vmBase;
	int n;
	byte* src;
	int* lsrc;
	int* dst;
	vmOps_t	op;
	int codeSegmentSize;
	vm->swapped = qfalse;

	fileHandle_t fvm;
	//open VM file (use engine calls so we can easily read into .pk3)
	vm->fileSize = g_syscall(G_FS_FOPEN_FILE, path, &fvm, FS_READ);
	//allocate memory block the size of the file
	vmBase = (byte*)malloc(vm->fileSize);
	
	//malloc failed
	if (!vmBase) {
		memset(vm, 0, sizeof(vm_t));
		return qfalse;
	}

	//read VM file into memory block
	g_syscall(G_FS_READ, vmBase, vm->fileSize, fvm);
	g_syscall(G_FS_FCLOSE_FILE, fvm);

	header = (vmHeader_t*)vmBase;

	//if we are a big-endian machine, need to swap everything around
	if (header->vmMagic == VM_MAGIC_BIG) {
		g_syscall(G_PRINT, "[QMMVM] WARNING: VM_Create: Big-endian magic number detected, will byteswap during load.\n");
		vm->swapped = qtrue;
		header->vmMagic = byteswap(header->vmMagic);
		header->instructionCount = byteswap(header->instructionCount);
		header->codeOffset = byteswap(header->codeOffset);
		header->codeLength = byteswap(header->codeLength);
		header->dataOffset = byteswap(header->dataOffset);
		header->dataLength = byteswap(header->dataLength);
		header->litLength = byteswap(header->litLength);
		header->bssLength = byteswap(header->bssLength);
	}
	vm->header = *header;	//save header info in vm_t

	// check file
	if (header->vmMagic != VM_MAGIC || header->instructionCount <= 0 || header->codeLength <= 0) {
		free(vmBase);
		memset(vm, 0, sizeof(vm_t));
		return qfalse;
	}

	// setup segments
	vm->codeSegmentLen = header->instructionCount;
	vm->dataSegmentLen = header->dataLength + header->litLength + header->bssLength;

	// calculate memory protection mask (including the stack?)
	for (vm->dataSegmentMask = 1; ; vm->dataSegmentMask <<= 1) {
		if(vm->dataSegmentMask > vm->dataSegmentLen + qmmvm_stacksize) {
			vm->dataSegmentMask--;
			break;
		}
	}
	
	//each opcode is 2 ints long, calculate total size of opcodes
	codeSegmentSize = vm->codeSegmentLen * sizeof(int) * 2;

	vm->memorySize = codeSegmentSize + vm->dataSegmentLen + qmmvm_stacksize;
	//load memory code block (freed in VM_Destroy)
	//if we are reloading, we should keep the same memory location, otherwise, make more
	vm->memory = (oldmem ? oldmem : (byte*)malloc(vm->memorySize));
	//malloc failed
	if (!vm->memory) {
		g_syscall(G_PRINT, vaf("[QMMVM] Unable to allocate VM memory chunk (size=%i)\n", vm->memorySize));
		free(vmBase);
		memset(vm, 0, sizeof(vm_t));
		return qfalse;
	}
	//clear the memory
	memset(vm->memory, 0, vm->memorySize);

	// set pointers
	vm->codeSegment = (int*)vm->memory;
	vm->dataSegment = (byte*)(vm->memory + codeSegmentSize);
	vm->stackSegment = (byte*)(vm->dataSegment + vm->dataSegmentLen);

	//setup registers
	vm->opPointer = NULL;
	vm->opStack = (int*)(vm->stackSegment + qmmvm_stacksize);
	vm->opBase = vm->dataSegmentLen + qmmvm_stacksize / 2;

	//load instructions from file to memory
	src = vmBase + header->codeOffset;
	dst = vm->codeSegment;

	//loop through each instruction
	for (n = 0; n < header->instructionCount; n++) {
		//get its opcode and move src to the parameter field
		op = (vmOps_t)*src++;
		//write opcode (as int) and move dst to next int
		*dst++ = (int)op;

		switch( op ) {
		//these ops all have full 4-byte 'param's, which may need to be byteswapped
		//remaining args are drawn from stack
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
			*dst = *(int*)src;
			if (vm->swapped == qtrue)
				*dst = byteswap(*dst);
			dst++;
			src += 4;
			break;
		//this op has only a single byte 'param' (draws 1 arg from stack)
		case OP_ARG:
			*dst++ = (int)*src++;
			break;
		//remaining ops require no 'param' (draw all, if any, args from stack)
		default:
			*dst++ = 0;
			break;
		}
	}


	// load data segment from file to memory
	lsrc = (int*)(vmBase + header->dataOffset);
	dst = (int*)(vm->dataSegment);
	
	//loop through each 4-byte data block (even though data may be single bytes)
	for (n = 0; n < header->dataLength/sizeof(int); n++) {
		*dst = *lsrc++;
		//swap if need-be
		if (vm->swapped == qtrue)
			*dst = byteswap(*dst);
		dst++;
	}

	//copy remaining data into the lit segment
	memcpy(dst, lsrc, header->litLength);

	//free file from memory
	free(vmBase);

	//a winner is us
	return qtrue;
}

//frees used memory and clears vm_t
void VM_Destroy(vm_t *vm) {
	if (vm->memory)
		free(vm->memory);
	memset(vm, 0, sizeof(vm_t));
}

//modified to save old memory pointer and pass it to VM_Create
//so we can keep the same memory location (might be desirable at
//some point in the future, but for now, we don't even reload)

//keep in mind...SHIT WILL BREAK IF YOU SWAP QVMS AND THEN RESTART
//do NOT blame me for assuming the file will stay the same
qboolean VM_Restart(vm_t *vm, qboolean savemem) {
	if(!vm) return qfalse;

	char name[MAX_QPATH];
	byte* oldmem = NULL;

	//save filename (we need this to reload the same file, obviously)
	strncpy(name, vm->name, sizeof(name));

	//save memory pointer or free it
	if (savemem == qtrue)
		oldmem = vm->memory;
	else
		free(vm->memory);

	//kill it!
	memset(vm, 0, sizeof(vm_t));

	//reload
	if (!VM_Create(vm, name, oldmem)) {
		VM_Destroy(vm);
		return qfalse;
	}
	
	return qtrue;
}
