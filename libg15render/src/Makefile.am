LLIBDIR=@LIBDIR@ 
AM_CFLAGS=@CFLAGS@ 

DISTCLEANFILES = Makefile.in

lib_LTLIBRARIES = libg15render.la
libg15render_la_SOURCES = libg15render.h text.c pixel.c screen.c
libg15render_la_LIBADD = @FTLIB@
libg15render_la_LDFLAGS = -version-info 2:0:1
include_HEADERS= libg15render.h

bin_PROGRAMS = g15fontconvert
g15fontconvert_SOURCES = g15fontconvert.c
g15fontconvert_LDADD = libg15render.la @FTLIB@
