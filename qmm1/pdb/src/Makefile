# Minimal Makefile
#
# By: Michael Laforest <paralizer -AT- users -DOT- sourceforge -DOT- net>

CC = gcc

CFLAGS = -Wall -pipe -m32
#-g
LDFLAGS = -shared
DEPFLAGS = -O2 -fPIC -fomit-frame-pointer -ffast-math -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing -fstrength-reduce

# Needed to recognize .c as a file suffix
.SUFFIXES: $(SUFFIXES) .

BIN = pdb.so
INCLUDES = -I. -I../include
OBJS =	binarytree.o \
	hash.o \
	list.o \
	pdb.o \
	pdb_file.o \
	pdb_parse.o \
	pdb_types.o \
	str.o \

all: $(BIN)

clean:
	rm *.o

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -o $@ -c $*.c
	