/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, djcj <djcj@gmx.de>
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
#ifdef WITH_DEFAULT_ICON
#  include <FL/Fl_Pixmap.H>
#  include <FL/Fl_RGB_Image.H>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <getopt.h>
#include <stdlib.h>

#include "fltk-dialog.hpp"
#include "main.hpp"
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


const char *title = NULL;
const char *msg = NULL;
int ret = 0;

/* get dimensions of the main screen work area */
int max_w = Fl::w();
int max_h = Fl::h();

int min_w = 60;
int min_h = 40;
int override_x = -1;
int override_y = -1;
int override_w = -1;
int override_h = -1;
bool resizable = true;
bool position_center = false;

double scale_min = 0;
double scale_max = 100;
double scale_step = 1;
double scale_init = 0;
double scale_value = 0;
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

void set_size(Fl_Window *o, Fl_Widget *w)
{
  if (resizable)
  {
    o->resizable(w);
  }

  if (override_w > 0 || override_h > 0)
  {
    o->size(override_w, override_h);
  }
}

void set_position(Fl_Window *o)
{
  if (position_center)
  {
    override_x = (max_w - o->w()) / 2;
    override_y = (max_h - o->h()) / 2;
  }

  if (override_x >= 0 && override_y >= 0)
  {
    o->position(override_x, override_y);
  }
}

#define ARGTOINT(a, b)  if (_argtoint(optarg, a, argv[0], b)) { return 1; }
#define STRINGTOINT(s, a, b)  if (_argtoint(s.c_str(), a, argv[0], b)) { return 1; }
static int _argtoint(const char *arg, int &val, char *self, std::string cmd)
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

#define ARGTODOUBLE(a, b)  if (_argtodouble(optarg, a, argv[0], b)) { return 1; }
static int _argtodouble(const char *arg, double &val, char *self, std::string cmd)
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

static int use_only_with(char *self, std::string a, std::string b)
{
  std::cerr << self << ": " << a << " can only be used with " << b << "\n"
    "See `" << self << " --help' for more information" << std::endl;
  return 1;
}

static void print_usage(char *prog)
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
#ifdef WITH_WINDOW_ICON
  "  --window-icon=FILE         Set the window icon; supported are: bmp gif\n"
  "                             jpg png pnm xbm xpm"
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
  "  --color                    Display color selection dialog\n"
#endif
  "  --scale                    Display scale dialog\n"
#ifdef WITH_CHECKLIST
  "  --checklist=OPT1[|..]      Display a check button list\n"
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

#ifdef WITH_FILE
  "\n"
  "File/directory selection options:\n"
  "  --native                   Use the operating system's native file\n"
  "                             chooser (GTK) if available, otherwise\n"
  "                             fall back to FLTK's own version\n"
#endif

#ifdef WITH_PROGRESS
  "\n"
  "Progress options:\n"
  "  --auto-close               Dismiss the dialog when 100% has been reached\n"
  "  --no-cancel                Hide cancel button\n"
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
  const char *but_alt = NULL;
  const char *scheme = "default";
  const char *scheme_default = "gtk+";
  int dialog = DIALOG_MESSAGE;  /* default message type */
  bool with_icon_box = true;

  std::string geometry;
  bool geometry_set = false;
  std::vector<std::string> v, v_wh;

#if defined(WITH_CALENDAR) || defined(WITH_DATE)
  std::string format = "";
#endif

#ifdef WITH_HTML
  const char *html = NULL;
#endif

#ifdef WITH_WINDOW_ICON
  const char *window_icon = NULL;
#endif

#ifdef WITH_NOTIFY
  int timeout = 5;
  bool timeout_set = false;
  const char *notify_icon = NULL;
#endif

#ifdef WITH_CHECKLIST
  std::string checklist_options = "";
#endif

#ifdef WITH_RADIOLIST
  std::string radiolist_options = "";
#endif

#ifdef WITH_DROPDOWN
  std::string dropdown_options = "";
#endif

#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
  bool return_number = false;
#endif

#ifdef WITH_FILE
  bool native = false;
#endif

#ifdef WITH_PROGRESS
  bool autoclose = false;
  bool hide_cancel = false;
#endif

#ifdef WITH_TEXTINFO
  const char *checkbox = NULL;
  bool autoscroll = false;
#endif

  /* used to check if two or more dialog
   * options were specified */
  int dialog_count = 0;

