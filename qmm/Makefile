#
# QMM Makefile
# Adapted from the Quake3 Makefile from http://tremulous.net/Q3A-source/
#

include Config.mak

CC=gcc

BASE_CFLAGS=-pipe

BROOT=linux
BR=$(BROOT)/release
BD=$(BROOT)/debug

B_Q3A=
B_RTCW=_rtcw
B_JKA=_jka
B_JK2=_jk2
B_COD=_cod

OBJR_Q3A=$(SRC_FILES:%.cpp=$(BR)$(B_Q3A)/%.o)
OBJD_Q3A=$(SRC_FILES:%.cpp=$(BD)$(B_Q3A)/%.o)
OBJR_RTCW=$(SRC_FILES:%.cpp=$(BR)$(B_RTCW)/%.o)
OBJD_RTCW=$(SRC_FILES:%.cpp=$(BD)$(B_RTCW)/%.o)
OBJR_JKA=$(SRC_FILES:%.cpp=$(BR)$(B_JKA)/%.o)
OBJD_JKA=$(SRC_FILES:%.cpp=$(BD)$(B_JKA)/%.o)
OBJR_JK2=$(SRC_FILES:%.cpp=$(BR)$(B_JK2)/%.o)
OBJD_JK2=$(SRC_FILES:%.cpp=$(BD)$(B_JK2)/%.o)
OBJR_COD=$(SRC_FILES:%.cpp=$(BR)$(B_COD)/%.o)
OBJD_COD=$(SRC_FILES:%.cpp=$(BD)$(B_COD)/%.o)

DEBUG_CFLAGS=$(BASE_CFLAGS) -g -pg
RELEASE_CFLAGS=$(BASE_CFLAGS) -DNDEBUG -O2 -fomit-frame-pointer -pipe -ffast-math -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing -fstrength-reduce 

SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared -ldl -lstdc++

help:
	@echo QMM supports the following make rules:
	@echo all - builds release and debug versions for all supported games
	@echo all_r - builds release versions for all supported games
	@echo all_d - builds debug versions for all supported games
	@echo q3a - builds release and debug versions for Quake 3 Arena
	@echo q3a_r - builds release version for Quake 3 Arena
	@echo q3a_d - builds debug version for Quake 3 Arena
	@echo rtcw - builds release and debug versions for Return to Castle Wolfenstein
	@echo rtcw_r - builds release version for Return to Castle Wolfenstein
	@echo rtcw_d - builds debug version for Return to Castle Wolfenstein
	@echo jka - builds release and debug versions for Jedi Knight
	@echo jka_r - builds release version for Jedi Knight
	@echo jka_d - builds debug version for Jedi Knight
	@echo jk2 - builds release and debug versions for Jedi Knight
	@echo jk2_r - builds release version for Jedi Knight
	@echo jk2_d - builds debug version for Jedi Knight
	@echo cod - builds release and debug versions for Call of Duty
	@echo cod_r - builds release version for Call of Duty
	@echo cod_d - builds debug version for Call of Duty
	@echo clean - cleans all output files and directories
	
all: all_r all_d

all_r: q3a_r rtcw_r jka_r jk2_r cod_r

all_d: q3a_d rtcw_d jka_d jk2_d cod_d

q3a: q3a_r q3a_d
q3a_r: $(BR)/$(BINARY).so
q3a_d: $(BD)/$(BINARY).so

rtcw: rtcw_r rtcw_d
rtcw_r: $(BR)$(B_RTCW)/$(BINARY).so
rtcw_d: $(BD)$(B_RTCW)/$(BINARY).so

jka: jka_r jka_d
jka_r: $(BR)$(B_JKA)/$(BINARY).so
jka_d: $(BD)$(B_JKA)/$(BINARY).so

jk2: jk2_r jk2_d
jk2_r: $(BR)$(B_JK2)/$(BINARY).so
jk2_d: $(BD)$(B_JK2)/$(BINARY).so

cod: cod_r cod_d
cod_r: $(BR)$(B_COD)/$(BINARY).so
cod_d: $(BD)$(B_COD)/$(BINARY).so

#quake 3 arena
$(BR)$(B_Q3A)/$(BINARY).so: $(BR)$(B_Q3A) $(OBJR_Q3A)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_Q3A)
  
$(BD)$(B_Q3A)/$(BINARY).so: $(BD)$(B_Q3A) $(OBJD_Q3A)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_Q3A)
  
#return to castle wolfenstein
$(BR)$(B_RTCW)/$(BINARY).so: $(BR)$(B_RTCW) $(OBJR_RTCW)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_RTCW)
  
$(BD)$(B_RTCW)/$(BINARY).so: $(BD)$(B_RTCW) $(OBJD_RTCW)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_RTCW)

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

#call of duty
$(BR)$(B_COD)/$(BINARY).so: $(BR)$(B_COD) $(OBJR_COD)
	$(CC) $(RELEASE_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJR_COD)
  
$(BD)$(B_COD)/$(BINARY).so: $(BD)$(B_COD) $(OBJD_COD)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJD_COD)


$(BR)$(B_Q3A)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(Q3A_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_Q3A)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(Q3A_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_RTCW)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(RTCW_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_RTCW)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(RTCW_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_JKA)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(JKA_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_JKA)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(JKA_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_JK2)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(JK2_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_JK2)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(JK2_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_COD)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(RELEASE_CFLAGS) $(COD_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
$(BD)$(B_COD)/%.o: %.cpp $(HDR_FILES)
	$(CC) $(DEBUG_CFLAGS) $(COD_FLAGS) $(SHLIBCFLAGS) -o $@ -c $<

$(BR)$(B_Q3A):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_Q3A):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi
	
$(BR)$(B_RTCW):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_RTCW):
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

$(BR)$(B_COD):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi

$(BD)$(B_COD):
	@if [ ! -d $(BROOT) ];then mkdir $(BROOT);fi
	@if [ ! -d $(@) ];then mkdir $@;fi
	
clean:
	@rm -rf $(BD)$(B_Q3A) $(BR)$(B_Q3A) $(BD)$(B_RTCW) $(BR)$(B_RTCW) $(BD)$(B_JKA) $(BRA)$(B_JKA) $(BD)$(B_JK2) $(BR)$(B_JK2) $(BD)$(B_COD) $(BR)$(B_COD)
