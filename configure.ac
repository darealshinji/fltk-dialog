m4_include([ax_check_compile_flag.m4])
m4_include([ax_cxx_compile_stdcxx.m4])

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


## check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_CHECK_PROG([XXD], [xxd], [yes])
AS_IF([test "x$XXD" != "xyes"], [exit 1])
AC_CHECK_PROG([PKG_CONFIG], [pkg-config], [yes])
AS_IF([test "x$PKG_CONFIG" != "xyes"], [exit 1])
AC_CHECK_PROG([CMAKE], [cmake], [yes])
AS_IF([test "x$CMAKE" != "xyes"], [exit 1])


## additional configure options
AC_ARG_ENABLE([hardening],
  [AS_HELP_STRING([--enable-hardening], [use hardening build flags])],
  [enable_hardening="yes"], [enable_hardening="no"])
AC_ARG_ENABLE([small],
  [AS_HELP_STRING([--enable-small], [optimize for small size])],
  [enable_small="yes"], [enable_small="no"])
AC_ARG_ENABLE([cxx11],
  [AS_HELP_STRING([--enable-cxx11], [use C++11 even if C++14 is available])],
  [enable_cxx11="yes"], [enable_cxx11="no"])

AC_ARG_ENABLE([calendar],
  [AS_HELP_STRING([--disable-calendar], [disable calendar dialog])],
  [use_calendar="no"], [use_calendar="yes"])
AC_ARG_ENABLE([checklist],
  [AS_HELP_STRING([--disable-checklist], [disable checklist dialog])],
  [use_checklist="no"], [use_checklist="yes"])
AC_ARG_ENABLE([color],
  [AS_HELP_STRING([--disable-color], [disable color dialog])],
  [use_color="no"], [use_color="yes"])
AC_ARG_ENABLE([date],
  [AS_HELP_STRING([--disable-date], [disable date dialog])],
  [use_date="no"], [use_date="yes"])
AC_ARG_ENABLE([dnd],
  [AS_HELP_STRING([--disable-dnd], [disable drag & drop dialog])],
  [use_dnd="no"], [use_dnd="yes"])
AC_ARG_ENABLE([dropdown],
  [AS_HELP_STRING([--disable-dropdown], [disable dropdown dialog])],
  [use_dropdown="no"], [use_dropdown="yes"])
AC_ARG_ENABLE([file],
  [AS_HELP_STRING([--disable-file], [disable file chooser dialog])],
  [use_file="no"], [use_file="yes"])
AC_ARG_ENABLE([native-file],
  [AS_HELP_STRING([--disable-native-file], [disable native file chooser dialog(s)])],
  [use_native_file="no"], [use_native_file="yes"])
AC_ARG_ENABLE([qt],
  [AS_HELP_STRING([--disable-qt], [disable Qt file chooser dialogs (implies --disable-qt4 and --disable-qt5)])],
  [use_qt="no"], [use_qt="yes"])
AC_ARG_ENABLE([qt4],
  [AS_HELP_STRING([--disable-qt4], [disable Qt4 file chooser dialog])],
  [use_qt4="no"], [use_qt4="yes"])
AC_ARG_ENABLE([qt5],
  [AS_HELP_STRING([--disable-qt5], [disable Qt5 file chooser dialog])],
  [use_qt5="no"], [use_qt5="yes"])
AC_ARG_ENABLE([font],
  [AS_HELP_STRING([--disable-font], [disable font selection dialog])],
  [use_font="no"], [use_font="yes"])
AC_ARG_ENABLE([html],
  [AS_HELP_STRING([--disable-html], [disable html dialog])],
  [use_html="no"], [use_html="yes"])
AC_ARG_ENABLE([notify],
  [AS_HELP_STRING([--disable-notify], [disable notification dialog])],
  [use_notify="no"], [use_notify="yes"])
AC_ARG_ENABLE([progress],
  [AS_HELP_STRING([--disable-progress], [disable progress dialog])],
  [use_progress="no"], [use_progress="yes"])
