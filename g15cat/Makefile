CC = gcc
CFLAGS= -Wall -pedantic -lg15render -lg15daemon_client

.PHONY: clean cleanall



g15cat:  g15cat.c g15cat.h display.o

display: display.c display.h


install:
	cp -f g15cat /usr/bin/

uninstall: 
	rm -f /usr/bin/g15cat


	

clean:	
	-rm -f *.o *~ core g15cat
