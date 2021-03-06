# Process this file with autoconf to produce a configure script.
#
# Copyright (C) 2002-2004 Carnegie Mellon University
# Written by H�kan L. S. Younes.
#
# Permission is hereby granted to distribute this software for
# non-commercial research purposes, provided that this copyright
# notice is included with any such distribution.
#
# THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
# SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
# ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
#
# $Id: configure.ac,v 1.9 2003-03-01 18:56:48 lorens Exp $

AC_INIT(VHPOP, 3.0, lorens@cs.cmu.edu)
AC_CONFIG_SRCDIR(vhpop.cc)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
AM_PROG_LEX
AC_PROG_YACC

AC_LANG(C++)

# Checks for libraries.
AC_SEARCH_LIBS(gettext, intl)

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stdlib.h string.h strings.h sys/time.h unistd.h sstream])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
#AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit memset select strcasecmp strerror strncasecmp])
AC_CHECK_FUNCS(getopt_long, , [AC_LIBOBJ(getopt) AC_LIBOBJ(getopt1)])
AC_CHECK_FUNCS(isinf, , [AC_EGREP_HEADER(finite, math.h, ,
                                         [AC_CHECK_HEADERS(ieeefp.h)])])
AC_CONFIG_FILES(Makefile)
AC_OUTPUT
