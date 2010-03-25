dnl ner: configure.ac

AC_PREREQ(2.59)

AC_INIT([ner], [0.0.1], [michael@obberon.com])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_LANG([C++])

AC_CONFIG_SRCDIR([src/ner.cc])
AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AX_PROG_CXX_CXX0X(, AC_MSG_ERROR(No C++0x compiler found))

PKG_PROG_PKG_CONFIG([0.9.0])

dnl Check for libraries {{{
PKG_CHECK_MODULES([gmime], [gmime-2.4],,
    [AC_MSG_ERROR([ner requires gmime])])
AC_CHECK_LIB(notmuch, notmuch_database_open,,
    [AC_MSG_ERROR([ner requires libnotmuch])])
AC_CHECK_LIB(ncursesw, initscr,,
    [AC_MSG_ERROR([ner requires ncurses])])
dnl }}}

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

