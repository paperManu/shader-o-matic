#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.68)
AC_INIT(SHADEROMATIC, 0.0.9, emmanueldurand@gmail.com)
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE()

# Check for C and C++ compilers
AC_PROG_CXX
AC_PROG_CC

CXXFLAGS="$CXXFLAGS -std=c++11"

# Check for header files
AC_HEADER_STDC

# GLFW
PKG_CHECK_MODULES([GLFW], [glfw3 >= 3.0.3])
if test "x${have_glfw}" = "xfalse" ; then
    AC_MSG_ERROR([Missing glfw3])
fi

# OpenCV
PKG_CHECK_MODULES([OPENCV], [opencv >= 2.3])
if test "x${have_opencv}" = "xfalse" ; then
    AC_MSG_ERROR([Missing opencv])
fi

# OpenGL
PKG_CHECK_MODULES([GL], [gl >= 2.3])
if test "x${have_gl}" = "xfalse" ; then
    AC_MSG_ERROR([Missing opengl])
fi

# GLib
PKG_CHECK_MODULES([GLIB], glib-2.0, have_glib=true, have_glib=false)
if test "x${have_glib}" = "xfalse" ; then
    AC_MSG_ERROR([Missing glib])
fi

BOOST_REQUIRE([1.48])
BOOST_SYSTEM
BOOST_FILESYSTEM
BOOST_CHRONO

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    ])
AC_OUTPUT
