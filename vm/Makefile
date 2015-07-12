#
# QMMVM Makefile
# Adapted from the Quake3 Makefile from http://tremulous.net/Q3A-source/
#

include Config.mak

CC=gcc

BASE_CFLAGS=-pipe 

BROOT=linux
BR=$(BROOT)/release
BD=$(BROOT)/debug

B_Q3A=
B_JK2=_jk2

OBJR_Q3A=$(SRC_FILES:%.cpp=$(BR)$(B_Q3A)/%.o)
OBJD_Q3A=$(SRC_FILES:%.cpp=$(BD)$(B_Q3A)/%.o)
OBJR_JK2=$(SRC_FILES:%.cpp=$(BR)$(B_JK2)/%.o)
OBJD_JK2=$(SRC_FILES:%.cpp=$(BD)$(B_JK2)/%.o)

DEBUG_CFLAGS=$(BASE_CFLAGS) -g -pg
RELEASE_CFLAGS=$(BASE_CFLAGS) -DNDEBUG -O2 -fomit-frame-pointer -pipe -ffast-math -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing -fstrength-reduce 

SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

help:
	@echo QMMVM supports the following make rules:
	@echo all - builds release and debug versions for all supported games
	@echo all_r - builds release versions for all supported games
	@echo all_d - builds debug versions for all supported games
	@echo q3a - builds release and debug versions for Quake 3 Arena
	@echo q3a_r - builds release version for Quake 3 Arena
	@echo q3a_d - builds debug version for Quake 3 Arena
	@echo jk2 - builds release and debug versions for Jedi Knight
	@echo jk2_r - builds release version for Jedi Knight
	@echo jk2_d - builds debug version for Jedi Knight
	@echo clean - cleans all output files and directories
	
all: all_r all_d

all_r: q3a_r jk2_r

all_d: q3a_d jk2_d

q3a: q3a_r q3a_d
q3a_r: $(BR)/$(BINARY).so
q3a_d: $(BD)/$(BINARY).so

jk2: jk2_r jk2_d
jk2_r: $(BR)$(B_JK2)/$(BINARY).so
jk2_d: $(BD)$(B_JK2)/$(BINARY).so

#quake 3 arena
$(BR)$(B_Q3A)/$(BINARY).so: $(BR)$(B_Q3A) $(OBJR_Q3A)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_Q3A)
  
$(BD)$(B_Q3A)/$(BINARY).so: $(BD)$(B_Q3A) $(OBJD_Q3A)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_Q3A)
  
#jedi knight 2
$(BR)$(B_JK2)/$(BINARY).so: $(BR)$(B_JK2) $(OBJR_JK2)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_JK2)
  
$(BD)$(B_JK2)/$(BINARY).so: $(BD)$(B_JK2) $(OBJD_JK2)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_JK2)


$(BR)$(B_Q3A)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(Q3A_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_Q3A)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(Q3A_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_JK2)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(JK2_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_JK2)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(JK2_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<


$(BR)$(B_Q3A):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_Q3A):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi
	
$(BR)$(B_JK2):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_JK2):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

clean:
	@rm -rf $(BD)$(B_Q3A) $(BR)$(B_Q3A) $(BD)$(B_JK2) $(BR)$(B_JK2)
