AM_CFLAGS=@CFLAGS@ 
AM_CXXFLAGS=@CXXFLAGS@ 
bin_PROGRAMS = g15composer
g15composer_SOURCES = main.cpp util.cpp g15c_logo.h composer.h
g15composer_LDADD = @FTLIB@
EXTRA_DIST = debian

dist-hook:
	rm -rf `find $(distdir)/debian -name .svn`
