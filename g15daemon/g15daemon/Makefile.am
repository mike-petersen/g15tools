
METASOURCES = AUTO
sbin_PROGRAMS = g15daemon
noinst_PROGRAMS = g15daemontest
g15daemon_SOURCES = g15_net.c g15_uinput.c libg15.h logo.h utility_funcs.c \
	g15daemon.h main.c
g15daemontest_SOURCES = lcdclient_test.c
INCLUDES = -I$(top_builddir)/libg15daemon_client/
g15daemontest_LDADD = $(top_builddir)/libg15daemon_client/libg15daemon_client.la