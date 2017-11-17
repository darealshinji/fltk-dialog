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

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#ifdef WITH_DEFAULT_ICON
#  include <FL/Fl_Pixmap.H>
#  include <FL/Fl_RGB_Image.H>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>

#include "fltk-dialog.hpp"
#include "misc/getopt.hpp"
#include "misc/split.hpp"

#ifdef WITH_DEFAULT_ICON
#  include "icon.xpm"
#endif


#if defined(WITH_RADIOLIST) && !defined(WITH_DROPDOWN)

#  define RADIOLIST_DROPDOWN_OPTS "Radiolist"
#  define RADIOLIST_DROPDOWN_ARGS "--radiolist"

#elif !defined(WITH_RADIOLIST) && defined(WITH_DROPDOWN)

#  define RADIOLIST_DROPDOWN_OPTS "Dropdown"
#  define RADIOLIST_DROPDOWN_ARGS "--dropdown"

#elif defined(WITH_RADIOLIST) && defined(WITH_DROPDOWN)

#  define RADIOLIST_DROPDOWN_OPTS "Radiolist/dropdown"
#  define RADIOLIST_DROPDOWN_ARGS "--radiolist or --dropdown"

#endif


#if defined(WITH_CALENDAR) && !defined(WITH_DATE)

#  define CALENDAR_DATE_OPTS "Calendar"
#  define CALENDAR_DATE_ARGS "--calendar"

#elif !defined(WITH_CALENDAR) && defined(WITH_DATE)

#  define CALENDAR_DATE_OPTS "Date"
#  define CALENDAR_DATE_ARGS "--date"

#elif defined(WITH_CALENDAR) && defined(WITH_DATE)

#  define CALENDAR_DATE_OPTS "Calendar/date"
#  define CALENDAR_DATE_ARGS "--calendar or --date"

#endif


