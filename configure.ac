#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# AC_PREREQ(2.59)
AC_INIT(cpat,1.3.0,[trev@highwater.co.nz])

# AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/spider.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

m4_include([m4/ax_with_curses.m4])
# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcurses':
#AC_CHECK_LIB([ncurses], [main])
AX_WITH_CURSES

if test $ax_cv_curses == yes; then
    LIBS="$LIBS $CURSES_LIB"
else 
    AC_ERROR([A working curses library is required])
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([getopt.h pwd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_TYPES([struct timespec])

# Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([strchr getopt_long nanosleep usleep])
AC_CHECK_TYPES(time_t)

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile])
AC_OUTPUT
