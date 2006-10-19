AM_CFLAGS=@CFLAGS@ 
AM_CXXFLAGS=@CXXFLAGS@ 
bin_PROGRAMS = g15composer
g15composer_SOURCES = G15Composer.cpp G15Composer.h main.cpp g15c_logo.h
g15composer_LDADD = @FTLIB@
EXTRA_DIST = debian examples

dist-hook:
	rm -rf `find $(distdir)/debian -name .svn`
	rm -rf `find $(distdir)/examples -name .svn`
