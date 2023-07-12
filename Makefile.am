SUBDIRS = 
AUTOMAKE_OPTIONS = subdir-objects 
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = LICENSE autogen.sh

GITREV := $(shell git rev-parse HEAD || echo "NONE")
GITDIRTY := $(shell git update-index --refresh >/dev/null 2>&1; git diff-index --exit-code HEAD >/dev/null 2>&1 || echo "-dirty")

.PHONY: docs 

sbin_PROGRAMS= scifserver
scifserver_SOURCES = messages.pb-c.c main.c serversocket.c client.c client_handler_fns.c
scifserver_CFLAGS = $(GLIB_CFLAGS) $(OPENSSL_CFLAGS) $(PROTOBUFC_CFLAGS)
scifserver_LDADD = $(GLIB_LIBS) $(OPENSSL_LIBS) $(PROTOBUFC_LIBS)


docs:
	doxygen ./docs/scifserver.dox


messages.pb-c.c : messages.proto 
	$(PROTOC) --c_out=. $<

clean-generic:
	rm -f ./messages.pb-c.*
