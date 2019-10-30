# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libbml], [0.1], [johann.baudy@seagnal.fr])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR
AC_PROG_LIBTOOL
#AC_LANG([C++])
#AX_CXX_COMPILE_STDCXX_11

# Checks for libraries.
LT_INIT
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_PROG_RANLIB
CXXFLAGS+=" -std=c++11"


# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

# Check module
PKG_CHECK_MODULES([CHECK], [check >= 0.9.10])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
