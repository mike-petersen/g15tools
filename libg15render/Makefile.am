LLIBDIR=@LIBDIR@ 
CFLAGS=@CFLAGS@ `freetype-config --cflags`
lib_LTLIBRARIES = libg15render.la
libg15render_la_SOURCES = libg15render.h text.c pixel.c screen.c font_6x4.c font_7x5.c font_8x8.c
libg15render_la_LIBADD = -lfreetype
libg15render_la_LDFLAGS = -version-info 1:0:0
include_HEADERS= libg15render.h

