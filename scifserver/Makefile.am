ACLOCAL_AMFLAGS = -I m4

GITREV := $(shell git rev-parse HEAD || echo "NONE")
GITDIRTY := $(shell git update-index --refresh >/dev/null 2>&1; git diff-index --exit-code HEAD >/dev/null 2>&1 || echo "-dirty")


sbin_PROGRAMS= scifserver
scifserver_SOURCES = main.c serversocket.c client.c client_handler_fns.c
scifserver_CFLAGS = $(GLIB_CFLAGS) $(OPENSSL_CFLAGS) $(PROTOBUFC_CFLAGS)
scifserver_LDADD = ${top_srcdir}/messages/libmessages.a $(GLIB_LIBS) $(OPENSSL_LIBS) $(PROTOBUFC_LIBS)

