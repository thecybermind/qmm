#
# QMM Makefile
# Adapted from the Quake3 Makefile from http://tremulous.net/Q3A-source/
#

include Config.mak

CC=g++
CC_C=gcc

BASE_CFLAGS=-pipe -m32

BROOT=linux
BR=$(BROOT)/release
BD=$(BROOT)/debug

OBJR=$(SRC_FILES:%.cpp=$(BR)/%.o) $(BR)/pdb.o
OBJD=$(SRC_FILES:%.cpp=$(BD)/%.o) $(BD)/pdb.o

DEBUG_CFLAGS=$(BASE_CFLAGS) -g -pg 
RELEASE_CFLAGS=$(BASE_CFLAGS) -O2 -fPIC -fomit-frame-pointer -ffast-math -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing -fstrength-reduce 

SHLIBCFLAGS=
#-fPIC
SHLIBLDFLAGS=-shared -m32

help:
	@echo QMM supports the following make rules:
	@echo release - builds release version
	@echo debug - builds debug version
	@echo clean - cleans all output files
	
release:
	@echo ---
	@echo --- building qmm \(release\)
	@echo ---
	$(MAKE) $(BR)/$(BINARY).so

	@echo ---
	@echo --- building pdb
	@echo ---
	$(MAKE) $(BR)/pdb.so


	@echo ---
	@echo --- Release build complete.
	@echo ---
	@echo --- Binaries are in linux/release
	@echo ---
	@echo --- Please read readme.txt for installation instructions.
	@echo ---

debug: $(BD)/$(BINARY).so

$(BR)/$(BINARY).so: $(BR) $(OBJR)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR)
  
$(BD)/$(BINARY).so: $(BD) $(OBJD)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD)

$(BR)/pdb.o: pdb.c pdb.h
	$(CC_C) $(RELEASE_CFLAGS) $(FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BD)/pdb.o: pdb.c pdb.h
	$(CC_C) $(DEBUG_CFLAGS) $(FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)/pdb.so:
	cd pdb/ && $(MAKE)
	cp pdb/src/pdb.so $(BR)

$(BR):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi
	
clean:
	@rm -rf $(BD) $(BR) $(BROOT)
	cd pdb/ && $(MAKE) clean