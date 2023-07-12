ACLOCAL_AMFLAGS = -I m4

noinst_LIBRARIES= libmessages.a 
libmessages_a_SOURCES = messages.pb-c.c
libmessage_a_CFLAGS = $(PROTOBUFC_CFLAGS)


messages.pb-c.c : messages.proto 
	$(PROTOC) --c_out=. $<

clean-generic:
	rm -f ./messages.pb-c.* testclient/messages.pb-c.*
