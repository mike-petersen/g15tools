bin_PROGRAMS = g15cat
g15cat_SOURCES = display.c g15cat.c g15cat.h display.h
EXTRA_DIST = doc
man1_MANS = $(top_srcdir)/man/g15cat.1
docdir = $(prefix)/share/doc/$(PACKAGE)-$(VERSION)
doc_DATA = AUTHORS COPYING README

dist-hook:
	rm -rf `find $(distdir)/doc -name .svn`
