#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(g15cat, 1.1, robynhub@gmail.com)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([g15cat.c])
AC_CONFIG_HEADER([config.h])

AC_PREFIX_DEFAULT(/usr)
# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET


# Checks for libraries.
AC_CHECK_LIB([g15daemon_client], [g15_send], ,AC_MSG_ERROR(["libg15daemon_client not found. please install it"]))
AC_CHECK_LIB([g15render], [g15r_loadWbmpToBuf], ,AC_MSG_ERROR([">=libg15render-1.2 not found. please install it"]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h signal.h fcntl.h stdio.h stdlib.h string.h sys/stat.h sys/types.h unistd.h g15daemon_client.h libg15.h libg15render.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([malloc fgetc getopt feof printf sigaction sleep])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
