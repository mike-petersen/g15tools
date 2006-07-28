#Makefile for g15xmms

PREFIX=/usr

CC=gcc
CFLAGS= -O2 -I$(PREFIX)/include/gtk-1.2 -I$(PREFIX)/include/glib-1.2 -I$(PREFIX)/include -I$(PREFIX)/lib/glib/include


LIBS=

all: g15daemon_xmms_spectrum

g15daemon_xmms_spectrum.o: g15daemon_xmms_spectrum.c
		$(CC) $(CFLAGS) -c g15daemon_xmms_spectrum.c

g15daemon_xmms_spectrum: g15daemon_xmms_spectrum.o 
		$(CC) $(CFLAGS) -lg15daemon_client --shared $(LIBS) g15daemon_xmms_spectrum.o -o libg15daemon_xmms_spectrum.so 

clean:
		rm -f ./*.o ./*.so ./*~

install: libg15daemon_xmms_spectrum.so
		cp libg15daemon_xmms_spectrum.so `$(PREFIX)/bin/xmms-config --visualization-plugin-dir`
