
# AX_PKG_CHECK_MODULES_SIMPLE(prefix, pkg-module, [mandatory|optional])
m4_define([AX_PKG_CHECK_MODULES_SIMPLE], [{
  AC_MSG_CHECKING([for package $1])
  AS_IF([test "x$(pkg-config --exists $2; echo $?)" = "x0"], [
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
    AS_IF([test "x$3" = "xmandatory"], [exit 1])
  ])
}])


AC_INIT([fltk-dialog], [0.1], [https://github.com/darealshinji/fltk-dialog/issues])
AC_CONFIG_SRCDIR([src/main.cpp])


: ${CFLAGS="-Os"}
: ${CXXFLAGS="-Os"}


# check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_CHECK_PROG([CMAKE], [cmake], [yes])
AS_IF([test "x$CMAKE" != "xyes"], [exit 1])
AC_CHECK_PROG([PKG_CONFIG], [pkg-config], [yes])
AS_IF([test "x$PKG_CONFIG" != "xyes"], [exit 1])
AC_CHECK_PROG([SVN], [svn], [yes])
AS_IF([test "x$SVN" != "xyes"], [AC_MSG_WARN([\`svn' not in PATH! Cannot download missing source files])])
AC_CHECK_PROG([WGET], [wget], [yes])
AS_IF([test "x$WGET" != "xyes"], [AC_MSG_WARN([\`wget' not in PATH! Cannot download missing source files])])


# additional configure options
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

AC_ARG_ENABLE([system-jpeg],
  [AS_HELP_STRING([--enable-system-jpeg], [link against system libjpeg])],
  [use_system_jpeg="yes"], [use_system_jpeg="no"])
AC_ARG_ENABLE([system-png],
  [AS_HELP_STRING([--disable-system-png], [link against system libpng])],
  [use_system_png="yes"], [use_system_png="no"])
AC_ARG_ENABLE([system-zlib],
  [AS_HELP_STRING([--disable-system-zlib], [link against system zlib])],
  [use_system_zlib="yes"], [use_system_zlib="no"])


# libnotify checks
have_libnotify="no"
AS_IF([test "x$use_notify" = "xno"], [
  dynamic_notify="no"
], [
  # libnotify headers
    CFLAGS_backup="$CFLAGS"
    CPPFLAGS_backup="$CPPFLAGS"
  AS_IF([test "x$(pkg-config --exists libnotify; echo $?)" = "x0"], [
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
  # libdl
  have_dlopen="no"
  AS_IF([test "x$dynamic_notify" = "xyes"], [
    AC_CHECK_HEADER([dlfcn.h], [
      AC_CHECK_LIB([dl], [dlopen], [have_dlopen="yes"], [
        AC_CHECK_LIB([c], [dlopen], [have_dlopen="yes"])
      ])
    ])
  ])
  AS_IF([test "x$have_dlopen" = "xyes"], [dynamic_notify="yes"])
])


# Qt checks
AX_PKG_CHECK_MODULES_SIMPLE([QtCore (Qt4)], [QtCore],     [mandatory])
AX_PKG_CHECK_MODULES_SIMPLE([QtGui (Qt4)],  [QtGui],      [mandatory])
AX_PKG_CHECK_MODULES_SIMPLE([Qt5Core],      [Qt5Core],    [mandatory])
AX_PKG_CHECK_MODULES_SIMPLE([Qt5Widgets],   [Qt5Widgets], [mandatory])


# Xft
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
AS_IF([test "x$have_xft" != "xyes"], [
  AC_MSG_WARN([$PACKAGE_NAME will not be linked against Xft.
  Displayed fonts may look ugly!
  (ignore this warning if you are not on an X11 system)])
])


# select(2) (used by READSTDIO macros)
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


# substitute variables
AC_SUBST([SYSTEM_JPEG], [$use_system_jpeg])
AC_SUBST([SYSTEM_PNG], [$use_system_png])
AC_SUBST([SYSTEM_ZLIB], [$use_system_zlib])
AC_SUBST([WITH_L10N], [$use_l10n])
AC_SUBST([WITH_DEFAULT_ICON], [$default_icon])
AC_SUBST([WITH_CALENDAR], [$use_calendar])
AC_SUBST([WITH_CHECKLIST], [$use_checklist])
AC_SUBST([WITH_COLOR], [$use_color])
AC_SUBST([WITH_DATE], [$use_date])
AC_SUBST([WITH_DND], [$use_dnd])
AC_SUBST([WITH_DROPDOWN], [$use_dropdown])
AC_SUBST([WITH_FILE], [$use_file])
AC_SUBST([WITH_FONT], [$use_font])
AC_SUBST([WITH_HTML], [$use_html])
AC_SUBST([WITH_NOTIFY], [$have_libnotify])
AC_SUBST([WITH_PROGRESS], [$use_progress])
AC_SUBST([WITH_RADIOLIST], [$use_radiolist])
AC_SUBST([WITH_TEXTINFO], [$use_textinfo])
AC_SUBST([WITH_WINDOW_ICON], [$use_window_icon])
AC_SUBST([DYNAMIC_NOTIFY], [$dynamic_notify])
AC_SUBST([OPT], [])  # override value in Makefile with empty string


AC_CONFIG_FILES([config.mak])
AC_OUTPUT
