# Process this file with autoconf to produce a configure script.
AC_INIT([wipfc], [2.0], [])
AM_INIT_AUTOMAKE

# allow to use a plain gcc compiler (gcc-3.4.6)
#CPPFLAGS="-std=c++0x -D __STDC_LIMIT_MACROS -I$srcdir/cpp -I$srcdir/../watcom/h"
#CXXFLAGS="-pipe -g -Wall -march=native"

CPPFLAGS="-D __STDC_LIMIT_MACROS -I$srcdir/cpp -I$srcdir/../watcom/h"
CXXFLAGS="-pipe -g -Wall"

AC_CONFIG_SRCDIR([cpp/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_LANG([C++])

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([algorithm])
AC_CHECK_HEADERS([clocale])
AC_CHECK_HEADERS([cstdint])
AC_CHECK_HEADERS([cstdio])
AC_CHECK_HEADERS([cstdlib])
AC_CHECK_HEADERS([cstring])
AC_CHECK_HEADERS([cwctype])
AC_CHECK_HEADERS([functional])
AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([limits])
AC_CHECK_HEADERS([list])
AC_CHECK_HEADERS([map])
AC_CHECK_HEADERS([memory])
AC_CHECK_HEADERS([numeric])
AC_CHECK_HEADERS([set])
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([vector])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([getcwd memmove memset setlocale strrchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
