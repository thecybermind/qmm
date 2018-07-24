Q3A_FLAGS = -DGAME_Q3A -I../sdks/q3a/game -I../qmm1
RTCWMP_FLAGS = -DGAME_RTCWMP -I../sdks/rtcwmp/game -I../qmm1
JKA_FLAGS = -DGAME_JKA -I../sdks/jka/game -I../qmm1
JK2_FLAGS = -DGAME_JK2 -I../sdks/jk2/game -I../qmm1
RTCWSP_FLAGS = -DGAME_RTCWSP -I../sdks/rtcwsp/game -I../qmm1
RTCWET_FLAGS = -DGAME_RTCWET -I../sdks/rtcwet/game -I../qmm1

BINARY = qadmin_qmm
SRC_FILES = cmds.cpp main.cpp util.cpp vote.cpp
HDR_FILES = main.h util.h version.h vote.h
