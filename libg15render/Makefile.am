LLIBDIR=@LIBDIR@ 
AM_CFLAGS=@CFLAGS@ 
lib_LTLIBRARIES = libg15render.la
libg15render_la_SOURCES = libg15render.h text.c pixel.c screen.c font_6x4.c font_7x5.c font_8x8.c
libg15render_la_LIBADD = @FTLIB@
libg15render_la_LDFLAGS = -version-info 2:0:1
include_HEADERS= libg15render.h
EXTRA_DIST = debian Doxyfile libg15render.3
man3_MANS = $(top_srcdir)/libg15render.3
docdir = $(prefix)/share/doc/$(PACKAGE)-$(VERSION)
doc_DATA = AUTHORS ChangeLog COPYING NEWS README

dist-hook:
	rm -rf `find $(distdir)/debian -name .svn`
