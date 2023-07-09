SUBDIRS = 
AUTOMAKE_OPTIONS = subdir-objects 
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = COPYING autogen.sh

GITREV := $(shell git rev-parse HEAD || echo "NONE")
GITDIRTY := $(shell git update-index --refresh >/dev/null 2>&1; git diff-index --exit-code HEAD >/dev/null 2>&1 || echo "-dirty")

.PHONY: docs 

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/include -Wall -Werror
sbin_PROGRAMS= scifserver
scifserver_SOURCES = main.c


docs:
	doxygen ./docs/scifserver.dox
