bin_PROGRAMS = g15composer
g15composer_SOURCES = g15composer.c g15composer.tab.c g15composer.lex.c g15composer.tab.h 
EXTRA_DIST = g15composer.l g15composer.y debian examples
noinst_HEADERS = g15composer.h g15c_logo.h

g15composer.tab.c: g15composer.lex.c g15composer.y g15composer.h
	$(YACC) -b g15composer -d g15composer.y

g15composer.lex.c: g15composer.l g15composer.h
	$(LEX) -o g15composer.lex.c g15composer.l

clean-local:
	rm -f g15composer.lex.c g15composer.tab.*

dist-hook:
	rm -rf `find $(distdir)/debian -name .svn`
	rm -rf `find $(distdir)/examples -name .svn`