AC_ARG_ENABLE([radiolist],
  [AS_HELP_STRING([--disable-radiolist], [disable radiolist dialog])],
  [use_radiolist="no"], [use_radiolist="yes"])
AC_ARG_ENABLE([textinfo],
  [AS_HELP_STRING([--disable-textinfo], [disable textinfo dialog])],
  [use_textinfo="no"], [use_textinfo="yes"])

AC_ARG_ENABLE([l10n],
  [AS_HELP_STRING([--disable-l10n], [disable localization])],
  [use_l10n="no"], [use_l10n="yes"])
AC_ARG_ENABLE([window-icon],
  [AS_HELP_STRING([--disable-window-icon], [disable option to set the window icon])],
  [use_window_icon="no"], [use_window_icon="yes"])
AC_ARG_ENABLE([default-icon],
  [AS_HELP_STRING([--disable-default-icon], [disable embedded default window icon])],
  [default_icon="no"], [default_icon="yes"])
AC_ARG_ENABLE([dynamic-notify],
  [AS_HELP_STRING([--disable-dynamic-notify], [disable dynamically loading libnotify (link directly against shared or static library)])],
  [dynamic_notify="no"], [dynamic_notify="yes"])
AC_ARG_ENABLE([embedded-plugins],
  [AS_HELP_STRING([--disable-embedded-plugins], [don't embed Qt file chooser plugins into executable but load them from LIBDIR/fltk-dialog])],
  [use_embedded_plugins="no"], [use_embedded_plugins="yes"])

AC_ARG_ENABLE([system-jpeg],
  [AS_HELP_STRING([--enable-system-jpeg], [link against system libjpeg])],
  [use_system_jpeg="yes"], [use_system_jpeg="no"])
AC_ARG_ENABLE([system-png],
  [AS_HELP_STRING([--enable-system-png], [link against system libpng])],
  [use_system_png="yes"], [use_system_png="no"])
AC_ARG_ENABLE([system-zlib],
  [AS_HELP_STRING([--enable-system-zlib], [link against system zlib])],
  [use_system_zlib="yes"], [use_system_zlib="no"])


## check build flags
# getopt.hpp requires C++11 or higher
AS_IF([test "x$enable_cxx11" = "xyes"],
  [AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])],
  [AX_CXX_COMPILE_STDCXX([14], [noext], [optional])
   AS_IF([test "x$HAVE_CXX14" = "x0"],
         [AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])])
])
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
  # libdl
  have_dlopen="no"
  AS_IF([test "x$dynamic_notify" = "xyes"], [
    AC_CHECK_HEADER([dlfcn.h], [
      AC_CHECK_LIB([dl], [dlopen], [have_dlopen="yes"], [
        AC_CHECK_LIB([c], [dlopen], [have_dlopen="yes"])
      ])
    ])
  ])
  AS_IF([test "x$have_dlopen" = "xyes"], [dynamic_notify="yes"], [
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
    AS_IF([test "$xhave_libnotify" = "xyes"], [
      AC_CHECK_LIB([notify], [notify_init], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_is_initted], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_notification_new], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_notification_set_timeout], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_notification_show], [have_libnotify="yes"], [have_libnotify="no"])
      AC_CHECK_LIB([notify], [notify_uninit], [have_libnotify="yes"], [have_libnotify="no"])
    ])
  ])
])


