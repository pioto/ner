dnl ner: configure.ac

AC_PREREQ(2.59)

AC_INIT([ner], [0.0.1], [michael@obberon.com])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_LANG([C++])

AC_CONFIG_SRCDIR([src/ner.cc])
AC_CONFIG_MACRO_DIR([m4])

AX_PROG_CXX_CXX0X(, AC_MSG_ERROR(No C++0x compiler found))

dnl Check for libraries {{{
AC_CHECK_LIB(notmuch, notmuch_database_open,,
    [AC_MSG_ERROR([ner requires libnotmuch])])
AC_CHECK_LIB(ncurses, initscr,,
    [AC_MSG_ERROR([ner requires ncurses])])
dnl }}}

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

