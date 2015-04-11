#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
m4_define([autoforward_major], [0])
m4_define([autoforward_minor], [7])
m4_define([autoforward_micro], [6])
m4_define([autoforward_version],
	  [autoforward_major.autoforward_minor.autoforward_micro])
AC_INIT([autoforward], [autoforward_version], [kenhys@gmail.com])
AC_CONFIG_SRCDIR([src/autoforward.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE

AC_DEFINE(MAJOR_VERSION, ["autoforward_major"], [Major version])
AC_DEFINE(MINOR_VERSION, ["autoforward_minor"], [Minor version])
AC_DEFINE(MICRO_VERSION, ["autoforward_micro"], [Micro version])
AC_SUBST([MAJOR_VERSION], [autoforward_major])
AC_SUBST([MINOR_VERSION], [autoforward_minor])
AC_SUBST([MICRO_VERSION], [autoforward_micro])

m4_define([BUILD_NUMBER], m4_include(build_number))
AC_DEFINE([BUILD_REVISION], [BUILD_NUMBER], [Build revision])
AC_SUBST(BUILD_REVISION, [BUILD_NUMBER])

AC_DEFINE([SYLPF_ID], [PACKAGE_NAME], [""])
AC_DEFINE([SYLPF_OPTION], [autoforward_option], [""])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AC_ARG_ENABLE(debug,
              [AS_HELP_STRING([--enable-debug],
                              [use debug flags (default=no)])],
  [plugin_debug="$enableval"],
  [plugin_debug="no"])
if test "x$plugin_debug" != "xno"; then
  plugin_debug="yes"
  if test "$CLANG" = "yes"; then
    CFLAGS="$CFLAGS -O0 -g"
    CXXFLAGS="$CXXFLAGS -O0 -g"
  elif test "$GCC" = "yes"; then
    CFLAGS="$CFLAGS -O0 -g3"
    CXXFLAGS="$CXXFLAGS -O0 -g3"
  fi
fi
AC_SUBST(plugin_debug)

m4_include(macros/sylplugin_factory.m4)

# Checks for libraries.
AM_PATH_GLIB_2_0(2.8.0,, AC_MSG_ERROR(Test for GLib failed. See the 'INSTALL' for help.))
AM_PATH_GTK_2_0(2.8.0,, AC_MSG_ERROR(Test for Gtk failed. See the 'INSTALL' for help.))
GETTEXT_PACKAGE=autoforward
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Define text domain.])
AM_GLIB_GNU_GETTEXT

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 res/version.rc])
AC_OUTPUT
