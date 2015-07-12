# Minimal Makefile
#
# By: Michael Laforest <paralizer -AT- users -DOT- sourceforge -DOT- net>
#
# This Makefile will invoke the Makefile in
# the src/ subdirectory.
#
# $Header: /cvsroot-fuse/qmm/qmm1/pdb/Makefile,v 1.2 2005/09/22 01:02:15 cybermind Exp $

BIN = pdb
DBIN = pdb-debug

all: $(BIN)
debug: $(DBIN)

$(BIN):
	cd src/ && $(MAKE)

$(DBIN):
	cd src/ && $(MAKE) debug

clean:
	cd src/ && $(MAKE) clean
