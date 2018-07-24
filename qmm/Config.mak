Q3A_FLAGS = -DGAME_Q3A -I../source/q3sdk
RTCW_FLAGS = -DGAME_RTCW -I../source/rtcwsdk
JKA_FLAGS = -DGAME_JKA -I../source/jkasdk
JK2_FLAGS = -DGAME_JK2 -I../source/jk2sdk
COD_FLAGS = -DGAME_COD -I../source/codsdk

BINARY = qmmi386
SRC_FILES = CDLL.cpp CEngine.cpp CMod.cpp CPlugin.cpp main.cpp util.cpp
HDR_FILES = CDLL.h CEngine.h CLinkList.h CMod.h CPlugin.h main.h qmm.h util.h version.h
