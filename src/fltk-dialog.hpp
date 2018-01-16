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
extern char separator;
extern std::string separator_s;
extern int ret;
extern bool resizable, position_center, window_decoration, window_taskbar, scale_val_set, always_on_top;
extern int override_x, override_y, override_w, override_h;
extern int win_w, win_h, max_w, max_h;
extern double scale_min, scale_max, scale_step, scale_init;

void run_window(Fl_Double_Window *o, Fl_Widget *w);
void set_size(Fl_Double_Window *o, Fl_Widget *w);
void set_position(Fl_Double_Window *o);
void set_taskbar(Fl_Double_Window *o);  /* place before show() */
void set_undecorated(Fl_Double_Window *o);  /* place after show() */
void set_always_on_top(Fl_Double_Window *o);  /* place after show() */
void measure_button_width(Fl_Widget *o, int &w, int off);
void aspect_ratio_scale(int &w, int &h, const int limit);
void split(const std::string &s, char c, std::vector<std::string> &v);
void repstr(const std::string &from, const std::string &to, std::string &s);
std::string translate(const char *inputText);
std::string word_wrap(const char *text, int width, Fl_Font font, int font_size);
void print_date(std::string format, int y, int m, int d);
char *gunzip(const char *file, size_t limit);

int about(void);
int dialog_calendar(std::string format);
int dialog_checklist(std::string checklist_options, bool return_value, bool check_all);
int dialog_color(void);
int dialog_date(std::string format);
int dialog_dnd(void);
int dialog_dropdown(std::string dropdown_list, bool return_number);
int dialog_file_chooser(int file_mode, int native_mode, int argc, char **argv);
int dialog_font(void);
int dialog_html_viewer(const char *file);
int dialog_message(const char *label_but_ret, const char *label_but, const char *label_but_alt = NULL,
                   int type = MESSAGE_TYPE_WARNING, bool with_icon_box = true);
int dialog_notify(const char *appname, int timeout, const char *notify_icon, bool libnotify);
int dialog_progress(bool pulsate, int multi, long kill_pid, bool autoclose, bool hide_cancel);
int dialog_textinfo(bool autoscroll, const char *checkbox);
int dialog_radiolist(std::string radiolist_options, bool return_number);

Fl_RGB_Image *img_to_rgb(const char *file);
#ifdef WITH_RSVG
Fl_RGB_Image *rsvg_to_rgb(const char *file);
#endif
void l10n(void);
std::string get_fltk_version(void);

#endif  /* !FLTK_DIALOG_HPP */

