#
# RocketMod Makefile
# Adapted from the Quake3 Makefile from http://tremulous.net/Q3A-source/
#

include Config.mak

CC=g++

BASE_CFLAGS=-pipe -m32

BROOT=linux
BR=$(BROOT)/release
BD=$(BROOT)/debug

B_Q3A=

OBJR_Q3A=$(SRC_FILES:%.cpp=$(BR)$(B_Q3A)/%.o)
OBJD_Q3A=$(SRC_FILES:%.cpp=$(BD)$(B_Q3A)/%.o)

DEBUG_CFLAGS=$(BASE_CFLAGS) -g -pg
RELEASE_CFLAGS=$(BASE_CFLAGS) -DNDEBUG -O2 -fomit-frame-pointer -pipe -ffast-math -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing -fstrength-reduce 

SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared -m32

help:
	@echo RocketMod supports the following make rules:
	@echo all - builds release versions for all supported games
	@echo all_d - builds debug versions for all supported games
	@echo q3a - builds release version for Quake 3 Arena
	@echo q3a_d - builds debug version for Quake 3 Arena
	@echo clean - cleans all output files and directories
	
all: q3a

all_d: q3a_d

q3a: $(BR)/$(BINARY).so
q3a_d: $(BD)/$(BINARY).so

#quake 3 arena
$(BR)$(B_Q3A)/$(BINARY).so: $(BR)$(B_Q3A) $(OBJR_Q3A)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_Q3A)
  
$(BD)$(B_Q3A)/$(BINARY).so: $(BD)$(B_Q3A) $(OBJD_Q3A)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_Q3A)
  
$(BR)$(B_Q3A)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(Q3A_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_Q3A)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(Q3A_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_Q3A):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_Q3A):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

clean:
	@rm -rf $(BD)$(B_Q3A) $(BR)$(B_Q3A)
