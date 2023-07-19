SUBDIRS = messages common scifserver scifclient tests
AUTOMAKE_OPTIONS = subdir-objects 
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = LICENSE autogen.sh

.PHONY: docs beautify 

docs:
	doxygen ./docs/scifserver.dox

beautify:
	$(shell find . -name '*.[ch]' | uncrustify -c ./uncrustify-linux.cfg -F -)
