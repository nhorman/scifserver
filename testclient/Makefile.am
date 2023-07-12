AUTOMAKE_OPTIONS = subdir-objects 
ACLOCAL_AMFLAGS = -I m4

sbin_PROGRAMS= scifclient
scifclient_SOURCES = main.c 
scifclient_CFLAGS = -I${top_srcdir}/include $(GLIB_CFLAGS) $(OPENSSL_CFLAGS) $(PROTOBUFC_CFLAGS)
scifclient_LDADD = ${top_srcdir}/messages/libmessages.a $(GLIB_LIBS) $(OPENSSL_LIBS) $(PROTOBUFC_LIBS)
scifclient_DEPENDENCIES = ${top_srcdir}/messages/messages.pb-c.c

