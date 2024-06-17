/*
QMM - Q3 MultiMod
Copyright 2004-2024
https://github.com/thecybermind/qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "osdef.h"
#include "CLinkList.h"
#include "qmm.h"
#include "qvm.h"	//for the qvmopcode_t enum
#include "util.h"

static CLinkList<byte> s_plugins;

const char* get_modulename() {
	static char name[MAX_PATH] = "";
	if (name[0])
		return name;

	const char* pname = name;

#ifdef WIN32
	MEMORY_BASIC_INFORMATION MBI;

	if (!VirtualQuery((void*)&get_modulename, &MBI, sizeof(MBI)) || MBI.State != MEM_COMMIT || !MBI.AllocationBase)
		return NULL;

	if(!GetModuleFileName((HMODULE)MBI.AllocationBase, name, sizeof(name)) || !name[0])
		return NULL;

	name[sizeof(name)-1] = '\0';

	int slen = strlen(name);
	for (int i = slen; i >= 0; --i) {
		if (name[i] == '\\') {
			pname = &name[i + 1];
			break;
		}
	}
#else
	Dl_info dli;
	memset(&dli, 0, sizeof(dli));

	if (!dladdr((void*)&get_modulename, &dli))
		return NULL;

	int slen = strlen(dli.dli_fname);
	for (int i = slen; i >= 0; --i) {
		if (dli.dli_fname[i] == '/') {
			++i;
			for (int j = 0; i+j <= slen; ++j)
				name[j] = dli.dli_fname[i+j];
			break;
		}
	}

	if (!name[0])
		strncpy(name, dli.dli_fname, sizeof(name));
#endif
	return pname;
}

const char* get_modulepath() {
	static char name[MAX_PATH] = "";
	if (name[0])
		return name;

#ifdef WIN32
	MEMORY_BASIC_INFORMATION MBI;

	if (!VirtualQuery((void*)&get_modulename, &MBI, sizeof(MBI)) || MBI.State != MEM_COMMIT || !MBI.AllocationBase)
		return NULL;

	if(!GetModuleFileName((HMODULE)MBI.AllocationBase, name, sizeof(name)) || !name[0])
		return NULL;

	name[sizeof(name)-1] = '\0';

	int slen = strlen(name);
	for (int i = slen; i >= 0; --i) {
		if (name[i] == '\\') {
			name[i] = '\0';
			break;
		}
	}
#else
	Dl_info dli;
	memset(&dli, 0, sizeof(dli));

	if (!dladdr((void*)&get_modulename, &dli))
		return NULL;

	int slen = strlen(dli.dli_fname);
	for (int i = slen; i >= 0; --i) {
		if (dli.dli_fname[i] == '/') {
			strncpy(name, dli.dli_fname, i);
			name[i] = '\0';
			break;
		}
	}

	if (!name[0])
		strncpy(name, dli.dli_fname, sizeof(name));
#endif
	return name;
}

void* get_modulehandle() {
	static void* handle = NULL;
	if (handle)
		return handle;

#ifdef WIN32
	MEMORY_BASIC_INFORMATION MBI;

	if (!VirtualQuery((void*)&get_modulehandle, &MBI, sizeof(MBI)) || MBI.State != MEM_COMMIT)
		return NULL;

	handle = (void*)MBI.AllocationBase;
#else
	Dl_info dli;
	memset(&dli, 0, sizeof(dli));

	if (!dladdr((void*)&get_modulehandle, &dli))
		return NULL;

	handle = dli.dli_fbase;
#endif
	return handle;
}

/*int ismoduleloaded(void* module) {
#ifdef WIN32
	char name[2];
	if(!GetModuleFileName((HMODULE)module, name, 1) || !name[0])
		return 0;
#else
	Dl_info dli;
	memset(&dli, 0, sizeof(dli));

	if (dlinfo(module, RTLD_DI_CONFIGADDR, &dli) == -1 || !dli.dli_fname[0])
		return 0;
#endif

	return 1;
}*/

int ismoduleloaded(void* handle) {
	CLinkNode<byte>* p = s_plugins.first();
	while (p) {
		if (p->data() == (byte*)handle)
			return 1;
		p = p->next();
	}
	return 0;
}

void setmoduleloaded(void* handle) {
	if (!handle || ismoduleloaded(handle))
		return;

	s_plugins.add((byte*)handle,0);
}

void setmoduleunloaded(void* handle) {
	CLinkNode<byte>* p = s_plugins.first();
	while (p) {
		if (p->data() == (byte*)handle) {
			s_plugins.del(p);
			return;
		}
		p = p->next();
	}
}

