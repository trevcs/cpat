dnl @synopsis MP_WITH_CURSES
dnl
dnl Detect SysV compatible curses, such as ncurses.
dnl
dnl 2006-02-18:  Trevor Carey-Smith
dnl	Altered so that the default in ncurses and the option is
dnl changed to "--with-curses" to force use of curses over ncurses.
dnl If a curses library is found, it is added to LIBS. 
dnl Also, it fails if no curses library is found.
dnl
dnl Defines HAVE_CURSES_H or HAVE_NCURSES_H if curses is found.
dnl CURSES_LIB is also set with the required libary, but is not
dnl appended to LIBS automatically. If no working curses libary is
dnl found CURSES_LIB will be left blank.
dnl
dnl This macro adds the option "--with-ncurses" to configure which can
dnl force the use of ncurses or nothing at all.
dnl
dnl @category InstalledPackages
dnl @author Mark Pulford <mark@kyne.com.au>
dnl @version 2002-04-06
dnl @license GPLWithACException

AC_DEFUN([MP_WITH_CURSES],
    [
	AC_ARG_WITH(curses, [  --with-curses          Force the use of curses over ncurses],,)
	mp_save_LIBS="$LIBS"
	CURSES_LIB=""
	if test "$with_curses" != yes
	then
	    AC_CACHE_CHECK([for working ncurses], mp_cv_ncurses,
		[LIBS="$mp_save_LIBS -lncurses"
	    AC_TRY_LINK(
		[#include <ncurses.h>],
		[chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
		mp_cv_ncurses=yes, mp_cv_ncurses=no)])
	    if test "$mp_cv_ncurses" = yes
	    then
		AC_DEFINE([HAVE_NCURSES_H], 1,[Define if your system has the ncurses lib.])
		CURSES_LIB="-lncurses"
	    fi
	fi
	if test ! "$CURSES_LIB"
	then
	    AC_CACHE_CHECK([for working curses], mp_cv_curses,
		[LIBS="$mp_save_LIBS -lcurses"
	    AC_TRY_LINK(
		[#include <curses.h>],
		[chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
		mp_cv_curses=yes, mp_cv_curses=no)])
	    if test "$mp_cv_curses" = yes
	    then
		AC_DEFINE([HAVE_CURSES_H], 1,[Define if your system has the curses lib.])
		CURSES_LIB="-lcurses"
	    fi
	fi
	if test ! "$CURSES_LIB"
	then
	    AC_MSG_ERROR([No curses library found],1)
	fi
	LIBS="$mp_save_LIBS $CURSES_LIB"
    ]
)dnl
