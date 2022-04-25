dnl
dnl
dnl This file is the part of AWP Image Processing Library V2 project 
dnl made by AWP Group 
dnl
dnl $Id: configure.ac,v 1.12 2004/01/16 11:46:19 jk Exp $
dnl
dnl

AC_INIT(fvlcore, 1.0)
AC_CONFIG_AUX_DIR(scripts)
AM_INIT_AUTOMAKE([foreign 1.7 dist-bzip2 dist-zip -Wall])
AM_CONFIG_HEADER(config.h)

dnl
dnl Check for programs
dnl
AC_PROG_CC

dnl
dnl Configuring libtool
dnl
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

dnl
dnl Checking for platform
dnl
AC_MSG_CHECKING([for native Win32 platform])
case "$host" in
    *-*-mingw*)
        HAVE_NATIVE_WIN32=yes
        ;;
    *)
        HAVE_NATIVE_WIN32=no
        ;;
esac
AC_MSG_RESULT([$HAVE_NATIVE_WIN32])
AM_CONDITIONAL([HAVE_NATIVE_WIN32], [test "$HAVE_NATIVE_WIN32" = "yes"])

dnl
dnl Check for math
dnl
if test "$HAVE_NATIVE_WIN32" != "yes"; then
    AC_CHECK_HEADER(math.h,
        # header found
        AC_CHECK_LIB(m,cos,
            # lib found
            M_LIBS="-lm"
            AC_SUBST(M_LIBS),
            # lib not found
            AC_MSG_ERROR(*** Math library not found! Can't continue.)
        )
    )
fi

dnl
dnl Enable doxygen docs
dnl

AC_ARG_ENABLE(doc,
        [  --enable-doc            generate documentation (default=yes)],,
	[enable_doc=yes])

AM_CONDITIONAL(ENABLE_DOC, [test x$enable_doc = xyes])
		    
if test x$enable_doc = xyes; then
        AC_PATH_PROG(DOXYGEN,doxygen)
        AC_PATH_PROG(DOT,dot)
					
        if test -z "$DOXYGEN" -o -z "$DOT"; then
        AC_MSG_ERROR([*** Can't find doxygen or dot! Don't know how to build documentation!])
        fi
fi

dnl
dnl Check for awpipl-v2, awpspml
dnl
export PKG_CONFIG_PATH=`eval echo $libdir`/pkgconfig:$PKG_CONFIG_PATH
PKG_CHECK_MODULES(AWPIPL2, awpipl2)

dnl
dnl After all tests set -no-undefined under the Win32
dnl
if test "$HAVE_NATIVE_WIN32" = "yes"; then
    LDFLAGS="$LDFLAGS -no-undefined"
    PC_FLAGS="-DHAVE_NATIVE_WIN32"
fi

AC_SUBST(PC_FLAGS)

AC_OUTPUT(
    Makefile
    fvlcore.pc
    src/Makefile
)