int byteswap(int i) {
	byte b1,b2,b3,b4;

	b1 = (byte)(i&255);
	b2 = (byte)((i>>8)&255);
	b3 = (byte)((i>>16)&255);
	b4 = (byte)((i>>24)&255);

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + (int)b4;
}
short byteswap(short s) {
	byte b1,b2;

	b1 = (byte)(s&255);
	b2 = (byte)((s>>8)&255);

	return (short)(((short)b1<<8) + (short)b2);
}

//this uses a cycling array of strings so the return value does not need to be stored locally
char* vaf(char* format, ...) {
	va_list	argptr;
	static char str[8][1024];
	static int index = 0;
	int i = index;

	va_start(argptr, format);
	vsnprintf(str[i], sizeof(str[i]), format, argptr);
	va_end(argptr);

	index = (index + 1) & 7;
	return str[i];
}

//return a string name for the VM opcode
const char* opcodename[] = {
	"OP_UNDEF",
	"OP_NOP",
	"OP_BREAK",
	"OP_ENTER",
	"OP_LEAVE",
	"OP_CALL",
	"OP_PUSH",
	"OP_POP",
	"OP_CONST",
	"OP_LOCAL",
	"OP_JUMP",
	"OP_EQ",
	"OP_NE",
	"OP_LTI",
	"OP_LEI",
	"OP_GTI",
	"OP_GEI",
	"OP_LTU",
	"OP_LEU",
	"OP_GTU",
	"OP_GEU",
	"OP_EQF",
	"OP_NEF",
	"OP_LTF",
	"OP_LEF",
	"OP_GTF",
	"OP_GEF",
	"OP_LOAD1",
	"OP_LOAD2",
	"OP_LOAD4",
	"OP_STORE1",
	"OP_STORE2",
	"OP_STORE4",
	"OP_ARG",
	"OP_BLOCK_COPY",
	"OP_SEX8",
	"OP_SEX16",
	"OP_NEGI",
	"OP_ADD",
	"OP_SUB",
	"OP_DIVI",
	"OP_DIVU",
	"OP_MODI",
	"OP_MODU",
	"OP_MULI",
	"OP_MULU",
	"OP_BAND",
	"OP_BOR",
	"OP_BXOR",
	"OP_BCOM",
	"OP_LSH",
	"OP_RSHI",
	"OP_RSHU",
	"OP_NEGF",
	"OP_ADDF",
	"OP_SUBF",
	"OP_DIVF",
	"OP_MULF",
	"OP_CVIF",
	"OP_CVFI"
};
/*const char* opcodename(int op) {
	register int op2 = op;
	switch(op2) {
		case OP_UNDEF:
			return "OP_UNDEF";
		case OP_NOP:
			return "OP_NOP";
		case OP_BREAK:
			return "OP_BREAK";
		case OP_ENTER:
			return "OP_ENTER";
		case OP_LEAVE:
			return "OP_LEAVE";
		case OP_CALL:
			return "OP_CALL";
		case OP_PUSH:
			return "OP_PUSH";
		case OP_POP:
			return "OP_POP";
		case OP_CONST:
			return "OP_CONST";
		case OP_LOCAL:
			return "OP_LOCAL";
		case OP_JUMP:
			return "OP_JUMP";
		case OP_EQ:
			return "OP_EQ";
		case OP_NE:
			return "OP_NE";
		case OP_LTI:
			return "OP_LTI";
		case OP_LEI:
			return "OP_LEI";
		case OP_GTI:
			return "OP_GTI";
		case OP_GEI:
			return "OP_GEI";
		case OP_LTU:
			return "OP_LTU";
		case OP_LEU:
			return "OP_LEU";
		case OP_GTU:
			return "OP_GTU";
		case OP_GEU:
			return "OP_GEU";
		case OP_EQF:
			return "OP_EQF";
		case OP_NEF:
			return "OP_NEF";
		case OP_LTF:
			return "OP_LTF";
		case OP_LEF:
			return "OP_LEF";
		case OP_GTF:
			return "OP_GTF";
		case OP_GEF:
			return "OP_GEF";
		case OP_LOAD1:
			return "OP_LOAD1";
		case OP_LOAD2:
			return "OP_LOAD2";
		case OP_LOAD4:
			return "OP_LOAD4";
		case OP_STORE1:
			return "OP_STORE1";
		case OP_STORE2:
			return "OP_STORE2";
		case OP_STORE4:
			return "OP_STORE4";
		case OP_ARG:
			return "OP_ARG";
		case OP_BLOCK_COPY:
			return "OP_BLOCK_COPY";
		case OP_SEX8:
			return "OP_SEX8";
		case OP_SEX16:
			return "OP_SEX16";
		case OP_NEGI:
			return "OP_NEGI";
		case OP_ADD:
			return "OP_ADD";
		case OP_SUB:
			return "OP_SUB";
		case OP_DIVI:
			return "OP_DIVI";
		case OP_DIVU:
			return "OP_DIVU";
		case OP_MODI:
			return "OP_MODI";
		case OP_MODU:
			return "OP_MODU";
		case OP_MULI:
			return "OP_MULI";
		case OP_MULU:
			return "OP_MULU";
		case OP_BAND:
			return "OP_BAND";
		case OP_BOR:
			return "OP_BOR";
		case OP_BXOR:
			return "OP_BXOR";
		case OP_BCOM:
			return "OP_BCOM";
		case OP_LSH:
			return "OP_LSH";
		case OP_RSHI:
			return "OP_RSHI";
		case OP_RSHU:
			return "OP_RSHU";
		case OP_NEGF:
			return "OP_NEGF";
		case OP_ADDF:
			return "OP_ADDF";
		case OP_SUBF:
			return "OP_SUBF";
		case OP_DIVF:
			return "OP_DIVF";
		case OP_MULF:
			return "OP_MULF";
		case OP_CVIF:
			return "OP_CVIF";
		case OP_CVFI:
			return "OP_CVFI";
		
		default:
			return "unknown";
	}
}*/