## Qt checks
have_qt="no"
have_qt4="no"
have_qt5="no"
AS_IF([test "x$use_native_file" = "xyes" -a "x$use_file" = "xyes"], [
  AS_IF([test "x$use_qt" = "xyes"], [
    AS_IF([test "x$use_qt4" = "xyes"], [
      AX_PKG_CHECK_MODULES_SIMPLE([QtCore (Qt4)], [QtCore])
      AX_PKG_CHECK_MODULES_SIMPLE([QtGui (Qt4)],  [QtGui])
      AS_IF([test "x$ax_cv_pkg_check_modules_simple_QtCore" = "xyes" \
               -o "x$ax_cv_pkg_check_modules_simple_QtGui" = "xyes"],
            [have_qt="yes" have_qt4="yes"])
    ])
    AS_IF([test "x$use_qt5" = "xyes"], [
      AX_PKG_CHECK_MODULES_SIMPLE([Qt5Core],      [Qt5Core])
      AX_PKG_CHECK_MODULES_SIMPLE([Qt5Widgets],   [Qt5Widgets])
      AS_IF([test "x$ax_cv_pkg_check_modules_simple_Qt5Core" = "xyes" \
               -o "x$ax_cv_pkg_check_modules_simple_Qt5Widgets" = "xyes"],
            [have_qt="yes" have_qt5="yes"])
    ])
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
AS_IF([test "x$have_xft" != "xyes"], [exit 1])


## select(2) (used by READSTDIO macros)
AC_CHECK_HEADER([sys/time.h], [], [missing="sys/time.h"])
AC_CHECK_HEADER([sys/types.h], [], [missing="sys/types.h $missing"])
AC_CHECK_HEADER([unistd.h], [], [missing="unistd.h $missing"])
AC_CHECK_FUNC([select])
AS_IF([test "x$ac_cv_header_sys_time_h" != "xyes" \
         -o "x$ac_cv_header_sys_types_h" != "xyes" \
         -o "x$ac_cv_header_unistd_h" != "xyes" \
         -o "x$ac_cv_func_select" != "xyes"],
[
  AS_IF([test "x$use_progress" != "xno"], [
    AC_MSG_WARN([disabling progress dialog! headers missing: $missing])
    use_progress="no"
  ])
  AS_IF([test "x$use_textinfo" != "xno"], [
    AC_MSG_WARN([disabling textinfo dialog! headers missing: $missing])
    use_textinfo="no"
  ])
])


AS_IF([test "$prefix" = NONE], [prefix=/usr/local])
AS_IF([test "$exec_prefix" = NONE], [exec_prefix=/usr/local])


## substitute variables
AC_SUBST([PREFIX], [$prefix])
AC_SUBST([EPREFIX], [$exec_prefix])
AC_SUBST([LIBDIR], [$libdir])
AC_SUBST([SYSTEM_JPEG], [$use_system_jpeg])
AC_SUBST([SYSTEM_PNG], [$use_system_png])
AC_SUBST([SYSTEM_ZLIB], [$use_system_zlib])
AC_SUBST([HAVE_QT], [$have_qt])
AC_SUBST([HAVE_QT4], [$have_qt4])
AC_SUBST([HAVE_QT5], [$have_qt5])
AC_SUBST([WITH_L10N], [$use_l10n])
AC_SUBST([WITH_DEFAULT_ICON], [$default_icon])
AC_SUBST([WITH_CALENDAR], [$use_calendar])
AC_SUBST([WITH_CHECKLIST], [$use_checklist])
AC_SUBST([WITH_COLOR], [$use_color])
AC_SUBST([WITH_DATE], [$use_date])
AC_SUBST([WITH_DND], [$use_dnd])
AC_SUBST([WITH_DROPDOWN], [$use_dropdown])
AC_SUBST([WITH_FILE], [$use_file])
AC_SUBST([WITH_NATIVE_FILE], [$use_native_file])
AC_SUBST([WITH_FONT], [$use_font])
AC_SUBST([WITH_HTML], [$use_html])
AC_SUBST([WITH_NOTIFY], [$have_libnotify])
AC_SUBST([WITH_PROGRESS], [$use_progress])
AC_SUBST([WITH_RADIOLIST], [$use_radiolist])
AC_SUBST([WITH_TEXTINFO], [$use_textinfo])
AC_SUBST([WITH_WINDOW_ICON], [$use_window_icon])
AC_SUBST([DYNAMIC_NOTIFY], [$dynamic_notify])
AC_SUBST([EMBEDDED_PLUGINS], [$use_embedded_plugins])


AC_CONFIG_FILES([config.mak])
AC_OUTPUT


AC_MSG_RESULT([
$(cat config.mak)
])

