#
# STUB_QMM Makefile
# Adapted from the Quake3 Makefile from http://tremulous.net/Q3A-source/
#

include Config.mak

CC=g++

BASE_CFLAGS=-pipe 

BROOT=linux
BR=$(BROOT)/release
BD=$(BROOT)/debug

B_Q3A=
B_RTCWMP=_rtcwmp
B_JKA=_jka
B_JK2=_jk2
B_RTCWSP=_rtcwsp
B_RTCWET=_rtcwet

OBJR_Q3A=$(SRC_FILES:%.cpp=$(BR)$(B_Q3A)/%.o)
OBJD_Q3A=$(SRC_FILES:%.cpp=$(BD)$(B_Q3A)/%.o)
OBJR_RTCWMP=$(SRC_FILES:%.cpp=$(BR)$(B_RTCWMP)/%.o)
OBJD_RTCWMP=$(SRC_FILES:%.cpp=$(BD)$(B_RTCWMP)/%.o)
OBJR_JKA=$(SRC_FILES:%.cpp=$(BR)$(B_JKA)/%.o)
OBJD_JKA=$(SRC_FILES:%.cpp=$(BD)$(B_JKA)/%.o)
OBJR_JK2=$(SRC_FILES:%.cpp=$(BR)$(B_JK2)/%.o)
OBJD_JK2=$(SRC_FILES:%.cpp=$(BD)$(B_JK2)/%.o)
OBJR_RTCWSP=$(SRC_FILES:%.cpp=$(BR)$(B_RTCWSP)/%.o)
OBJD_RTCWSP=$(SRC_FILES:%.cpp=$(BD)$(B_RTCWSP)/%.o)
OBJR_RTCWET=$(SRC_FILES:%.cpp=$(BR)$(B_RTCWET)/%.o)
OBJD_RTCWET=$(SRC_FILES:%.cpp=$(BD)$(B_RTCWET)/%.o)

DEBUG_CFLAGS=$(BASE_CFLAGS) -g -pg
RELEASE_CFLAGS=$(BASE_CFLAGS) -DNDEBUG -O2 -fomit-frame-pointer -pipe -ffast-math -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing -fstrength-reduce 

SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

help:
	@echo STUB_QMM supports the following make rules:
	@echo all - builds release versions for all supported games
	@echo all_d - builds debug versions for all supported games
	@echo q3a - builds release version for Quake 3 Arena
	@echo q3a_d - builds debug version for Quake 3 Arena
	@echo jka - builds release version for Jedi Knight: Jedi Academy
	@echo jka_d - builds debug version for Jedi Knight: Jedi Academy
	@echo jk2 - builds release version for Jedi Knight II: Jedi Outcast
	@echo jk2_d - builds debug version for Jedi Knight II: Jedi Outcast
	@echo rtcwmp - builds release version for Return to Castle Wolfenstein \(MP\)
	@echo rtcwmp_d - builds debug version for Return to Castle Wolfenstein \(MP\)
	@echo rtcwsp - builds release version for Return to Castle Wolfenstein \(SP\)
	@echo rtcwsp_d - builds debug version for Return to Castle Wolfenstein \(SP\)
	@echo rtcwet - builds release version for RtCW: Enemy Territory
	@echo rtcwet_d - builds debug version for RtCW: Enemy Territory
	@echo clean - cleans all output files and directories
	
all: q3a rtcwmp jka jk2 rtcwsp rtcwet

all_d: q3a_d rtcwmp_d jka_d jk2_d rtcwsp_d rtcwet_d

q3a: $(BR)/$(BINARY).so
q3a_d: $(BD)/$(BINARY).so

rtcwmp: $(BR)$(B_RTCWMP)/$(BINARY).so
rtcwmp_d: $(BD)$(B_RTCWMP)/$(BINARY).so

jka: $(BR)$(B_JKA)/$(BINARY).so
jka_d: $(BD)$(B_JKA)/$(BINARY).so

