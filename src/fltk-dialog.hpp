/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018, djcj <djcj@gmx.de>
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
#include <vector>

#define XSTRINGIFY(x)  STRINGIFY(x)
#define STRINGIFY(x)   #x

#define GETPROCADDRESS(handle,type,func,param) \
  typedef type (*func##_t) param; \
  func##_t func = (func##_t) dlsym(handle, STRINGIFY(func));

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

enum {
  MESSAGE_TYPE_INFO,
  MESSAGE_TYPE_WARNING,
  MESSAGE_TYPE_QUESTION,
  MESSAGE_TYPE_INPUT,
  MESSAGE_TYPE_PASSWORD,
  MESSAGE_TYPE_SCALE
};

enum {
  FILE_CHOOSER,
  DIR_CHOOSER
};

enum {
  NATIVE_NONE,
  NATIVE_ANY,
  NATIVE_GTK,
  NATIVE_QT4,
  NATIVE_QT5
};

extern const char *title, *msg;
extern bool resizable, position_center, window_taskbar, window_decoration, always_on_top;
extern int override_x, override_y, override_w, override_h;
extern int win_w, win_h, max_w, max_h;

void run_window(Fl_Double_Window *o, Fl_Widget *w);
void set_size(Fl_Double_Window *o, Fl_Widget *w);
void set_position(Fl_Double_Window *o);
void set_taskbar(Fl_Double_Window *o);  /* place before show() */
void set_undecorated(Fl_Double_Window *o);  /* place after show() */
void set_always_on_top(Fl_Double_Window *o);  /* place after show() */
int measure_button_width(const char *label, int extra_width = 0);
void aspect_ratio_scale(int &w, int &h, const int limit);
void split(const std::string &s, char c, std::vector<std::string> &v);
void repstr(const std::string &from, const std::string &to, std::string &s);
std::string translate(const char *text);
std::string text_wrap(const char *text, int width, Fl_Font font, int font_size);
std::string format_date(std::string format, int y, int m, int d);
size_t strlastcasecmp(const char *s1, const char *s2);

int dialog_message(int type = MESSAGE_TYPE_WARNING
,                  bool with_icon_box = true
,                  const char *label_but_alt = NULL
,                  double scale_min = 0
,                  double scale_max = 100
,                  double scale_step = 1
,                  double scale_init = 0);

int about(void);
int dialog_calendar(std::string format, bool arabic);
int dialog_checklist(std::string checklist_options, bool return_value, bool check_all, char separator);
int dialog_color(void);
int dialog_date(std::string format);
int dialog_dnd(void);
int dialog_dropdown(std::string dropdown_list, bool return_number, char separator);
int dialog_file_chooser(int file_mode, int native_mode, char separator);
int dialog_font(void);
int dialog_html_viewer(const char *file);
int dialog_notify(const char *appname, int timeout, const char *notify_icon, bool libnotify, bool force_nanosvg);
int dialog_progress(bool pulsate, unsigned int multi, long kill_pid, bool autoclose, bool hide_cancel);
int dialog_textinfo(bool autoscroll, const char *checkbox);
int dialog_radiolist(std::string radiolist_options, bool return_number, char separator);

Fl_RGB_Image *img_to_rgb(const char *file, bool force_nanosvg);
#ifdef WITH_RSVG
Fl_RGB_Image *rsvg_to_rgb(const char *file);
#endif
bool l10n(void);
std::string get_fltk_version(void);

#endif  /* !FLTK_DIALOG_HPP */

