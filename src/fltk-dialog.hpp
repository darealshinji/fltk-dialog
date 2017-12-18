/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017, djcj <djcj@gmx.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef FLTK_DIALOG_HPP
#define FLTK_DIALOG_HPP

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <string>

#define STREQ(x, y)      (strcmp(x, y) == 0)
#define SSTREQ(x, y, n)  (strncmp(x, y, n) == 0)
#define MEMEQ(x, y, n)   (memcmp(x, y, n) == 0)

#define XSTRINGIFY(x)  STRINGIFY(x)
#define STRINGIFY(x)   #x

#ifdef HAVE_QT
# if defined(HAVE_QT5) && defined(HAVE_QT4)
#  define QTDEF 5
# else
#  if defined(HAVE_QT5) && !defined(HAVE_QT4)
#   define QTDEF 5
#  elif !defined(HAVE_QT5) && defined(HAVE_QT4)
#   define QTDEF 4
#  endif
# endif
# if (QTDEF == 4)
#  define QTGUI_SO      qt4gui_so
#  define QTGUI_SO_LEN  qt4gui_so_len
# else
#  define QTGUI_SO      qt5gui_so
#  define QTGUI_SO_LEN  qt5gui_so_len
# endif
#endif

#ifndef FLTK_DIALOG_MODULE_PATH
# define FLTK_DIALOG_MODULE_PATH "/usr/local/lib/fltk-dialog"
#endif

extern const char *title, *msg;
extern char separator;
extern std::string separator_s;
extern int ret;
extern bool resizable, position_center, window_decoration, window_taskbar, scale_val_set;
extern int override_x, override_y, override_w, override_h;
extern int win_w, win_h, max_w, max_h;
extern double scale_min, scale_max, scale_step, scale_init;

/* main.cpp */
void set_size(Fl_Double_Window *o, Fl_Widget *w);
void set_position(Fl_Double_Window *o);
void set_taskbar(Fl_Double_Window *o);
void set_undecorated(Fl_Double_Window *o);

/* about.cpp */
int about(void);

/* calendar.cpp */
#ifdef WITH_CALENDAR
int dialog_calendar(std::string format);
#endif

/* checklist.cpp */
#ifdef WITH_CHECKLIST
int dialog_checklist(std::string checklist_options,
                            bool return_value,
                            bool check_all);
#endif

/* color.cpp */
#ifdef WITH_COLOR
int dialog_color(void);
#endif

/* date.cpp */
#ifdef WITH_DATE
int dialog_date(std::string format);
#endif

/* dnd.cpp */
#ifdef WITH_DND
int dialog_dnd(void);
#endif

/* dropdown.cpp */
#ifdef WITH_DROPDOWN
int dialog_dropdown(std::string dropdown_list,
                           bool return_number);
#endif

/* file_dlopen_qtplugin.cpp */
#ifdef WITH_FILE
int dlopen_getfilenameqt(int qt_major, int mode, int argc, char **argv);
#endif

/* file.cpp */
#ifdef WITH_FILE
int dialog_file_chooser(void);
int dialog_dir_chooser(void);
#  define FILE_CHOOSER 0
#  define DIR_CHOOSER 1
#  ifdef WITH_NATIVE_FILE_CHOOSER
int dialog_native_file_chooser(int mode, int argc, char **argv);
int dialog_native_file_chooser_gtk(int mode);
#    ifdef HAVE_QT
int dialog_native_file_chooser_qt(int qt_major, int mode, int argc, char **argv);
#    endif  /* HAVE_QT */
#  endif  /* WITH_NATIVE_FILE_CHOOSER */
#endif  /* WITH_FILE */

/* font.cpp */
#ifdef WITH_FONT
int dialog_font(void);
#endif

/* html.cpp */
#ifdef WITH_HTML
int dialog_html_viewer(const char *file);
#endif

/* message.cpp */
void measure_button_width(Fl_Widget *o, int &w, int off);
enum {
  MESSAGE_TYPE_INFO,
  MESSAGE_TYPE_WARNING,
  MESSAGE_TYPE_QUESTION,
  MESSAGE_TYPE_INPUT,
  MESSAGE_TYPE_PASSWORD,
  MESSAGE_TYPE_SCALE
};
int dialog_message(
  const char *label_but_ret,
  const char *label_but,
  const char *label_but_alt = NULL,
  int type = MESSAGE_TYPE_WARNING,
  bool with_icon_box = true
);

/* notify.cpp */
#ifdef WITH_NOTIFY
int dialog_notify(const char *appname,
                          int timeout,
                  const char *notify_icon);
#endif

/* progress.cpp */
#ifdef WITH_PROGRESS
int dialog_progress(bool pulsate,
                     int multi,
                    long kill_pid,
                    bool autoclose,
                    bool hide_cancel);
#endif

/* textinfo.cpp */
#ifdef WITH_TEXTINFO
int dialog_textinfo(      bool  autoscroll,
                    const char *checkbox);
#endif

/* radiolist.cpp */
#ifdef WITH_RADIOLIST
int dialog_radiolist(std::string radiolist_options,
                            bool return_number);
#endif

/* window_icon.cpp */
#ifdef WITH_WINDOW_ICON
void set_window_icon(const char *file);
#ifdef WITH_RSVG
int rsvg_default_icon(const char *file);
#endif
#endif

/* l10n.cpp */
#ifdef WITH_L10N
void l10n(void);
#endif

/* misc/print_date.cpp */
void print_date(std::string format,
                        int y,
                        int m,
                        int d);

/* misc/translate.cpp */
void repstr(const std::string &from,
            const std::string &to,
                  std::string &s);
std::string translate(const char *inputText);

/* version.cpp */
std::string get_fltk_version(void);

#endif  /* !FLTK_DIALOG_HPP */