#ifdef WITH_DEFAULT_ICON
  /* set global default icon for all windows */
  Fl_Pixmap win_pixmap(icon_xpm);
  Fl_RGB_Image win_icon(&win_pixmap, Fl_Color(0));
  Fl_Window::default_icon(&win_icon);
#endif

  /* use a slightly brighter gray than the default one in FLTK */
  Fl::background(204, 204, 204);

  /* localize buttons */
#ifdef WITH_L10N
  l10n();
#endif

  /* run "About" dialog if invoked
   * without command line options */
  if (argc < 2)
  {
    /* disable fltk's '@' symbols */
    Fl::set_labeltype(FL_NORMAL_LABEL, draw_cb, measure_cb);
    Fl::scheme(scheme_default);
    return about();
  }

  for (int i = 1; i < argc; ++i)
  {
    if (STREQ("--help", argv[i]) || STREQ("-h", argv[i]))
    {
      print_usage(argv[0]);
      return 0;
    }
    else if (STREQ("--version", argv[i]) || STREQ("-v", argv[i]))
    {
      print_fltk_version();
      return 0;
    }
  }

  static struct option long_options[] =
  {
    { "about",           no_argument,        0,  LO_ABOUT           },
    { "no-escape",       no_argument,        0,  LO_NO_ESCAPE       },
    { "scheme",          required_argument,  0,  LO_SCHEME          },
    { "text",            required_argument,  0,  LO_TEXT            },
    { "title",           required_argument,  0,  LO_TITLE           },
    { "ok-label",        required_argument,  0,  LO_OK_LABEL        },
    { "close-label",     required_argument,  0,  LO_CLOSE_LABEL     },
    { "cancel-label",    required_argument,  0,  LO_CANCEL_LABEL    },
    { "yes-label",       required_argument,  0,  LO_YES_LABEL       },
    { "no-label",        required_argument,  0,  LO_NO_LABEL        },
    { "alt-label",       required_argument,  0,  LO_ALT_LABEL       },
    { "width",           required_argument,  0,  LO_WIDTH           },
    { "height",          required_argument,  0,  LO_HEIGHT          },
    { "posx",            required_argument,  0,  LO_POSX            },
    { "posy",            required_argument,  0,  LO_POSY            },
    { "geometry",        required_argument,  0,  LO_GEOMETRY        },
    { "fixed",           no_argument,        0,  LO_FIXED           },
    { "center",          no_argument,        0,  LO_CENTER          },

#ifdef WITH_DND
    { "dnd",             no_argument,        0,  LO_DND             },
#endif

#ifdef WITH_HTML
    { "html",            required_argument,  0,  LO_HTML            },
#endif

    { "message",         no_argument,        0,  LO_MESSAGE         },
    { "warning",         no_argument,        0,  LO_WARNING         },
    { "question",        no_argument,        0,  LO_QUESTION        },
    { "no-symbol",       no_argument,        0,  LO_NO_SYMBOL       },

#ifdef WITH_FILE
    { "file",            no_argument,        0,  LO_FILE            },
    { "directory",       no_argument,        0,  LO_DIRECTORY       },
    { "native",          no_argument,        0,  LO_NATIVE          },
#endif

    { "entry",           no_argument,        0,  LO_ENTRY           },
    { "password",        no_argument,        0,  LO_PASSWORD        },

#ifdef WITH_COLOR
    { "color",           no_argument,        0,  LO_COLOR           },
#endif

#ifdef WITH_NOTIFY
    { "notification",    no_argument,        0,  LO_NOTIFY          },
    { "timeout",         required_argument,  0,  LO_TIMEOUT         },
    { "notify-icon",     required_argument,  0,  LO_NOTIFY_ICON     },
#endif

#ifdef WITH_PROGRESS
    { "progress",        no_argument,        0,  LO_PROGRESS        },
    { "auto-close",      no_argument,        0,  LO_AUTO_CLOSE      },
    { "no-cancel",       no_argument,        0,  LO_NO_CANCEL       },
#endif

    { "scale",           no_argument,        0,  LO_SCALE           },
    { "value",           required_argument,  0,  LO_VALUE           },
    { "min-value",       required_argument,  0,  LO_MIN_VALUE       },
    { "max-value",       required_argument,  0,  LO_MAX_VALUE       },
    { "step",            required_argument,  0,  LO_STEP            },

#ifdef WITH_CHECKLIST
    { "checklist",       required_argument,  0,  LO_CHECKLIST       },
#endif

#ifdef WITH_RADIOLIST
    { "radiolist",       required_argument,  0,  LO_RADIOLIST       },
#endif

#ifdef WITH_DROPDOWN
    { "dropdown",        required_argument,  0,  LO_DROPDOWN        },
#endif

#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
    { "return-number",   no_argument,        0,  LO_RETURN_NUMBER   },
#endif

#ifdef WITH_DATE
    { "date",            no_argument,        0,  LO_DATE            },
#endif

#ifdef WITH_CALENDAR
    { "calendar",        no_argument,        0,  LO_CALENDAR        },
#endif

#ifdef WITH_FONT
    { "font",            no_argument,        0,  LO_FONT            },
#endif

#if defined(WITH_CALENDAR) || defined(WITH_DATE)
    { "format",          required_argument,  0,  LO_FORMAT          },
#endif

#ifdef WITH_TEXTINFO
    { "text-info",       no_argument,        0,  LO_TEXT_INFO       },
    { "auto-scroll",     no_argument,        0,  LO_AUTO_SCROLL     },
    { "checkbox",        required_argument,  0,  LO_CHECKBOX        },
#endif

#ifdef WITH_WINDOW_ICON
    { "window-icon",     required_argument,  0,  LO_WINDOW_ICON     },
#endif

    { 0, 0, 0, 0 }
  };

  int opt = -1;
  int long_index = 0;
  while ((opt = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1)
  {
    switch (opt)
    {
      case LO_ABOUT:
        dialog = DIALOG_ABOUT;
        dialog_count++;
        break;
      case LO_NO_ESCAPE:
        Fl::add_handler(esc_handler);
        break;
      case LO_SCHEME:
        scheme = optarg;
        break;
      case LO_TEXT:
        msg = optarg;
        break;
      case LO_TITLE:
        title = optarg;
        break;
      case LO_OK_LABEL:
        if (!STREQ(optarg, ""))
        {
          fl_ok = optarg;
        }
        break;
      case LO_CLOSE_LABEL:
        if (!STREQ(optarg, ""))
        {
          fl_close = optarg;
        }
        break;
      case LO_CANCEL_LABEL:
        if (!STREQ(optarg, ""))
        {
          fl_cancel = optarg;
        }
        break;
      case LO_YES_LABEL:
        if (!STREQ(optarg, ""))
        {
          fl_yes = optarg;
        }
        break;
      case LO_NO_LABEL:
        if (!STREQ(optarg, ""))
        {
          fl_no = optarg;
        }
        break;
      case LO_ALT_LABEL:
        if (!STREQ(optarg, ""))
        {
          but_alt = optarg;
        }
        break;
      case LO_WIDTH:
        ARGTOINT(override_w, "--width");
        break;
      case LO_HEIGHT:
        ARGTOINT(override_h, "--height");
        break;
      case LO_POSX:
        ARGTOINT(override_x, "--posx");
        break;
      case LO_POSY:
        ARGTOINT(override_y, "--posy");
        break;
      case LO_GEOMETRY:
        geometry = std::string(optarg);
        geometry_set = true;
        break;
      case LO_FIXED:
        resizable = false;
        break;
      case LO_CENTER:
        position_center = true;
        break;
#ifdef WITH_DND
      case LO_DND:
        dialog = DIALOG_DND;
        dialog_count++;
        break;
#endif
#ifdef WITH_HTML
      case LO_HTML:
        dialog = DIALOG_HTML;
        html = optarg;
        dialog_count++;
        break;
#endif
      case LO_MESSAGE:
        dialog = DIALOG_MESSAGE;
        dialog_count++;
        break;
      case LO_WARNING:
        dialog = DIALOG_WARNING;
        dialog_count++;
        break;
      case LO_QUESTION:
        dialog = DIALOG_QUESTION;
        dialog_count++;
        break;
      case LO_ENTRY:
        dialog = DIALOG_INPUT;
        dialog_count++;
        break;
      case LO_PASSWORD:
        dialog = DIALOG_PASSWORD;
        dialog_count++;
        break;
      case LO_NO_SYMBOL:
        with_icon_box = false;
        break;
#ifdef WITH_FILE
      case LO_FILE:
        dialog = DIALOG_FL_FILE_CHOOSER;
        dialog_count++;
        break;
      case LO_DIRECTORY:
        dialog = DIALOG_FL_DIR_CHOOSER;
        dialog_count++;
        break;
      case LO_NATIVE:
        native = true;
        break;
#endif
#ifdef WITH_COLOR
      case LO_COLOR:
        dialog = DIALOG_FL_COLOR;
        dialog_count++;
        break;
#endif
#ifdef WITH_NOTIFY
      case LO_NOTIFY:
        dialog = DIALOG_NOTIFY;
        dialog_count++;
        break;
      case LO_TIMEOUT:
        ARGTOINT(timeout, "--timeout");
        timeout_set = true;
        break;
      case LO_NOTIFY_ICON:
        notify_icon = optarg;
        break;
#endif
#ifdef WITH_PROGRESS
      case LO_PROGRESS:
        dialog = DIALOG_FL_PROGRESS;
        dialog_count++;
        break;
      case LO_AUTO_CLOSE:
        autoclose = true;
        break;
      case LO_NO_CANCEL:
        hide_cancel = true;
        break;
#endif
      case LO_SCALE:
        dialog = DIALOG_SCALE;
        dialog_count++;
        break;
      case LO_VALUE:
        ARGTODOUBLE(scale_init, "--value");
        scale_val_set = true;
        break;
      case LO_MIN_VALUE:
        ARGTODOUBLE(scale_min, "--min-value");
        scale_val_set = true;
        break;
      case LO_MAX_VALUE:
        ARGTODOUBLE(scale_max, "--max-value");
        scale_val_set = true;
        break;
      case LO_STEP:
        ARGTODOUBLE(scale_step, "--step");
        scale_val_set = true;
        break;
#ifdef WITH_CHECKLIST
      case LO_CHECKLIST:
        dialog = DIALOG_FL_CHECK_BUTTON;
        checklist_options = std::string(optarg);
        dialog_count++;
        break;
#endif
#ifdef WITH_RADIOLIST
      case LO_RADIOLIST:
        dialog = DIALOG_FL_RADIO_ROUND_BUTTON;
        radiolist_options = std::string(optarg);
        dialog_count++;
        break;
#endif
#ifdef WITH_DROPDOWN
      case LO_DROPDOWN:
        dialog = DIALOG_DROPDOWN;
        dropdown_options = std::string(optarg);
        dialog_count++;
        break;
#endif
#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
      case LO_RETURN_NUMBER:
        return_number = true;
        break;
#endif
#ifdef WITH_CALENDAR
      case LO_CALENDAR:
        dialog = DIALOG_FL_CALENDAR;
        dialog_count++;
        break;
#endif
#ifdef WITH_DATE
      case LO_DATE:
        dialog = DIALOG_FDATE;
        dialog_count++;
        break;
#endif
#if defined(WITH_CALENDAR) || defined(WITH_DATE)
      case LO_FORMAT:
        format = std::string(optarg);
        break;
#endif
#ifdef WITH_TEXTINFO
      case LO_TEXT_INFO:
        dialog = DIALOG_TEXTINFO;
        dialog_count++;
        break;
      case LO_AUTO_SCROLL:
        autoscroll = true;
        break;
      case LO_CHECKBOX:
        checkbox = optarg;
        break;
#endif
#ifdef WITH_FONT
      case LO_FONT:
        dialog = DIALOG_FONT;
        dialog_count++;
        break;
#endif
#ifdef WITH_WINDOW_ICON
      case LO_WINDOW_ICON:
        window_icon = optarg;
        break;
#endif
      default:
        std::cerr << "See `" << argv[0] << " --help' for available commands" << std::endl;
        return 1;
    }
  }

  if (dialog_count++ >= 2)
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

  if (geometry_set)
  {
    split(geometry, '+', v);
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

#ifdef WITH_FILE
  if (native && (dialog != DIALOG_FL_FILE_CHOOSER &&
                 dialog != DIALOG_FL_DIR_CHOOSER))
  {
    return use_only_with(argv[0], "--native", "--file or --directory");
  }
#endif

#ifdef WITH_NOTIFY
  if (timeout_set && dialog != DIALOG_NOTIFY)
  {
    return use_only_with(argv[0], "--timeout", "--notification");
  }

  if (notify_icon != NULL && dialog != DIALOG_NOTIFY)
  {
    return use_only_with(argv[0], "--notify-icon", "--notification");
  }
#endif

#ifdef WITH_PROGRESS
  if (autoclose && dialog != DIALOG_FL_PROGRESS)
  {
    return use_only_with(argv[0], "--auto-close", "--progress");
  }

  if (hide_cancel && dialog != DIALOG_FL_PROGRESS)
  {
    return use_only_with(argv[0], "--no-cancel", "--progress");
  }
#endif

  if (scale_val_set && dialog != DIALOG_SCALE)
  {
    return use_only_with(argv[0], "--value/--min-value/--max-value/--step", "--scale");
  }

#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
  if (return_number && (dialog != DIALOG_FL_RADIO_ROUND_BUTTON &&
                        dialog != DIALOG_DROPDOWN))
  {
    return use_only_with(argv[0], "--return-number", RADIOLIST_DROPDOWN_ARGS);
  }
#endif

#if defined(WITH_CALENDAR) || defined(WITH_DATE)
  if (format != "" && (dialog != DIALOG_FL_CALENDAR &&
                       dialog != DIALOG_FDATE))
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
  /* keep fltk's '@' symbols enabled for HTML and FDate dialogs */
  if (dialog != DIALOG_HTML && dialog != DIALOG_FDATE)
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
    std::cerr << "\"" << scheme << "\" is not a valid scheme!\n"
      << "Available schemes are: default gtk+ gleam plastic simple" << std::endl;
    return 1;
  }

#ifdef WITH_WINDOW_ICON
  if (window_icon != NULL)
  {
    set_window_icon(window_icon);
  }
#endif

  switch (dialog)
  {
    case DIALOG_ABOUT:
      return about();
    case DIALOG_MESSAGE:
      return dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO, with_icon_box);
    case DIALOG_ALERT:
      return dialog_message(fl_ok, fl_cancel, but_alt, MESSAGE_TYPE_WARNING, with_icon_box);
    case DIALOG_CHOICE:
      return dialog_message(fl_yes, fl_no, but_alt, MESSAGE_TYPE_QUESTION, with_icon_box);

#ifdef WITH_DND
    case DIALOG_DND:
      return dialog_dnd();
#endif

#ifdef WITH_FILE
    case DIALOG_FL_FILE_CHOOSER:
      if (native)
      {
        return dialog_fl_native_file_chooser(FILE_CHOOSER);
      }
      else
      {
        return dialog_fl_file_chooser();
      }
    case DIALOG_FL_DIR_CHOOSER:
      if (native)
      {
        return dialog_fl_native_file_chooser(DIR_CHOOSER);
      }
      else
      {
        return dialog_fl_dir_chooser();
      }
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
    case DIALOG_FL_COLOR:
      return dialog_fl_color();
#endif

#ifdef WITH_NOTIFY
    case DIALOG_NOTIFY:
      return dialog_notify(argv[0], timeout, notify_icon);
#endif

#ifdef WITH_PROGRESS
    case DIALOG_FL_PROGRESS:
      return dialog_fl_progress(autoclose, hide_cancel);
#endif

    case DIALOG_SCALE:
      return dialog_message(fl_ok, fl_cancel, but_alt, MESSAGE_TYPE_SCALE, false);

#ifdef WITH_CHECKLIST
    case DIALOG_FL_CHECK_BUTTON:
      return dialog_fl_check_button(checklist_options);
#endif

#ifdef WITH_RADIOLIST
    case DIALOG_FL_RADIO_ROUND_BUTTON:
      return dialog_fl_radio_round_button(radiolist_options, return_number);
#endif

#ifdef WITH_DROPDOWN
    case DIALOG_DROPDOWN:
      return dialog_dropdown(dropdown_options, return_number);
#endif

#ifdef WITH_CALENDAR
    case DIALOG_FL_CALENDAR:
      return dialog_fl_calendar(format);
#endif

#ifdef WITH_DATE
    case DIALOG_FDATE:
      return dialog_fdate(format);
#endif

#ifdef WITH_FONT
    case DIALOG_FONT:
      return dialog_font();
#endif

#ifdef WITH_TEXTINFO
    case DIALOG_TEXTINFO:
      return dialog_textinfo(autoscroll, checkbox);
#endif
  }
}

