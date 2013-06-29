AC_INIT([libwebos-gui], [1.0.0], [morphis@gravedo.de], [libwebos-gui])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([dist-bzip2 subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CXX
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

PKG_PROG_PKG_CONFIG

GLIB_REQUIRED=2.18.0

PKG_CHECK_MODULES(GLIB,
  glib-2.0 >= $GLIB_REQUIRED
  gio-2.0 >= $GLIB_REQUIRED)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(LIBHYBRIS, hybris-egl-platform >= 0.1.0)
AC_SUBST(LIBHYBRIS_CFLAGS)
AC_SUBST(LIBHYBRIS_LIBS)

AC_ARG_ENABLE(debug,
  [  --enable-debug            Enable debug build (default=disabled)],
  [debug=$enableval],
  [debug="no"])
AM_CONDITIONAL( [WANT_DEBUG], [test x"$debug" = x"yes"])

AC_CONFIG_FILES([
    Makefile
    libwebos-gui/Makefile
    eglplatform-offscreen/Makefile
    webos-gui.pc
])

AC_OUTPUT
