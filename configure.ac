m4_include(m4/ax_pthread.m4)

# Define our symantic version major/minor/release values
# When you want to bump our version, do so by adjusting the
# Second argument in these macros
m4_define([ssmajor], [0])
m4_define([ssminor], [0])
m4_define([ssrelease], [1])

# Concatenate them into a string for AC_INIT
m4_define(ssversion, m4_join([],[v],ssmajor,[.],ssminor,[.],ssrelease))

AC_INIT([scifserver],[ssversion])

# Now export our version components as C defines
AC_DEFINE(VERSION_MAJOR, ssmajor, [Package Version Major value])
AC_DEFINE(VERSION_MINOR, ssminor, [Package Version Minor value])
AC_DEFINE(VERSION_RELEASE, ssrelease, [Package Version Release value])

AC_PREREQ([2.69])dnl
AC_CONFIG_HEADERS(ss-config.h,[],[])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB

AC_PROG_EGREP

#determine if we are in a git tree
AS_IF([git rev-parse --is-inside-work-tree >/dev/null 2>&1], [AC_DEFINE(IN_GIT_TREE, [ ], [Building within git tree])])

AX_PTHREAD([have_pthread=yes],[have_pthread=no])

AS_IF([test "x$have_pthread" = "xyes"],[
	AC_DEFINE(HAVE_PTHREAD,[],[We support pthreads])],[
	AC_MSG_ERROR([Missing pthread support])])

AC_CHECK_FUNC(getopt_long,[AC_DEFINE(HAVE_GETOPT_LONG,[],[Is the getopt_long function available])],[AC_MSG_NOTICE([Falling back to getopt])])

PKG_CHECK_MODULES([PROTOBUFC], [libprotobuf-c],
	[AC_DEFINE(HAVE_PROTOBUFC,[], [protobuf-c support available])],
	[AC_MSG_ERROR([ibprotobuf-c libraries missing])])

AC_CHECK_TOOL([PROTOC], [protoc], [], [])

AS_IF([test "x$enable_cunit" != "xno"], [
	AC_DEFINE(STATIC_DECL,[ ],[All functions public])
	AC_DEFINE(HAVE_CUNIT,[],[Building the test suite])
	PKG_CHECK_MODULES([CUNIT], [cunit], [], [AC_MSG_ERROR([cunit is required])])
	AC_CHECK_HEADERS(execinfo.h, [AC_DEFINE(HAVE_EXECINFO,[],[Enabling execinfo features])],[],[])
	],[
	AC_DEFINE(STATIC_DECL,[static],[All functions static])
	])
AM_CONDITIONAL([CUNIT], [test "x$enable_cunit" = "xyes"])

CFLAGS="-I\${top_srcdir} -I\${top_srcdir}/include -Wall -Werror"

AC_MSG_NOTICE()
AC_MSG_NOTICE([scifserver Version:     $VERSION])
AC_MSG_NOTICE([Target:                 $target])
AC_MSG_NOTICE([Installation prefix:    $prefix])
AC_MSG_NOTICE([Compiler:               $CC])
AC_MSG_NOTICE([Compiler flags: $CFLAGS])

AC_CONFIG_FILES([Makefile component/Makefile])
AC_OUTPUT
