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

#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wshadow"
# pragma GCC diagnostic ignored "-Wunused-parameter"
# if __GNUC__ > 7
#  pragma GCC diagnostic ignored "-Wcast-function-type"
# endif
#endif

#include <FL/Fl.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/x.H>

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Browser.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Image_Surface.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Single_Window.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Valuator.H>

#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_XBM_Image.H>
#include <FL/Fl_XPM_Image.H>

#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif

#include <string>
#include <vector>

#define XSTRINGIFY(x)  STRINGIFY(x)
#define STRINGIFY(x)   #x

#define PROTO(type,func,param) \
  typedef type (*func##_t) param; \
  func##_t func;

#define GETPROCADDRESS(handle,type,func,param) \
  PROTO(type,func,param) \
  func = reinterpret_cast<func##_t>(dlsym(handle, STRINGIFY(func)));

#define PROJECT_URL "https://github.com/darealshinji/fltk-dialog"

#ifndef FLTK_DIALOG_MODULE_PATH
# define FLTK_DIALOG_MODULE_PATH "/usr/local/lib/fltk-dialog"
#endif

#define FLTK_VERSION_STRING \
  XSTRINGIFY(FL_MAJOR_VERSION) "." XSTRINGIFY(FL_MINOR_VERSION) "." XSTRINGIFY(FL_PATCH_VERSION)

enum {
  DIALOG_ABOUT,
  DIALOG_CALENDAR,
  DIALOG_CHECKLIST,
  DIALOG_COLOR,
  DIALOG_DATE,
  DIALOG_DIR_CHOOSER,
  DIALOG_DND,
  DIALOG_DROPDOWN,
  DIALOG_FILE_CHOOSER,
  DIALOG_FONT,
  DIALOG_HTML,
  DIALOG_INDICATOR,
  DIALOG_INPUT,
  DIALOG_MESSAGE,
  DIALOG_NOTIFY,
  DIALOG_PASSWORD,
  DIALOG_PROGRESS,
  DIALOG_RADIOLIST,
  DIALOG_QUESTION,
  DIALOG_SCALE,
  DIALOG_TEXTINFO,
  DIALOG_WARNING
};

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
  NATIVE_QT
};

enum {
  INDICATOR_X11 = 1 << 1,
  INDICATOR_GTK = 1 << 2
};

enum {
  LANG_EN,
  LANG_AR,
  LANG_DE,
  LANG_ES,
  LANG_FR,
  LANG_IT,
  LANG_JA,
  LANG_PT,
  LANG_RU,
  LANG_ZH,
  LANG_ZH_TW,
  LANG_COUNT
};

extern const char *title, *msg, *quote;
extern bool resizable, position_center, window_taskbar, window_decoration, always_on_top, use_fribidi;
extern int override_x, override_y, override_w, override_h;
extern int max_w, max_h;

extern const char *weekdays[LANG_COUNT][7];
extern const char *month_names[LANG_COUNT][13];
extern Fl_Menu_Item item_month[13];
extern int selected_language;

extern const int days_in_month[2][13];
extern const int ordinal_day[2][13];

void run_window(Fl_Double_Window *o, Fl_Widget *w, int min_w, int min_h);
void set_size(Fl_Double_Window *o, Fl_Widget *w);
void set_size_range(Fl_Double_Window *o, int min_w, int min_h);
void set_position(Fl_Double_Window *o);
void set_taskbar(Fl_Double_Window *o);  /* place before show() */
void set_undecorated(Fl_Double_Window *o);  /* place after show() */
void set_always_on_top(Fl_Double_Window *o);  /* place after show() */
int measure_button_width(const char *label, int extra_width = 0);
void aspect_ratio_scale(int &w, int &h, const int limit);
void split(const std::string &s, char const c, std::vector<std::string> &v);
void repstr(const std::string &from, const std::string &to, std::string &s);
std::string translate(const char *text);
std::string text_wrap(const char *text, int width, Fl_Font font, int font_size);
char *format_date(const char *format, int y, int m, int d);
size_t strlastcasecmp(const char *s1, const char *s2);
std::string get_random(void);
bool save_to_temp(const unsigned char *data, const unsigned int data_len, const char *postfix, std::string &path);
int leap_year(int y);

#ifdef WITH_FRIBIDI
char *fribidi_parse_line(const char *input);
#endif

int dialog_message(int type = MESSAGE_TYPE_WARNING
,                  bool with_icon_box = true
,                  const char *label_but_alt = NULL
,                  double scale_min = 0
,                  double scale_max = 100
,                  double scale_step = 1
,                  double scale_init = 0);

int about(void);
int dialog_calendar(const char *format);
int dialog_checklist(std::string checklist_options, bool return_value, bool check_all, char separator);
int dialog_color(void);
int dialog_date(const char *format);
int dialog_dnd(void);
int dialog_dropdown(std::string dropdown_list, bool return_number, char separator);
int dialog_file_chooser(int mode, int native);
int dialog_font(void);
int dialog_html_viewer(const char *file);
int dialog_indicator(const char *command, const char *indicator_icon, int native, bool listen, bool auto_close);
int dialog_notify(const char *appname, int timeout, const char *notify_icon, bool libnotify);
int dialog_progress(bool pulsate, unsigned int multi, long kill_pid, bool autoclose, bool hide_cancel);
int dialog_textinfo(bool autoscroll, const char *checkbox, bool autoclose, bool hide_cancel);
int dialog_radiolist(std::string radiolist_options, bool return_number, char separator);

char *file_chooser(int mode);
Fl_RGB_Image *img_to_rgb(const char *file);
void l10n(void);

#endif  /* !FLTK_DIALOG_HPP */

