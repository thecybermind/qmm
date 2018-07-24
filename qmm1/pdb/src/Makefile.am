## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES =\
	 -I../include/

AM_CFLAGS =\
	-fPIC\
	 -Wall\
	 -g -pg

AM_CXXFLAGS =\
	-fPIC\
	 -Wall\
	 -g -pg

bin_PROGRAMS = pdb

pdb_SOURCES = \
	pdb.c\
	pdb_file.c\
	pdb_parse.c\
	pdb_types.c\
	binarytree.c\
	list.c\
	str.c\
	hash.c

pdb_LDFLAGS = \
	-shared

pdb_LDADD = \
	 -lm

