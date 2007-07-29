## Process this file with automake to produce Makefile.in

SUBDIRS = src

EXTRA_DIST = debian Doxyfile libg15render.3

MAINTAINERCLEANFILES = \
    configure \
    Makefile.in \
    config.h.in \
    aclocal.m4 \
    config/*

man3_MANS = $(top_srcdir)/libg15render.3
docdir = $(prefix)/share/doc/$(PACKAGE)-$(VERSION)
doc_DATA = AUTHORS ChangeLog COPYING NEWS README

dist-hook:
	rm -rf `find $(distdir)/debian -name .svn`