enum dialogTypes {
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

const char *title = NULL;
const char *msg = NULL;
int ret = 0;

char separator = '|';
std::string separator_s = "|";

/* get dimensions of the main screen work area */
int max_w = Fl::w();
int max_h = Fl::h();

int override_x = -1;
int override_y = -1;
int override_w = -1;
int override_h = -1;
bool resizable = true;
bool position_center = false;
bool window_taskbar = true;
bool window_decoration = true;

double scale_min = 0;
double scale_max = 100;
double scale_step = 1;
double scale_init = scale_min;
bool scale_val_set = false;

/* don't use fltk's '@' symbols */
#define USE_SYMBOLS 0

/* global FLTK callback for drawing all label text */
static void draw_cb(const Fl_Label *o, int x, int y, int w, int h, Fl_Align a)
{
  fl_font(o->font, o->size);
  fl_color((Fl_Color)o->color);
  fl_draw(o->value, x, y, w, h, a, o->image, USE_SYMBOLS);
}

/* global FLTK callback for measuring all labels */
static void measure_cb(const Fl_Label *o, int &w, int &h)
{
  fl_font(o->font, o->size);
  fl_measure(o->value, w, h, USE_SYMBOLS);
}

static int esc_handler(int event)
{
  if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
  {
    return 1; /* ignore Escape key */
  }
  return 0;
}

void set_size(Fl_Double_Window *o, Fl_Widget *w)
{
  if (resizable)
  {
    o->resizable(w);
  }

  if (override_w > 0)
  {
    o->size(override_w, o->h());
  }

  if (override_h > 0)
  {
    o->size(o->w(), override_h);
  }
}

void set_position(Fl_Double_Window *o)
{
  if (position_center)
  {
    override_x = (max_w - o->w()) / 2;
    override_y = (max_h - o->h()) / 2;
  }

  if (override_x >= 0)
  {
    o->position(override_x, o->y());
  }

  if (override_y >= 0)
  {
    o->position(o->x(), override_y);
  }
}

/* place before show() */
void set_taskbar(Fl_Double_Window *o)
{
  if (!window_taskbar)
  {
    o->border(0);
  }
}

/* place after show() */
void set_undecorated(Fl_Double_Window *o)
{
  if (window_decoration)
  {
    o->border(1);
  }
  else
  {
    o->border(0);
  }
}

#define ARGTOINT(a, b)  if (_argtoint(args[b].c_str(), a, argv[0], b)) { return 1; }
#define STRINGTOINT(s, a, b)  if (_argtoint(s.c_str(), a, argv[0], b)) { return 1; }
static int _argtoint(const char *arg, int &val, const char *self, std::string cmd)
{
  char *p;
  long l = strtol(arg, &p, 10);

  if (*p)
  {
    std::cerr << self << ": " << cmd << ": input is not an integer number" << std::endl;
    return 1;
  }
  val = (int) l;
  return 0;
}

#define ARGTODOUBLE(a, b)  if (_argtodouble(args[b].c_str(), a, argv[0], b)) { return 1; }
static int _argtodouble(const char *arg, double &val, const char *self, std::string cmd)
{
  char *p;
  val = strtod(arg, &p);

  if (*p)
  {
    std::cerr << self << ": " << cmd << ": input is not an integer or float point number" << std::endl;
    return 1;
  }
  return 0;
}

#define ARGTOLONG(a, b)  if (_argtolong(args[b].c_str(), a, argv[0], b)) { return 1; }
static int _argtolong(const char *arg, long &val, const char *self, std::string cmd)
{
  char *p;
  val = strtol(arg, &p, 10);

  if (*p)
  {
    std::cerr << self << ": " << cmd << ": input is not a long integer number" << std::endl;
    return 1;
  }
  return 0;
}

static int use_only_with(const char *self, std::string a, std::string b)
{
  std::cerr << self << ": " << a << " can only be used with " << b << "\n"
    "See `" << self << " --help' for more information" << std::endl;
  return 1;
}

static void print_usage(const char *prog)
{
  std::cout << "Usage:\n"
  "  " << prog << " OPTION [...]\n"
  "\n"
  "Options:\n"
  "  -h, --help                 Show help options\n"
  "  -v, --version              Show FLTK and program version\n"
  "  --about                    About FLTK dialog\n"
  "  --text=TEXT                Set the dialog text\n"
  "  --title=TITLE              Set the dialog title\n"
  "  --ok-label=TEXT            Set the OK button text\n"
  "  --cancel-label=TEXT        Set the CANCEL button text\n"
  "  --close-label=TEXT         Set the CLOSE button text\n"
  "  --separator=SEPARATOR      Set common separator character\n"
#ifdef WITH_WINDOW_ICON
  "  --window-icon=FILE         Set the window icon; supported are: bmp gif\n"
  "                             jpg png svg svgz xbm xpm\n"
#endif
  "  --width=WIDTH              Set the window width\n"
  "  --height=HEIGHT            Set the window height\n"
  "  --posx=NUMBER              Set the X position of a window\n"
  "  --posy=NUMBER              Set the Y position of a window\n"
  "  --geometry=WxH+X+Y         Set the window geometry\n"
  "  --fixed                    Set window unresizable\n"
  "  --center                   Place window on center of screen\n"
  "  --no-escape                Don't close window on hitting ESC button\n"
  "  --scheme=NAME              Set the window scheme to use: default, gtk+,\n"
  "                             gleam, plastic or simple; default is gtk+\n"
  "  --no-system-colors         Use FLTK's default gray color scheme\n"
  "  --undecorated              Set window undecorated (doesn't work on\n"
  "                             file/directory selection)\n"
  "  --skip-taskbar             Don't show window in taskbar\n"
  "  --message                  Display message dialog\n"
  "  --warning                  Display warning dialog\n"
  "  --question                 Display question dialog\n"
#ifdef WITH_DND
  "  --dnd                      Display drag-n-drop box\n"
#endif
#ifdef WITH_FILE
  "  --file                     Display file selection dialog\n"
  "  --directory                Display directory selection dialog\n"
#endif
  "  --entry                    Display text entry dialog\n"
  "  --password                 Display password dialog\n"
#ifdef WITH_PROGRESS
  "  --progress                 Display progress indication dialog\n"
#endif
#ifdef WITH_CALENDAR
  "  --calendar                 Display calendar dialog; returns date as Y-M-D\n"
#endif
#ifdef WITH_DATE
  "  --date                     Display date dialog; returns date as Y-M-D\n"
#endif
#ifdef WITH_COLOR
  "  --color                    Display color selection dialog; returns color\n"
  "                             as \"RGB [0.000-1.000]|RGB [0-255]|HTML hex|HSV\"\n"
#endif
  "  --scale                    Display scale dialog\n"
#ifdef WITH_CHECKLIST
  "  --checklist=OPT1|OPT2[|..] Display a check button list\n"
#endif
#ifdef WITH_RADIOLIST
  "  --radiolist=OPT1|OPT2[|..] Display a radio button list\n"
#endif
#ifdef WITH_DROPDOWN
  "  --dropdown=OPT1|OPT2[|..]  Display a dropdown menu\n"
#endif
#ifdef WITH_HTML
  "  --html=FILE                Display HTML viewer\n"
#endif
#ifdef WITH_TEXTINFO
  "  --text-info                Display text information dialog\n"
#endif
#ifdef WITH_NOTIFY
  "  --notification             Display notification\n"
#endif
#ifdef WITH_FONT
  "  --font                     Display font selection dialog\n"
#endif
  "\n"
  "Message/warning/question options:\n"
  "  --no-symbol                Don't show symbol box\n"
  "\n"
  "Question options:\n"
  "  --yes-label=TEXT           Sets the label of the Yes button\n"
  "  --no-label=TEXT            Sets the label of the No button\n"
  "  --alt-label=TEXT           Adds a third button and sets its label;\n"
  "                             exit code is 2\n"

#if defined(WITH_FILE) && defined(WITH_NATIVE_FILE_CHOOSER)
  "\n"
  "File/directory selection options:\n"
  "  --native                   Use the operating system's native file chooser if\n"
  "                             available, otherwise fall back to FLTK's own version\n"
#  ifdef HAVE_QT
  "  --native-gtk               Display the Gtk+ native file chooser\n"
#    ifdef HAVE_QT4
  "  --native-qt4               Display the Qt4 native file chooser\n"
#    endif
#    ifdef HAVE_QT5
  "  --native-qt5               Display the Qt5 native file chooser\n"
#    endif
  "  --native-qt                Alias for --native-qt" XSTRINGIFY(QTDEF) "\n"
#  endif
#endif

#ifdef WITH_PROGRESS
  "\n"
  "Progress options:\n"
  "  --pulsate                  Pulsate progress bar\n"
  "  --watch-pid=PID            Process ID to watch\n"
  "  --auto-close               Dismiss the dialog when 100% has been reached\n"
  "  --no-cancel                Hide cancel button\n"
#endif

#ifdef WITH_CHECKLIST
  "\n"
  "Checklist options:\n"
  "  --check-all                 Start with all items selected\n"
  "  --return-value              Return list of selected items instead of a \"TRUE|FALSE\" list\n"
#endif

#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
  "\n"
  RADIOLIST_DROPDOWN_OPTS " options:\n"
  "  --return-number            Return selected entry number instead of label text\n"
#endif

#if defined(WITH_CALENDAR) || defined(WITH_DATE)
  "\n"
  CALENDAR_DATE_OPTS " options:\n"
  "  --format=FORMAT            Set a custom output format\n"
  "                             Interpreted sequences for FORMAT are:\n"
  "                             (using the date 2006-01-08)\n"
  "                             d   day (8)\n"
  "                             D   day (08)\n"
  "                             m   month (1)\n"
  "                             M   month (01)\n"
  "                             y   year (06)\n"
  "                             Y   year (2006)\n"
  "                             j   day of the year (8)\n"
  "                             J   day of the year (008)\n"
  "                             W   weekday name (Sunday)\n"
  "                             w   weekday name (Sun)\n"
  "                             n   ISO 8601 week number (1)\n"
  "                             N   ISO 8601 week number (01)\n"
  "                             B   month name (January)\n"
  "                             b   month name (Jan)\n"
  "                             u   day of the week, Monday being 1 (7)\n"
  "                             \\n  newline character\n"
  "                             \\t  tab character\n"
#endif

  "\n"
  "Scale options:\n"
  "  --value=VALUE              Set initial value\n"
  "  --min-value=VALUE          Set minimum value\n"
  "  --max-value=VALUE          Set maximum value\n"
  "  --step=VALUE               Set step size\n"
  "                             VALUE can be float point or integer\n"

#ifdef WITH_TEXTINFO
  "\n"
  "Text information options:\n"
  "  --checkbox=TEXT            Enable an \"I read and agree\" checkbox\n"
  "  --auto-scroll              Always scroll to the bottom of the text\n"
#endif

#ifdef WITH_NOTIFY
  "\n"
  "Notification options:\n"
  "  --timout=SECONDS           Set the timeout value for the notification in\n"
  "                             seconds (may be ignored by some desktop environments)\n"
  "  --notify-icon=PATH         Set the icon for the notification box\n"
#endif

    << std::endl;
}

int main(int argc, char **argv)
{
#ifdef WITH_DEFAULT_ICON
  Fl_Pixmap win_pixmap(icon_xpm);
  Fl_RGB_Image win_icon(&win_pixmap, Fl_Color(0));
  Fl_Window::default_icon(&win_icon);
#endif

#ifdef WITH_L10N
  l10n();
#endif

  /* recommended in Fl_Double_Window.H */
  Fl::visual(FL_DOUBLE|FL_INDEX);

  const char *scheme_default = "gtk+";

  if (argc < 2)
  {
    Fl::set_labeltype(FL_NORMAL_LABEL, draw_cb, measure_cb); /* disable fltk's '@' symbols */
    Fl::scheme(scheme_default);
    Fl::get_system_colors();
    return about();
  }

  struct getopt args(argc, (const char **)argv);

  const char *args_noparam =
#ifdef WITH_DND
    " --dnd"
#endif
#ifdef WITH_FILE
    " --file"
    " --directory"
# ifdef WITH_NATIVE_FILE_CHOOSER
    " --native"
#  ifdef HAVE_QT
    " --native-gtk"
    " --native-qt"
#   ifdef HAVE_QT4
    " --native-qt4"
#   endif
#   ifdef HAVE_QT5
    " --native-qt5"
#   endif
#  endif
# endif
#endif
#ifdef WITH_COLOR
    " --color"
#endif
#ifdef WITH_NOTIFY
    " --notification"
    " --timeout"
    " --notify-icon"
#endif
#ifdef WITH_PROGRESS
    " --progress"
    " --pulsate"
    " --auto-close"
    " --no-cancel"
#endif
#ifdef WITH_CHECKLIST
    " --check-all"
    " --return-value"
#endif
#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
    " --return-number"
#endif
#ifdef WITH_CALENDAR
    " --calendar"
#endif
#ifdef WITH_DATE
    " --date"
#endif
#ifdef WITH_TEXTINFO
    " --text-info"
    " --auto-scroll"
#endif
#ifdef WITH_FONT
    " --font"
#endif
    " --help"
    " -h"
    " --version"
    " -v"
    " --about"
    " --no-escape"
    " --no-system-colors"
    " --undecorated"
    " --skip-taskbar"
    " --fixed"
    " --center"
    " --message"
    " --warning"
    " --question"
    " --entry"
    " --password"
    " --no-symbol"
    " --scale"
    " ";

  const char *args_param =
#ifdef WITH_HTML
    " --html"
#endif
#ifdef WITH_CHECKLIST
    " --checklist"
#endif
#ifdef WITH_RADIOLIST
    " --radiolist"
#endif
#ifdef WITH_DROPDOWN
    " --dropdown"
#endif
#ifdef WITH_PROGRESS
    " --watch-pid"
#endif
#if defined(WITH_CALENDAR) || defined(WITH_DATE)
    " --format"
#endif
#ifdef WITH_TEXTINFO
    " --checkbox"
#endif
#ifdef WITH_WINDOW_ICON
    " --window-icon"
#endif
    " --scheme"
    " --text"
    " --title"
    " --ok-label"
    " --close-label"
    " --cancel-label"
    " --yes-label"
    " --no-label"
    " --alt-label"
    " --width"
    " --height"
    " --posx"
    " --posy"
    " --geometry"
    " --value"
    " --min-value"
    " --max-value"
    " --step"
    " ";

  for (size_t i = 1; i < args.size(); ++i)
  {
    std::stringstream ss;
    std::vector<std::string> v;
    std::string arg, needle;
    bool has_parameter = false;

    ss << i;
    split(args[ss.str()], '=', v);

    if (v.size() > 1)
    {
      has_parameter = true;
      arg = v[0];
    }
    else
    {
      arg = args[ss.str()];
    }

    needle = " " + arg + " ";

    if (!strstr(args_noparam, needle.c_str()) && !strstr(args_param, needle.c_str()))
    {
      std::cerr << argv[0] << ": `" << arg << "' is NOT a valid command!\n"
        << "See `" << argv[0] << " --help' for available commands" << std::endl;
      return 1;
    }

    if (has_parameter && strstr(args_noparam, needle.c_str()))
    {
      std::cerr << argv[0] << ": `" << arg << "' doesn't take any arguments!\n"
        << "See `" << argv[0] << " --help' for more information" << std::endl;
      return 1;
    }

    if (strstr(args_param, needle.c_str()) && (!has_parameter || (has_parameter && v[1] == "")))
    {
      std::cerr << argv[0] << ": `" << arg << "' requires an argument!\n"
        << "See `" << argv[0] << " --help' for more information" << std::endl;
      return 1;
    }
  }

  if (args.has("-h") || args.has("--help") || args.size() == 1) {
    print_usage(argv[0]);
    return 0;
  }

  if (args.has("-v") || args.has("--version") || args.size() == 1) {
    std::cout << "using FLTK version " << get_fltk_version() << " - http://www.fltk.org" << std::endl;
    return 0;
  }

  int dialog_count = 0;  /* check if two or more dialog options were specified */
  int dialog = DIALOG_MESSAGE;  /* default message type */

  if (args.has("--about")) {
    dialog = DIALOG_ABOUT;
    dialog_count++;
  }

  if (args.has("--no-escape")) {
    Fl::add_handler(esc_handler);
  }

  const char *scheme = "default";
  if (args.has("--scheme")) {
    scheme = args["--scheme"].c_str();
  }

  bool system_colors = true;
  if (args.has("--no-system-colors")) {
    system_colors = false;
  }

  if (args.has("--undecorated")) {
    window_decoration = false;
  }

  if (args.has("--skip-taskbar")) {
    window_taskbar = false;
  }

  if (args.has("--text")) {
    msg = args["--text"].c_str();
  }

  if (args.has("--title")) {
    title = args["--title"].c_str();
  }

  if (args.has("--ok-label")) {
    fl_ok = args["--ok-label"].c_str();
  }

  if (args.has("--close-label")) {
    fl_close = args["--close-label"].c_str();
  }

  if (args.has("--cancel-label")) {
    fl_cancel = args["--cancel-label"].c_str();
  }

  if (args.has("--yes-label")) {
    fl_yes = args["--yes-label"].c_str();
  }

  if (args.has("--no-label")) {
    fl_no = args["--no-label"].c_str();
  }

  const char *but_alt = NULL;
  if (args.has("--alt-label")) {
    but_alt = args["--alt-label"].c_str();
  }

  if (args.has("--separator")) {
    separator_s = args["--separator"].substr(0,1);
    separator = separator_s.c_str()[0];
  }

  if (args.has("--width")) {
    ARGTOINT(override_w, "--width");
  }

  if (args.has("--height")) {
    ARGTOINT(override_h, "--height");
  }

  if (args.has("--posx")) {
    ARGTOINT(override_x, "--posx");
  }

  if (args.has("--posy")) {
    ARGTOINT(override_y, "--posy");
  }

  if (args.has("--geometry")) {
    std::vector<std::string> v, v_wh;
    split(args["--geometry"], '+', v);

    if (v.size() != 3)
    {
      std::cerr << argv[0] << ": --geometry=WxH+X+Y: wrong format" << std::endl;
      return 1;
    }

    split(v[0], 'x', v_wh);
    if (v_wh.size() != 2)
    {
      std::cerr << argv[0] << ": --geometry=WxH+X+Y: wrong format" << std::endl;
      return 1;
    }

    STRINGTOINT(v[1], override_x, "--geometry=WxH+X+Y -> X");
    STRINGTOINT(v[2], override_y, "--geometry=WxH+X+Y -> Y");
    STRINGTOINT(v_wh[0], override_w, "--geometry=WxH+X+Y -> W");
    STRINGTOINT(v_wh[1], override_h, "--geometry=WxH+X+Y -> H");
  }

  if (args.has("--fixed")) {
    resizable = false;
  }

  if (args.has("--center")) {
    position_center = true;
  }

#ifdef WITH_DND
  if (args.has("--dnd")) {
    dialog = DIALOG_DND;
    dialog_count++;
  }
#endif

#ifdef WITH_HTML
  const char *html = NULL;
  if (args.has("--html")) {
    dialog = DIALOG_HTML;
    html = args["--html"].c_str();
    dialog_count++;
  }
#endif

  if (args.has("--message")) {
    dialog = DIALOG_MESSAGE;
    dialog_count++;
  }

  if (args.has("--warning")) {
    dialog = DIALOG_WARNING;
    dialog_count++;
  }

  if (args.has("--question")) {
    dialog = DIALOG_QUESTION;
    dialog_count++;
  }

  if (args.has("--entry")) {
    dialog = DIALOG_INPUT;
    dialog_count++;
  }

  if (args.has("--password")) {
    dialog = DIALOG_PASSWORD;
    dialog_count++;
  }

  bool with_icon_box = true;
  if (args.has("--no-symbol")) {
    with_icon_box = false;
  }

#ifdef WITH_FILE
  if (args.has("--file")) {
    dialog = DIALOG_FILE_CHOOSER;
    dialog_count++;
  }

  if (args.has("--directory")) {
    dialog = DIALOG_DIR_CHOOSER;
    dialog_count++;
  }

# ifdef WITH_NATIVE_FILE_CHOOSER
  int native_count = 0;
  bool native = false;
  bool native_gtk = false;
  bool native_qt4 = false;
  bool native_qt5 = false;

  if (args.has("--native")) {
    native = true;
    native_count++;
  }

# ifdef HAVE_QT
  if (args.has("--native-gtk")) {
    native_gtk = true;
    native_count++;
  }

  if (args.has("--native-qt")) {
# if (QTDEF == 5)
    native_qt5 = true;
# else
    native_qt4 = true;
# endif
    native_count++;
  }

# ifdef HAVE_QT4
  if (args.has("--native-qt4")) {
    native_qt4 = true;
    native_count++;
  }
# endif

# ifdef HAVE_QT5
  if (args.has("--native-qt5")) {
    native_qt5 = true;
    native_count++;
  }
# endif

# endif  /* HAVE_QT */
# endif  /* WITH_NATIVE_FILE_CHOOSER */
#endif  /* WITH_FILE */

#ifdef WITH_COLOR
  if (args.has("--color")) {
    dialog = DIALOG_COLOR;
    dialog_count++;
  }
#endif

#ifdef WITH_NOTIFY
  int timeout = 5;
  bool timeout_set = false;
  const char *notify_icon = NULL;

  if (args.has("--notification")) {
    dialog = DIALOG_NOTIFY;
    dialog_count++;
  }

  if (args.has("--timeout")) {
    ARGTOINT(timeout, "--timeout");
    timeout_set = true;
  }

  if (args.has("--notify-icon")) {
    notify_icon = args["--notify-icon"].c_str();
  }
#endif

#ifdef WITH_PROGRESS
  long kill_pid = -1;
  bool pulsate = false;
  bool autoclose = false;
  bool hide_cancel = false;

  if (args.has("--progress")) {
    dialog = DIALOG_PROGRESS;
    dialog_count++;
  }

  if (args.has("--watch-pid")) {
    ARGTOLONG(kill_pid, "--watch-pid");
  }

  if (args.has("--pulsate")) {
    pulsate = true;
  }

  if (args.has("--auto-close")) {
    autoclose = true;
  }

  if (args.has("--no-cancel")) {
    hide_cancel = true;
  }
#endif

  if (args.has("--scale")) {
    dialog = DIALOG_SCALE;
    dialog_count++;
  }

  if (args.has("--min-value")) {
    ARGTODOUBLE(scale_min, "--min-value");
    scale_init = scale_min;
    scale_val_set = true;
  }

  if (args.has("--max-value")) {
    ARGTODOUBLE(scale_max, "--max-value");
    scale_val_set = true;
  }

  if (args.has("--value")) {
    ARGTODOUBLE(scale_init, "--value");
    scale_val_set = true;
  }

  if (args.has("--step")) {
    ARGTODOUBLE(scale_step, "--step");
    scale_val_set = true;

    if (scale_step < 0)
    {
      std::cerr << argv[0] << ": error `--step': value cannot be negative" << std::endl;
      return 1;
    }
    else if (scale_step == 0)
    {
      std::cerr << argv[0] << ": error `--step': value cannot be zero" << std::endl;
      return 1;
    }
  }

#ifdef WITH_CHECKLIST
  std::string checklist_options = "";
  bool check_all = false;
  bool return_value = false;

  if (args.has("--checklist")) {
    dialog = DIALOG_CHECKLIST;
    checklist_options = args["--checklist"];
    dialog_count++;
  }

  if (args.has("--check-all")) {
    check_all = true;
  }

  if (args.has("--return-value")) {
    return_value = true;
  }
#endif

#ifdef WITH_RADIOLIST
  std::string radiolist_options = "";

  if (args.has("--radiolist")) {
    dialog = DIALOG_RADIOLIST;
    radiolist_options = args["--radiolist"];
    dialog_count++;
  }
#endif

#ifdef WITH_DROPDOWN
  std::string dropdown_options = "";

  if (args.has("--dropdown")) {
    dialog = DIALOG_DROPDOWN;
    dropdown_options = args["--dropdown"];
    dialog_count++;
  }
#endif

#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
  bool return_number = false;

  if (args.has("--return-number")) {
    return_number = true;
  }
#endif

#ifdef WITH_CALENDAR
  if (args.has("--calendar")) {
    dialog = DIALOG_CALENDAR;
    dialog_count++;
  }
#endif

#ifdef WITH_DATE
  if (args.has("--date")) {
    dialog = DIALOG_DATE;
    dialog_count++;
  }
#endif

#if defined(WITH_CALENDAR) || defined(WITH_DATE)
  std::string format = "";
  if (args.has("--format")) {
    format = args["--format"];
  }
#endif

#ifdef WITH_TEXTINFO
  const char *checkbox = NULL;
  bool autoscroll = false;

  if (args.has("--text-info")) {
    dialog = DIALOG_TEXTINFO;
    dialog_count++;
  }

  if (args.has("--auto-scroll")) {
    autoscroll = true;
  }

  if (args.has("--checkbox")) {
    checkbox = args["--checkbox"].c_str();
  }
#endif

#ifdef WITH_FONT
  if (args.has("--font")) {
    dialog = DIALOG_FONT;
    dialog_count++;
  }
#endif

#ifdef WITH_WINDOW_ICON
  if (args.has("--window-icon")) {
    set_window_icon(args["--window-icon"].c_str());
  }
#endif

  if (dialog_count >= 2)
  {
    std::cerr << argv[0] << ": two or more dialog options specified" << std::endl;
    return 1;
  }

  if (!with_icon_box && (dialog != DIALOG_MESSAGE &&
                         dialog != DIALOG_WARNING &&
                         dialog != DIALOG_QUESTION))
  {
    return use_only_with(argv[0], "--no-symbol", "--message, --warning or --question");
  }

#if defined(WITH_FILE) && defined(WITH_NATIVE_FILE_CHOOSER)
  if ((native || native_gtk || native_qt4 || native_qt5) &&
      (dialog != DIALOG_FILE_CHOOSER && dialog != DIALOG_DIR_CHOOSER))
  {
    return use_only_with(argv[0], "--native/--native-gtk/--native-qt4/--native-qt5",
                         "--file or --directory");
  }

  if (native_count >= 2)
  {
    std::cerr << argv[0] << ": two or more `--native' options specified" << std::endl;
    return 1;
  }
#endif

#ifdef WITH_NOTIFY
  if (dialog != DIALOG_NOTIFY)
  {
    if (timeout_set)
    {
      return use_only_with(argv[0], "--timeout", "--notification");
    }

    if (notify_icon != NULL)
    {
      return use_only_with(argv[0], "--notify-icon", "--notification");
    }
  }
#endif

#ifdef WITH_PROGRESS
  if (dialog != DIALOG_PROGRESS)
  {
    if (pulsate)
    {
      return use_only_with(argv[0], "--pulsate", "--progress");
    }

    if (autoclose)
    {
      return use_only_with(argv[0], "--auto-close", "--progress");
    }

    if (hide_cancel)
    {
      return use_only_with(argv[0], "--no-cancel", "--progress");
    }
  }
#endif

  if (scale_val_set && dialog != DIALOG_SCALE)
  {
    return use_only_with(argv[0], "--value/--min-value/--max-value/--step", "--scale");
  }

#ifdef WITH_CHECKLIST
  if (return_value && dialog != DIALOG_CHECKLIST)
  {
    return use_only_with(argv[0], "--return-value", "--checklist");
  }

  if (check_all && dialog != DIALOG_CHECKLIST)
  {
    return use_only_with(argv[0], "--check-all", "--checklist");
  }
#endif

#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
  if (return_number && (dialog != DIALOG_RADIOLIST &&
                        dialog != DIALOG_DROPDOWN))
  {
    return use_only_with(argv[0], "--return-number", RADIOLIST_DROPDOWN_ARGS);
  }
#endif

#if defined(WITH_CALENDAR) || defined(WITH_DATE)
  if (format != "" && (dialog != DIALOG_CALENDAR &&
                       dialog != DIALOG_DATE))
  {
    return use_only_with(argv[0], "--format", CALENDAR_DATE_ARGS);
  }
#endif

#ifdef WITH_TEXTINFO
  if (dialog != DIALOG_TEXTINFO && (autoscroll == true || checkbox != NULL))
  {
    return use_only_with(argv[0], "--auto-scroll/--checkbox", "--text-info");
  }
#endif

#if defined(WITH_HTML) || defined(WITH_DATE)
  /* keep fltk's '@' symbols enabled for HTML and Date dialogs */
  if (dialog != DIALOG_HTML && dialog != DIALOG_DATE)
  {
    Fl::set_labeltype(FL_NORMAL_LABEL, draw_cb, measure_cb);
  }
#endif

  if (STREQ("gtk", scheme))
  {
    scheme = "gtk+";
  }
  else if (STREQ("simple", scheme))
  {
    scheme = "none";
  }

  if (STREQ("default", scheme))
  {
    Fl::scheme(scheme_default);
  }
  else if (STREQ("none", scheme) || STREQ("gtk+", scheme) ||
           STREQ("gleam", scheme) || STREQ("plastic", scheme))
  {
    Fl::scheme(scheme);
  }
  else
  {
    std::cerr << argv[0] << ": \"" << scheme << "\" is not a valid scheme!\n"
      << "Available schemes are: default gtk+ gleam plastic simple" << std::endl;
    return 1;
  }

  if (system_colors)
  {
    Fl::get_system_colors();
  }

  switch (dialog)
  {
    case DIALOG_ABOUT:
      return about();
    case DIALOG_MESSAGE:
      return dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO, with_icon_box);
    case DIALOG_WARNING:
      return dialog_message(fl_ok, fl_cancel, but_alt, MESSAGE_TYPE_WARNING, with_icon_box);
    case DIALOG_QUESTION:
      return dialog_message(fl_yes, fl_no, but_alt, MESSAGE_TYPE_QUESTION, with_icon_box);

#ifdef WITH_DND
    case DIALOG_DND:
      return dialog_dnd();
#endif

#ifdef WITH_FILE
    case DIALOG_FILE_CHOOSER:
#  ifdef WITH_NATIVE_FILE_CHOOSER
      if (native)
      {
        return dialog_native_file_chooser(FILE_CHOOSER, argc, argv);
      }
#    ifdef HAVE_QT
      else if (native_gtk)
      {
        return dialog_native_file_chooser_gtk(FILE_CHOOSER);
      }
#      ifdef HAVE_QT4
      else if (native_qt4)
      {
        return dialog_native_file_chooser_qt(4, FILE_CHOOSER, argc, argv);
      }
#      endif
#      ifdef HAVE_QT5
      else if (native_qt5)
      {
        return dialog_native_file_chooser_qt(5, FILE_CHOOSER, argc, argv);
      }
#      endif
#    endif  /* HAVE_QT */
      else
      {
        return dialog_file_chooser();
      }
#  else
      return dialog_file_chooser();
#  endif  /* WITH_NATIVE_FILE_CHOOSER */

    case DIALOG_DIR_CHOOSER:
#  ifdef WITH_NATIVE_FILE_CHOOSER
      if (native)
      {
        return dialog_native_file_chooser(DIR_CHOOSER, argc, argv);
      }
#    ifdef HAVE_QT
      else if (native_gtk)
      {
        return dialog_native_file_chooser_gtk(DIR_CHOOSER);
      }
#      ifdef HAVE_QT4
      else if (native_qt4)
      {
        return dialog_native_file_chooser_qt(4, DIR_CHOOSER, argc, argv);
      }
#      endif
#      ifdef HAVE_QT5
      else if (native_qt5)
      {
        return dialog_native_file_chooser_qt(5, DIR_CHOOSER, argc, argv);
      }
#      endif
#    endif  /* HAVE_QT */
      else
      {
        return dialog_dir_chooser();
      }
#  else
      return dialog_dir_chooser();
#  endif  /* WITH_NATIVE_FILE_CHOOSER */
#endif  /* WITH_FILE */

    case DIALOG_INPUT:
      return dialog_message(fl_ok, fl_cancel, but_alt, MESSAGE_TYPE_INPUT, false);

#ifdef WITH_HTML
    case DIALOG_HTML:
      return dialog_html_viewer(html);
#endif

    case DIALOG_PASSWORD:
      return dialog_message(fl_ok, fl_cancel, but_alt, MESSAGE_TYPE_PASSWORD, false);

#ifdef WITH_COLOR
    case DIALOG_COLOR:
      return dialog_color();
#endif

#ifdef WITH_NOTIFY
    case DIALOG_NOTIFY:
      return dialog_notify(argv[0], timeout, notify_icon);
#endif

#ifdef WITH_PROGRESS
    case DIALOG_PROGRESS:
      return dialog_progress(pulsate, kill_pid, autoclose, hide_cancel);
#endif

    case DIALOG_SCALE:
      return dialog_message(fl_ok, fl_cancel, but_alt, MESSAGE_TYPE_SCALE, false);

#ifdef WITH_CHECKLIST
    case DIALOG_CHECKLIST:
      return dialog_checklist(checklist_options, return_value, check_all);
#endif

#ifdef WITH_RADIOLIST
    case DIALOG_RADIOLIST:
      return dialog_radiolist(radiolist_options, return_number);
#endif

#ifdef WITH_DROPDOWN
    case DIALOG_DROPDOWN:
      return dialog_dropdown(dropdown_options, return_number);
#endif

#ifdef WITH_CALENDAR
    case DIALOG_CALENDAR:
      return dialog_calendar(format);
#endif

#ifdef WITH_DATE
    case DIALOG_DATE:
      return dialog_date(format);
#endif

#ifdef WITH_FONT
    case DIALOG_FONT:
      return dialog_font();
#endif

#ifdef WITH_TEXTINFO
    case DIALOG_TEXTINFO:
      return dialog_textinfo(autoscroll, checkbox);
#endif

    /* should never be reached */
    default:
      std::cerr << argv[0] << ":\nmain(): error: unknown or unused dialog" << std::endl;
      return 1;
  }

  /* should never be reached */
  return 1;
}

