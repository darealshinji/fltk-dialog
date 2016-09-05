AC_INIT([fltk-dialog], [0.1],
        [https://github.com/darealshinji/fltk-dialog/issues])
AC_CONFIG_SRCDIR([src/main.cpp])


: ${CFLAGS="-Os"}
: ${CXXFLAGS="-Os"}


# check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_CHECK_PROG([CMAKE], [cmake], [cmake], [no])
AS_IF([test "x$CMAKE" = "xno"], [exit 1])
AC_CHECK_PROG([PKG_CONFIG], [pkg-config], [pkg-config], [no])
AS_IF([test "x$PKG_CONFIG" = "xno"], [exit 1])


# additional configure options
AC_ARG_ENABLE([calendar],
  [AS_HELP_STRING([--disable-calendar], [disable calendar dialog])],
  [use_calendar="no"], [use_calendar="yes"])
AC_ARG_ENABLE([color],
  [AS_HELP_STRING([--disable-color], [disable color dialog])],
  [use_color="no"], [use_color="yes"])
AC_ARG_ENABLE([dnd],
  [AS_HELP_STRING([--disable-dnd], [disable drag & drop dialog])],
  [use_dnd="no"], [use_dnd="yes"])
AC_ARG_ENABLE([entry],
  [AS_HELP_STRING([--disable-entry], [disable entry dialog])],
  [use_entry="no"], [use_entry="yes"])
AC_ARG_ENABLE([file],
  [AS_HELP_STRING([--disable-file], [disable file chooser dialog])],
  [use_file="no"], [use_file="yes"])
AC_ARG_ENABLE([html],
  [AS_HELP_STRING([--disable-html], [disable html dialog])],
  [use_html="no"], [use_html="yes"])
AC_ARG_ENABLE([notify],
  [AS_HELP_STRING([--disable-notify], [disable notification dialog])],
  [use_notify="no"], [use_notify="yes"])
AC_ARG_ENABLE([password],
  [AS_HELP_STRING([--disable-password], [disable password dialog])],
  [use_password="no"], [use_password="yes"])
AC_ARG_ENABLE([progress],
  [AS_HELP_STRING([--disable-progress], [disable progress dialog])],
  [use_progress="no"], [use_progress="yes"])
AC_ARG_ENABLE([scale],
  [AS_HELP_STRING([--disable-scale], [disable scale dialog])],
  [use_scale="no"], [use_scale="yes"])
AC_ARG_ENABLE([textinfo],
  [AS_HELP_STRING([--disable-textinfo], [disable textinfo dialog])],
  [use_textinfo="no"], [use_textinfo="yes"])

AC_ARG_ENABLE([window-icon],
  [AS_HELP_STRING([--disable-window-icon], [disable window icon select option])],
  [use_window_icon="no"], [use_window_icon="yes"])
AC_ARG_ENABLE([default-icon],
  [AS_HELP_STRING([--disable-default-icon], [disable embedded default window icon])],
  [default_icon="no"], [default_icon="yes"])
AC_ARG_ENABLE([dynamic-notify],
  [AS_HELP_STRING([--disable-dynamic-notify], [disable dynamically loading libnotify (link directly against shared or static library; less portable)])],
  [dynamic_notify="no"], [dynamic_notify="yes"])

AC_ARG_ENABLE([system-jpeg],
  [AS_HELP_STRING([--enable-system-jpeg], [link against system wide libjpeg])],
  [use_system_jpeg="yes"], [use_system_jpeg="no"])
AC_ARG_ENABLE([system-png],
  [AS_HELP_STRING([--enable-system-png], [link against system wide libpng])],
  [use_system_png="yes"], [use_system_png="no"])
AC_ARG_ENABLE([system-zlib],
  [AS_HELP_STRING([--enable-system-zlib], [link against system wide zlib])],
  [use_system_zlib="yes"], [use_system_zlib="no"])


# libnotify checks
have_libnotify="no"
AS_IF([test "x$use_notify" = "xno"], [
  dynamic_notify="no"
], [
  # libnotify headers
  CFLAGS_backup="$CFLAGS"
  CPPFLAGS_backup="$CPPFLAGS"
  CFLAGS="$CFLAGS $(pkg-config --cflags libnotify)"
  CPPFLAGS="$CPPFLAGS $(pkg-config --cflags libnotify)"
  AC_CHECK_HEADER([libnotify/notify.h], [have_libnotify="yes"], [have_libnotify="no"])
  CFLAGS="$CFLAGS_backup"
  CPPFLAGS="$CPPFLAGS_backup"
  # libnotify library functions
  AS_IF([test "$xhave_libnotify" = "xyes"], [
    AC_CHECK_LIB([notify], [notify_init], [have_libnotify="yes"], [have_libnotify="no"])
    AC_CHECK_LIB([notify], [notify_is_initted], [have_libnotify="yes"], [have_libnotify="no"])
    AC_CHECK_LIB([notify], [notify_notification_new], [have_libnotify="yes"], [have_libnotify="no"])
    AC_CHECK_LIB([notify], [notify_notification_set_timeout], [have_libnotify="yes"], [have_libnotify="no"])
    AC_CHECK_LIB([notify], [notify_notification_show], [have_libnotify="yes"], [have_libnotify="no"])
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
AS_IF([test "x$ac_cv_header_sys_time_h" != "xyes" \
         -o "x$ac_cv_header_sys_types_h" != "xyes" \
         -o "x$ac_cv_header_unistd_h" != "xyes"],
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
dnl AC_SUBST([STATIC_FLTK], [$static_fltk])
AC_SUBST([WITH_DEFAULT_ICON], [$default_icon])
AC_SUBST([WITH_CALENDAR], [$use_calendar])
AC_SUBST([WITH_COLOR], [$use_color])
AC_SUBST([WITH_DND], [$use_dnd])
AC_SUBST([WITH_ENTRY], [$use_entry])
AC_SUBST([WITH_FILE], [$use_file])
AC_SUBST([WITH_HTML], [$use_html])
AC_SUBST([WITH_NOTIFY], [$have_libnotify])
AC_SUBST([WITH_PASSWORD], [$use_password])
AC_SUBST([WITH_PROGRESS], [$use_progress])
AC_SUBST([WITH_SCALE], [$use_scale])
AC_SUBST([WITH_TEXTINFO], [$use_textinfo])
AC_SUBST([WITH_WINDOW_ICON], [$use_window_icon])
AC_SUBST([DYNAMIC_NOTIFY], [$dynamic_notify])
AC_SUBST([OPT], [])  # override value in Makefile with empty string


AC_CONFIG_FILES([config.mak])
AC_OUTPUT
