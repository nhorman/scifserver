SUBDIRS = component config
AUTOMAKE_OPTIONS = subdir-objects 
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = LICENSE autogen.sh

GITREV := $(shell git rev-parse HEAD || echo "NONE")
GITDIRTY := $(shell git update-index --refresh >/dev/null 2>&1; git diff-index --exit-code HEAD >/dev/null 2>&1 || echo "-dirty")

.PHONY: docs 

SCIFLIBS := ./component/libcomponent.a ./config/libconfigure.a

sbin_PROGRAMS= scifserver
scifserver_SOURCES = main.c

scifserver_LDFLAGS = -Wl,--whole-archive $(SCIFLIBS) -Wl,--no-whole-archive

docs:
	doxygen ./docs/scifserver.dox