int get_int_cvar(const char* cvar) {
	if (!cvar || !*cvar)
		return -1;

	return ENG_SYSCALL(ENG_MSG(QMM_G_CVAR_VARIABLE_INTEGER_VALUE), cvar);
}

//this uses a cycling array of strings so the return value does not need to be stored locally
#define MAX_CVAR_LEN	256
const char* get_str_cvar(const char* cvar) {
	if (!cvar || !*cvar)
		return NULL;

	static char temp[8][MAX_CVAR_LEN];
	static int index = 0;
	int i = index;

	ENG_SYSCALL(ENG_MSG(QMM_G_CVAR_VARIABLE_STRING_BUFFER), cvar, temp[i], sizeof(temp[i]));
	index = (index + 1) & 7;
	return temp[i];
}

static int s_fh = -1;

void log_set(int fh) {
	s_fh = fh;
}

int log_get() {
	return s_fh;
}

int log_write(const char* text, int len) {
	if (s_fh != -1 && text && *text) {
		if (len == -1)
			len = strlen(text);

		return ENG_SYSCALL(ENG_MSG(QMM_G_FS_WRITE), text, len, s_fh);
	}
	
	return -1;
}

const char* my_strcasestr(const char* s1, const char* s2) {
	if (!s1 || !*s1 || !s2 || !*s2)
		return NULL;

	//store string length
	int slen1 = strlen(s1);
	int slen2 = strlen(s2);

	//allocate space for lowercase versions of the strings
	char* temps1 = (char*)malloc(slen1+1);
	char* temps2 = (char*)malloc(slen2+1);

	//null terminate
	temps1[slen1] = '\0';
	temps2[slen2] = '\0';

	//copy strings
	int i;
	for (i = 0; i < slen1; ++i)
		temps1[i] = (char)tolower(s1[i]);

	for (i = 0; i < slen2; ++i)
		temps2[i] = (char)tolower(s2[i]);

	//search
	char* x = strstr(temps1, temps2);

	//free allocated space
	free(temps1);
	free(temps2);

	if (!x)
		return NULL;

	//determine the offset into the string that the match occurred and add to s1
	return s1+(x-temps1);
}

int isnumber(const char* str) {
	for (int i = 0; str[i]; ++i) {
		if (str[i] < '0' || str[i] > '9')
			return 0;
	}

	return 1;
}

int write_file(const char* file, const char* outfile) {
	outfile = vaf("%s/%s", g_EngineMgr->GetModDir(), outfile ? outfile : file);
	
	//check if the real file already exists
	FILE* ffile = fopen(outfile, "r");
	if (ffile) {
		fclose(ffile);
		return 0;
	}

	//open file from inside pk3
	int fpk3, fsize = ENG_SYSCALL(ENG_MSG(QMM_G_FS_FOPEN_FILE), file, &fpk3, ENG_MSG(QMM_FS_READ));
	if (fsize <= 0) {
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_FCLOSE_FILE), fpk3);
		return 0;
	}

	//open output file
	ffile = fopen(outfile, "wb");
	if (!ffile) {
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_FCLOSE_FILE), fpk3);
		return 0;
	}

	//read file in blocks of 512
	byte buf[512];
	int left = fsize;
	while (left >= sizeof(buf)) {
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), buf, sizeof(buf), fpk3);
		fwrite(buf, sizeof(buf), 1, ffile);
		left -= sizeof(buf);
	}
	if (left) {
		ENG_SYSCALL(ENG_MSG(QMM_G_FS_READ), buf, left, fpk3);
		fwrite(buf, left, 1, ffile);
	}

	//close file handles
	ENG_SYSCALL(ENG_MSG(QMM_G_FS_FCLOSE_FILE), fpk3);
	fclose(ffile);

	return fsize;
}
