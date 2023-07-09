SUBDIRS = component
AUTOMAKE_OPTIONS = subdir-objects 
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = LICENSE autogen.sh

GITREV := $(shell git rev-parse HEAD || echo "NONE")
GITDIRTY := $(shell git update-index --refresh >/dev/null 2>&1; git diff-index --exit-code HEAD >/dev/null 2>&1 || echo "-dirty")

.PHONY: docs 

SCIFLIBS := ./component/libcomponent.a

sbin_PROGRAMS= scifserver
scifserver_SOURCES = main.c

scifserver_LDFLAGS = -Wl,--whole-archive
scifserver_LDADD = $(SCIFLIBS)

docs:
	doxygen ./docs/scifserver.dox
