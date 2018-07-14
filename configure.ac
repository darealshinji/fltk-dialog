m4_include([ax_check_compile_flag.m4])

# AX_PKG_CHECK_MODULES_SIMPLE(prefix, pkg-module)
m4_define([AX_PKG_CHECK_MODULES_SIMPLE], [{
  AC_MSG_CHECKING([for package $1])
  AS_IF([test "x$(pkg-config --exists $2; echo $?)" = "x0"], [
    ax_cv_pkg_check_modules_simple_$2="yes"
    AC_MSG_RESULT([yes])
  ], [
    ax_cv_pkg_check_modules_simple_$2="no"
    AC_MSG_RESULT([no])
  ])
}])


AC_INIT([fltk-dialog], [0.1], [https://github.com/darealshinji/fltk-dialog/issues])
AC_CONFIG_SRCDIR([src/main.cpp])

: ${CFLAGS="-O3 -Wall"}
: ${CXXFLAGS="-O3 -Wall"}


## check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_CHECK_PROG([PKG_CONFIG], [pkg-config], [yes])
AS_IF([test "x$PKG_CONFIG" != "xyes"], [exit 1])
AC_CHECK_PROG([CMAKE], [cmake], [yes])
AS_IF([test "x$CMAKE" != "xyes"], [exit 1])
AC_CHECK_PROG([XXD], [xxd], [yes])
AS_IF([test "x$XXD" != "xyes"], [exit 1])
AC_CHECK_PROG([PATCH], [patch], [yes])
AS_IF([test "x$PATCH" != "xyes"], [exit 1])


## additional configure options
AC_ARG_ENABLE([hardening],
  [AS_HELP_STRING([--enable-hardening], [use hardening build flags])],
  [enable_hardening="yes"], [enable_hardening="no"])
AC_ARG_ENABLE([small],
  [AS_HELP_STRING([--enable-small], [optimize for small size])],
  [enable_small="yes"], [enable_small="no"])
AC_ARG_ENABLE([strip],
  [AS_HELP_STRING([--enable-strip], [strip binary])],
  [LDFLAGS="$LDFLAGS -s"], [])
AC_ARG_ENABLE([qt],
  [AS_HELP_STRING([--disable-qt], [disable Qt file chooser dialogs (implies --disable-qt4 and --disable-qt5)])],
  [use_qt="no"], [use_qt="yes"])
AC_ARG_ENABLE([qt4],
  [AS_HELP_STRING([--disable-qt4], [disable Qt4 file chooser dialog])],
  [use_qt4="no"], [use_qt4="yes"])
AC_ARG_ENABLE([qt5],
  [AS_HELP_STRING([--disable-qt5], [disable Qt5 file chooser dialog])],
  [use_qt5="no"], [use_qt5="yes"])
AC_ARG_ENABLE([dynamic-notify],
  [AS_HELP_STRING([--disable-dynamic-notify], [disable dynamically loading libnotify (link directly against shared or static library)])],
  [dynamic_notify="no"], [dynamic_notify="yes"])
AC_ARG_ENABLE([dynamic-uuid],
  [AS_HELP_STRING([--disable-dynamic-uuid], [disable dynamically loading libuuid (link directly against shared or static library)])],
  [dynamic_uuid="no"], [dynamic_uuid="yes"])
AC_ARG_ENABLE([rsvg],
  [AS_HELP_STRING([--enable-rsvg], [build rsvg plugin])],
  [use_rsvg="yes"], [use_rsvg="no"])
AC_ARG_ENABLE([embedded-plugins],
  [AS_HELP_STRING([--disable-embedded-plugins], [don't embed Qt file chooser plugins into executable but load them from LIBDIR/fltk-dialog])],
  [use_embedded_plugins="no"], [use_embedded_plugins="yes"])
AC_ARG_ENABLE([fribidi],
  [AS_HELP_STRING([--disable-fribidi], [disable fribidi support])],
  [use_fribidi="no"], [use_fribidi="yes"])
AC_ARG_ENABLE([system-fribidi],
  [AS_HELP_STRING([--enable-system-fribidi], [link against system fribidi])],
  [use_system_fribidi="yes" use_fribidi="yes"], [use_system_fribidi="no"])
AC_ARG_ENABLE([system-jpeg],
  [AS_HELP_STRING([--enable-system-jpeg], [link against system libjpeg])],
  [use_system_jpeg="yes"], [use_system_jpeg="no"])
AC_ARG_ENABLE([system-png],
  [AS_HELP_STRING([--enable-system-png], [link against system libpng])],
  [use_system_png="yes"], [use_system_png="no"])
AC_ARG_ENABLE([system-zlib],
  [AS_HELP_STRING([--disable-system-zlib], [don't link against system zlib])],
  [use_system_zlib="no"], [use_system_zlib="yes"])


## check build flags
AS_IF([test "x$enable_small" = "xyes"], [
  CFLAGS="$CFLAGS -g0 -Os -ffunction-sections -fdata-sections"
  CXXFLAGS="$CXXFLAGS -g0 -Os -ffunction-sections -fdata-sections"
  LDFLAGS="$LDFLAGS -s -Wl,--gc-sections"
])
AS_IF([test "x$enable_hardening" = "xyes"], [
  _sps="-fstack-protector-strong"
  _sp="-fstack-protector --param=ssp-buffer-size=4"
  AX_CHECK_COMPILE_FLAG([$_sps], [CFLAGS="$CFLAGS $_sps" CXXFLAGS="$CXXFLAGS $_sps"],
    [AX_CHECK_COMPILE_FLAG([$_sp], [CFLAGS="$CFLAGS $_sp" CXXFLAGS="$CXXFLAGS $_sp"], [])
  ])
  CFLAGS="$CFLAGS -D_FORTIFY_SOURCE=2"
  CXXFLAGS="$CXXFLAGS -D_FORTIFY_SOURCE=2"
  LDFLAGS="$LDFLAGS -Wl,-O1 -Wl,-z,defs -Wl,-z,relro -Wl,--as-needed"
])


## libnotify checks
have_libnotify="yes"
AS_IF([test "x$use_notify" = "xno"], [
  dynamic_notify="no"
  have_libnotify="no"
], [
  AS_IF([test "x$dynamic_notify" != "xyes"], [
    # libnotify headers
    AX_PKG_CHECK_MODULES_SIMPLE([libnotify], [libnotify])
    AS_IF([test "x$ax_cv_pkg_check_modules_simple_libnotify" = "xyes"], [
      CFLAGS_backup="$CFLAGS"
      CPPFLAGS_backup="$CPPFLAGS"
      CFLAGS="$CFLAGS $(pkg-config --cflags libnotify)"
      CPPFLAGS="$CPPFLAGS $(pkg-config --cflags libnotify)"
      AC_CHECK_HEADER([libnotify/notify.h], [have_libnotify="yes"], [have_libnotify="no"])
      CFLAGS="$CFLAGS_backup"
      CPPFLAGS="$CPPFLAGS_backup"
    ])
    # libnotify library functions
    AS_IF([test "x$have_libnotify" = "xyes"], [
      AC_CHECK_LIB([notify], [notify_init], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_is_initted], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_notification_new], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_notification_set_timeout], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_notification_show], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_uninit], [have_libnotify="yes"], [have_libnotify="no"])
    ])
    AS_IF([test "x$have_libnotify" != "xyes"], [
      AC_MSG_ERROR([re-run \`$0' without \`--disable-dynamic-notify'])
    ])
  ])
])


## libuuid checks
have_libuuid="yes"
AS_IF([test "x$use_libuuid" = "xno"], [
  dynamic_uuid="no"
  have_libuuid="no"
], [
  AS_IF([test "x$dynamic_uuid" != "xyes"], [
    # uuid headers
    AX_PKG_CHECK_MODULES_SIMPLE([uuid], [uuid])
    AS_IF([test "x$ax_cv_pkg_check_modules_simple_uuid" = "xyes"], [
      CFLAGS_backup="$CFLAGS"
      CPPFLAGS_backup="$CPPFLAGS"
      CFLAGS="$CFLAGS $(pkg-config --cflags uuid)"
      CPPFLAGS="$CPPFLAGS $(pkg-config --cflags uuid)"
      AC_CHECK_HEADER([uuid/uuid.h], [have_uuid="yes"], [have_uuid="no"])
      CFLAGS="$CFLAGS_backup"
      CPPFLAGS="$CPPFLAGS_backup"
    ])
    # uuid library functions
    AS_IF([test "x$have_uuid" = "xyes"], [
      AC_CHECK_LIB([uuid], [uuid_generate], [have_uuid="yes"], [have_uuid="no"])
    ])
    AS_IF([test "x$have_uuid" != "xyes"], [
      AC_MSG_ERROR([re-run \`$0' without \`--disable-dynamic-uuid'])
    ])
  ])
])


## fribidi checks
have_fribidi="yes"
AS_IF([test "x$use_fribidi" = "xyes" -a "x$use_system_fribidi" = "xyes"], [
  # fribidi headers
  AX_PKG_CHECK_MODULES_SIMPLE([fribidi], [fribidi])
  AS_IF([test "x$ax_cv_pkg_check_modules_simple_fribidi" = "xyes"], [
    CFLAGS_backup="$CFLAGS"
    CPPFLAGS_backup="$CPPFLAGS"
    CFLAGS="$CFLAGS $(pkg-config --cflags fribidi)"
    CPPFLAGS="$CPPFLAGS $(pkg-config --cflags fribidi)"
    AC_CHECK_HEADER([fribidi.h], [have_fribidi="yes"], [have_fribidi="no"])
    CFLAGS="$CFLAGS_backup"
    CPPFLAGS="$CPPFLAGS_backup"
  ])
  # fribidi library functions
  AS_IF([test "x$have_fribidi" = "xyes"], [
    AC_CHECK_LIB([fribidi], [fribidi_parse_charset], [have_fribidi="yes"], [have_fribidi="no"])
    AC_CHECK_LIB([fribidi], [fribidi_set_mirroring], [have_fribidi="yes"], [have_fribidi="no"])
    AC_CHECK_LIB([fribidi], [fribidi_set_reorder_nsm], [have_fribidi="yes"], [have_fribidi="no"])
    AC_CHECK_LIB([fribidi], [fribidi_charset_to_unicode], [have_fribidi="yes"], [have_fribidi="no"])
    AC_CHECK_LIB([fribidi], [fribidi_log2vis], [have_fribidi="yes"], [have_fribidi="no"])
    AC_CHECK_LIB([fribidi], [fribidi_remove_bidi_marks], [have_fribidi="yes"], [have_fribidi="no"])
    AC_CHECK_LIB([fribidi], [fribidi_unicode_to_charset], [have_fribidi="yes"], [have_fribidi="no"])
  ])
  AS_IF([test "x$have_fribidi" != "xyes"], [
    AC_MSG_ERROR([re-run \`$0' without \`--enable-system-fribidi'])
  ])
], [
  have_fribidi="no"
])


## Qt checks
have_qt="no"
have_qt4="no"
have_qt5="no"
AS_IF([test "x$use_qt" = "xyes"], [
  AS_IF([test "x$use_qt4" = "xyes"], [
    AX_PKG_CHECK_MODULES_SIMPLE([QtCore (Qt4)], [QtCore])
    AX_PKG_CHECK_MODULES_SIMPLE([QtGui (Qt4)], [QtGui])
    AS_IF([test "x$ax_cv_pkg_check_modules_simple_QtCore" = "xyes" \
             -o "x$ax_cv_pkg_check_modules_simple_QtGui" = "xyes"],
          [have_qt="yes" have_qt4="yes"])
  ])
  AS_IF([test "x$use_qt5" = "xyes"], [
    AX_PKG_CHECK_MODULES_SIMPLE([Qt5Core], [Qt5Core])
    AX_PKG_CHECK_MODULES_SIMPLE([Qt5Widgets], [Qt5Widgets])
    AS_IF([test "x$ax_cv_pkg_check_modules_simple_Qt5Core" = "xyes" \
             -o "x$ax_cv_pkg_check_modules_simple_Qt5Widgets" = "xyes"],
          [have_qt="yes" have_qt5="yes"])
  ])
])


## Xft
have_xft="no"
CFLAGS_backup="$CFLAGS"
CPPFLAGS_backup="$CPPFLAGS"
CFLAGS="$CFLAGS $(pkg-config --cflags freetype2)"
CPPFLAGS="$CPPFLAGS $(pkg-config --cflags freetype2)"
AC_CHECK_HEADER([X11/Xft/Xft.h], [
  AC_CHECK_LIB([Xft], [XftDrawCreate], [have_xft="yes"])
])
CFLAGS="$CFLAGS_backup"
CPPFLAGS="$CPPFLAGS_backup"
# force Xft because otherwise fonts will look very ugly
AS_IF([test "x$have_xft" != "xyes"], [exit 1])


AS_IF([test "$prefix" = NONE], [prefix=/usr/local])
AS_IF([test "$exec_prefix" = NONE], [exec_prefix=/usr/local])


## substitute variables
AC_SUBST([PREFIX], [$prefix])
AC_SUBST([EPREFIX], [$exec_prefix])
AC_SUBST([LIBDIR], [$libdir])
AC_SUBST([WITH_FRIBIDI], [$use_fribidi])
AC_SUBST([SYSTEM_FRIBIDI], [$use_system_fribidi])
AC_SUBST([SYSTEM_JPEG], [$use_system_jpeg])
AC_SUBST([SYSTEM_PNG], [$use_system_png])
AC_SUBST([SYSTEM_ZLIB], [$use_system_zlib])
AC_SUBST([HAVE_QT], [$have_qt])
AC_SUBST([HAVE_QT4], [$have_qt4])
AC_SUBST([HAVE_QT5], [$have_qt5])
AC_SUBST([WITH_RSVG], [$use_rsvg])
AC_SUBST([DYNAMIC_NOTIFY], [$dynamic_notify])
AC_SUBST([DYNAMIC_UUID], [$dynamic_uuid])
AC_SUBST([EMBEDDED_PLUGINS], [$use_embedded_plugins])


dnl Don't use AC_CONFIG_SUBDIRS
AS_IF([test "x$use_rsvg" = "xyes"], [
  AS_IF([test -e "librsvg/configure"], [
    for module in glib-2.0 gio-2.0 gdk-pixbuf-2.0 cairo pangocairo libxml-2.0 libcroco-0.6 ; do
      AC_MSG_CHECKING([for package $module])
      AS_IF([test "x$(pkg-config --exists $module; echo $?)" = "x0"], [
        AC_MSG_RESULT([yes])
      ], [
        AC_MSG_RESULT([no])
        exit 1
      ])
    done
    echo "=== configuring in librsvg ($PWD/librsvg)"
    cd librsvg || exit 1
    CFLAGS="$CFLAGS $CPPFLAGS" ./configure --disable-shared --disable-tools --disable-introspection --disable-pixbuf-loader --with-pic || exit 1
    cd -
  ], [
    AC_MSG_ERROR([cannot find \`librsvg/configure'; please add librsvg sources or re-run \`$0' without \`--enable-rsvg'])
  ])
])
AS_IF([test "x$use_fribidi" = "xyes" -a "x$use_system_fribidi" != "xyes"], [
  AS_IF([test -e "fribidi/configure"], [
    echo "=== configuring in fribidi ($PWD/fribidi)"
    cd fribidi || exit 1
    CFLAGS="$CFLAGS $CPPFLAGS" ./configure --disable-shared --disable-deprecated || exit 1
    cd -
  ], [
    AC_MSG_ERROR([cannot find \`fribidi/configure'; please add fribidi sources or re-run \`$0' with \`--disable-fribidi'])
  ])
])

AC_CONFIG_FILES([config.mak])
AC_OUTPUT

# run cmake
make fltk/build/Makefile || exit 1

AC_MSG_RESULT([
$(cat config.mak)
])