jk2: $(BR)$(B_JK2)/$(BINARY).so
jk2_d: $(BD)$(B_JK2)/$(BINARY).so

rtcwsp: $(BR)$(B_RTCWSP)/$(BINARY).so
rtcwsp_d: $(BD)$(B_RTCWSP)/$(BINARY).so

rtcwet: $(BR)$(B_RTCWET)/$(BINARY).so
rtcwet_d: $(BD)$(B_RTCWET)/$(BINARY).so

#quake 3 arena
$(BR)$(B_Q3A)/$(BINARY).so: $(BR)$(B_Q3A) $(OBJR_Q3A)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_Q3A)
  
$(BD)$(B_Q3A)/$(BINARY).so: $(BD)$(B_Q3A) $(OBJD_Q3A)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_Q3A)
  
#return to castle wolfenstein (multiplayer)
$(BR)$(B_RTCWMP)/$(BINARY).so: $(BR)$(B_RTCWMP) $(OBJR_RTCWMP)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_RTCWMP)
  
$(BD)$(B_RTCWMP)/$(BINARY).so: $(BD)$(B_RTCWMP) $(OBJD_RTCWMP)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_RTCWMP)

#jedi knight academy
$(BR)$(B_JKA)/$(BINARY).so: $(BR)$(B_JKA) $(OBJR_JKA)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_JKA)
  
$(BD)$(B_JKA)/$(BINARY).so: $(BD)$(B_JKA) $(OBJD_JKA)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_JKA)

#jedi knight 2
$(BR)$(B_JK2)/$(BINARY).so: $(BR)$(B_JK2) $(OBJR_JK2)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_JK2)
  
$(BD)$(B_JK2)/$(BINARY).so: $(BD)$(B_JK2) $(OBJD_JK2)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_JK2)

#return to castle wolfenstein (single player)
$(BR)$(B_RTCWSP)/$(BINARY).so: $(BR)$(B_RTCWSP) $(OBJR_RTCWSP)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_RTCWSP)
  
$(BD)$(B_RTCWSP)/$(BINARY).so: $(BD)$(B_RTCWSP) $(OBJD_RTCWSP)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_RTCWSP)

#return to castle wolfenstein: enemy territory
$(BR)$(B_RTCWET)/$(BINARY).so: $(BR)$(B_RTCWET) $(OBJR_RTCWET)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_RTCWET)
  
$(BD)$(B_RTCWET)/$(BINARY).so: $(BD)$(B_RTCWET) $(OBJD_RTCWET)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_RTCWET)


$(BR)$(B_Q3A)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(Q3A_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_Q3A)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(Q3A_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_RTCWMP)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(RTCWMP_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_RTCWMP)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(RTCWMP_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_JKA)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(JKA_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_JKA)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(JKA_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_JK2)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(JK2_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_JK2)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(JK2_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_RTCWSP)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(RTCWSP_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_RTCWSP)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(RTCWSP_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_RTCWET)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(RTCWET_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_RTCWET)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(RTCWET_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_Q3A):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_Q3A):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi
	
$(BR)$(B_RTCWMP):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_RTCWMP):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi
	
$(BR)$(B_JKA):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_JKA):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BR)$(B_JK2):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_JK2):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BR)$(B_RTCWSP):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_RTCWSP):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi
	
$(BR)$(B_RTCWET):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_RTCWET):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi
	
clean:
	@rm -rf $(BD)$(B_Q3A) $(BR)$(B_Q3A) $(BD)$(B_RTCWMP) $(BR)$(B_RTCWMP) $(BD)$(B_JKA) $(BRA)$(B_JKA) $(BD)$(B_JK2) $(BR)$(B_JK2) $(BD)$(B_RTCWSP) $(BR)$(B_RTCWSP) $(BD)$(B_RTCWET) $(BR)$(B_RTCWET)
