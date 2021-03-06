dnl  Copyright 2005 Red Hat, Inc.
dnl 
dnl  Permission to use, copy, modify, distribute, and sell this software and its
dnl  documentation for any purpose is hereby granted without fee, provided that
dnl  the above copyright notice appear in all copies and that both that
dnl  copyright notice and this permission notice appear in supporting
dnl  documentation, and that the name of Red Hat not be used in
dnl  advertising or publicity pertaining to distribution of the software without
dnl  specific, written prior permission.  Red Hat makes no
dnl  representations about the suitability of this software for any purpose.  It
dnl  is provided "as is" without express or implied warranty.
dnl 
dnl  RED HAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
dnl  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
dnl  EVENT SHALL RED HAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
dnl  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
dnl  DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
dnl  TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
dnl  PERFORMANCE OF THIS SOFTWARE.
dnl
dnl Process this file with autoconf to create configure.

# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([xlogo], [1.0.5],
        [https://gitlab.freedesktop.org/xorg/app/xlogo/issues], [xlogo])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign dist-bzip2])

# Require X.Org macros 1.8 or later for MAN_SUBSTS set by XORG_MANPAGE_SECTIONS
m4_ifndef([XORG_MACROS_VERSION],
          [m4_fatal([must install xorg-macros 1.8 or later before running autoconf/autogen])])
XORG_MACROS_VERSION(1.8)
XORG_DEFAULT_OPTIONS

# Base set of required pkg-config packages
XLOGO_DEPS="sm xaw7 xmu xt >= 1.0 xext x11"

# Use render & xft2 to draw logo?

AC_ARG_WITH(render, 
	AS_HELP_STRING([--with-xrender], [Use Xrender & Xft2 to draw logo]), 
	USE_RENDER="$withval", USE_RENDER="try")

if test "x$USE_RENDER" = "xtry" ; then
	PKG_CHECK_EXISTS([xrender xft], USE_RENDER="yes", USE_RENDER="no")
fi

if test "x$USE_RENDER" = "xyes" ; then
	XLOGO_DEPS="xft xrender $XLOGO_DEPS"

	AC_DEFINE(XRENDER, 1, [Define to 1 to use Xrender & Xft2 to draw logo])
fi
AM_CONDITIONAL([USE_RENDER], [test "x$USE_RENDER" = "xyes"])

PKG_CHECK_MODULES(XLOGO, $XLOGO_DEPS)

PKG_CHECK_MODULES(APPDEFS, xt)
xt_appdefaultdir=`$PKG_CONFIG --variable=appdefaultdir xt`
AC_ARG_WITH(appdefaultdir,
	AS_HELP_STRING([--with-appdefaultdir=<pathname>],
	  [specify directory for app-defaults files (default is autodetected)]),
	[appdefaultdir="$withval"], [appdefaultdir="${xt_appdefaultdir}"])
AC_SUBST(appdefaultdir)

AC_CONFIG_FILES([
	Makefile
	man/Makefile])
AC_OUTPUT
