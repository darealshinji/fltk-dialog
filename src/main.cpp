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

#include <iostream>  /* std::cout, std::cerr, std::endl */
#include <string>    /* std::string */
#include <getopt.h>  /* getopt_long_only */
#include <stdlib.h>  /* exit */

#include "fltk-dialog.hpp"
#include "main.hpp"
#ifdef WITH_DEFAULT_ICON
#  include "icon.xpm"
#endif


const char *title = NULL;
const char *msg = NULL;
const char *dropdown_return_number = NULL;

/* don't use fltk's '@' symbols */
static int use_symbols = 0;

/* global FLTK callback for drawing all label text */
void draw_cb(const Fl_Label *o, int x, int y, int w, int h, Fl_Align a)
{
  fl_font(o->font, o->size);
  fl_color((Fl_Color)o->color);
  fl_draw(o->value, x, y, w, h, a, o->image, use_symbols);
}

/* global FLTK callback for measuring all labels */
void measure_cb(const Fl_Label *o, int &w, int &h)
{
  fl_font(o->font, o->size);
  fl_measure(o->value, w, h, use_symbols);
}

static int esc_handler(int event)
{
  if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
  {
    return 1; /* ignore Escape key */
  }
  return 0;
}

int use_only_with(char *self, std::string a, std::string b)
{
  std::cerr << self << ": " << a << " can only be used with " << b << "\n"
    "See `" << self << " --help' for more information" << std::endl;
  return 1;
}

void print_usage(char *prog)
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
  "  --warning                  Display warning dialog\n"
  "  --question                 Display question dialog\n"
#ifdef WITH_DND
  "  --dnd                      Display drag-n-drop box\n"
#endif
#ifdef WITH_FILE
  "  --file                     Display file selection dialog\n"
  "  --directory                Display directory selection dialog\n"
#endif
#ifdef WITH_ENTRY
  "  --entry                    Display text entry dialog\n"
#endif
#ifdef WITH_PASSWORD
  "  --password                 Display password dialog\n"
#endif
#ifdef WITH_PROGRESS
  "  --progress                 Display progress indication dialog\n"
#endif
#ifdef WITH_CALENDAR
  "  --calendar                 Display calendar dialog; returns date as Y-M-D\n"
#endif
#ifdef WITH_COLOR
  "  --color                    Display color selection dialog\n"
#endif
#ifdef WITH_SCALE
  "  --scale                    Display scale dialog\n"
#endif
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
#ifdef WITH_WINDOW_ICON
  "  --window-icon=FILE         Set the window icon; supported are: bmp gif\n"
  "                             jpg png pnm xbm xpm"
#endif
  "  --no-escape                Don't close window on hitting ESC button\n"
  "  --scheme=NAME              Set the window scheme to use: default, gtk+,\n"
  "                             gleam, plastic or simple; default is gtk+\n"
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

#if defined(WITH_RADIOLIST) && !defined(WITH_DROPDOWN)
  "\n"
  "Radiolist options:\n"
#elif !defined(WITH_RADIOLIST) && defined(WITH_DROPDOWN)
  "\n"
  "Dropdown options:\n"
#elif defined(WITH_RADIOLIST) && defined(WITH_DROPDOWN)
  "\n"
  "Radiolist/dropdown options:\n"
#endif

#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
  "  --return-number            Return selected entry number instead of label text\n"
#endif

#ifdef WITH_CALENDAR
  "\n"
  "Calendar options:\n"
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

#ifdef WITH_SCALE
  "\n"
  "Scale options:\n"
  "  --value=VALUE              Set initial value\n"
  "  --min-value=VALUE          Set minimum value\n"
  "  --max-value=VALUE          Set maximum value\n"
  "  --step=VALUE               Set step size\n"
  "                             VALUE can be float point or integer\n"
#endif

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
  "                             seconds (may be ignored by your notification daemon)\n"
  "  --notify-icon=PATH         Set the icon for the notification box\n"
#endif

    << std::endl;
}

int main(int argc, char **argv)
{
  const char *but_yes = NULL;
  const char *but_no = NULL;
  const char *but_alt = NULL;
  const char *scheme = "default";
  const char *scheme_default = "gtk+";
  int dialog = DIALOG_FL_MESSAGE;  /* default message type */

#ifdef WITH_SCALE
  char *minval = NULL;
  char *maxval = NULL;
  char *stepval = NULL;
  char *initval = NULL;
#endif

#ifdef WITH_CALENDAR
  std::string format = "";
#endif

#ifdef WITH_HTML
  const char *html = NULL;
#endif

#ifdef WITH_WINDOW_ICON
  std::string window_icon = "";
#endif

#ifdef WITH_NOTIFY
  const char *notify_timeout = NULL;
  std::string notify_icon = "";
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
  std::string checkbox = "";
  bool autoscroll = false;
#endif

  /* using these to check if two or
   * more dialog options were specified */
  int dabout = 0;
  int dalert = 0;
  int dcalendar = 0;
  int dchoice = 0;
  int ddirchoser = 0;
  int ddnd = 0;
  int ddropdown = 0;
  int dfilechooser = 0;
  int dhtml = 0;
  int dinput = 0;
  int dpassword = 0;
  int dcolor = 0;
  int dnotify = 0;
  int dprogress = 0;
  int dchecklist = 0;
  int dradiolist = 0;
  int dvalslider = 0;
  int dtextinfo = 0;

#ifdef WITH_DEFAULT_ICON
  /* set global default icon for all windows */
  Fl_Pixmap win_pixmap(icon_xpm);
  Fl_RGB_Image win_icon(&win_pixmap, Fl_Color(0));
  Fl_Window::default_icon(&win_icon);
#endif

  /* use a slightly brighter gray than the default one in FLTK */
  Fl::background(204, 204, 204);

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

#ifdef WITH_DND
    { "dnd",             no_argument,        0,  LO_DND             },
#endif

#ifdef WITH_HTML
    { "html",            required_argument,  0,  LO_HTML            },
#endif

    { "warning",         no_argument,        0,  LO_WARNING         },
    { "question",        no_argument,        0,  LO_QUESTION        },

#ifdef WITH_FILE
    { "file",            no_argument,        0,  LO_FILE            },
    { "directory",       no_argument,        0,  LO_DIRECTORY       },
    { "native",          no_argument,        0,  LO_NATIVE          },
#endif

#ifdef WITH_ENTRY
    { "entry",           no_argument,        0,  LO_ENTRY           },
#endif

#ifdef WITH_PASSWORD
    { "password",        no_argument,        0,  LO_PASSWORD        },
#endif

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

#ifdef WITH_SCALE
    { "scale",           no_argument,        0,  LO_SCALE           },
    { "value",           required_argument,  0,  LO_VALUE           },
    { "min-value",       required_argument,  0,  LO_MIN_VALUE       },
    { "max-value",       required_argument,  0,  LO_MAX_VALUE       },
    { "step",            required_argument,  0,  LO_STEP            },
#endif

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

#ifdef WITH_CALENDAR
    { "calendar",        no_argument,        0,  LO_CALENDAR        },
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
        dabout = 1;
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
        if (STREQ(optarg, ""))
        {
          but_yes = fl_yes;
        }
        else
        {
          but_yes = optarg;
        }
        break;
      case LO_NO_LABEL:
        if (!STREQ(optarg, ""))
        {
          but_no = fl_no;
        }
        else
        {
          but_no = optarg;
        }
        break;
      case LO_ALT_LABEL:
        if (!STREQ(optarg, ""))
        {
          but_alt = optarg;
        }
        break;
#ifdef WITH_DND
      case LO_DND:
        dialog = DIALOG_DND;
        ddnd = 1;
        break;
#endif
#ifdef WITH_HTML
      case LO_HTML:
        dialog = DIALOG_HTML;
        html = optarg;
        dhtml = 1;
        break;
#endif
      case LO_WARNING:
        dialog = DIALOG_ALERT;
        dalert = 1;
        break;
      case LO_QUESTION:
        dialog = DIALOG_FL_CHOICE;
        dchoice = 1;
        break;
#ifdef WITH_FILE
      case LO_FILE:
        dialog = DIALOG_FL_FILE_CHOOSER;
        dfilechooser = 1;
        break;
      case LO_DIRECTORY:
        dialog = DIALOG_FL_DIR_CHOOSER;
        ddirchoser = 1;
        break;
      case LO_NATIVE:
        native = true;
        break;
#endif
#ifdef WITH_ENTRY
      case LO_ENTRY:
        dialog = DIALOG_FL_INPUT;
        dinput = 1;
        break;
#endif
#ifdef WITH_PASSWORD
      case LO_PASSWORD:
        dialog = DIALOG_FL_PASSWORD;
        dpassword = 1;
        break;
#endif
#ifdef WITH_COLOR
      case LO_COLOR:
        dialog = DIALOG_FL_COLOR;
        dcolor = 1;
        break;
#endif
#ifdef WITH_NOTIFY
      case LO_NOTIFY:
        dialog = DIALOG_NOTIFY;
        dnotify = 1;
        break;
      case LO_TIMEOUT:
        notify_timeout = optarg;
        break;
      case LO_NOTIFY_ICON:
        notify_icon = std::string(optarg);
        break;
#endif
#ifdef WITH_PROGRESS
      case LO_PROGRESS:
        dialog = DIALOG_FL_PROGRESS;
        dprogress = 1;
        break;
      case LO_AUTO_CLOSE:
        autoclose = 1;
        break;
      case LO_NO_CANCEL:
        hide_cancel = true;
        break;
#endif
#ifdef WITH_SCALE
      case LO_SCALE:
        dialog = DIALOG_FL_VALUE_SLIDER;
        dvalslider = 1;
        break;
      case LO_VALUE:
        initval = optarg;
        break;
      case LO_MIN_VALUE:
        minval = optarg;
        break;
      case LO_MAX_VALUE:
        maxval = optarg;
        break;
      case LO_STEP:
        stepval = optarg;
        break;
#endif
#ifdef WITH_CHECKLIST
      case LO_CHECKLIST:
        dialog = DIALOG_FL_CHECK_BUTTON;
        checklist_options = std::string(optarg);
        dchecklist = 1;
        break;
#endif
#ifdef WITH_RADIOLIST
      case LO_RADIOLIST:
        dialog = DIALOG_FL_RADIO_ROUND_BUTTON;
        radiolist_options = std::string(optarg);
        dradiolist = 1;
        break;
#endif
#ifdef WITH_DROPDOWN
      case LO_DROPDOWN:
        dialog = DIALOG_DROPDOWN;
        dropdown_options = std::string(optarg);
        ddropdown = 1;
        break;
#endif
#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
      case LO_RETURN_NUMBER:
        return_number = true;
        dropdown_return_number = (char *)"y";
        break;
#endif
#ifdef WITH_CALENDAR
      case LO_CALENDAR:
        dialog = DIALOG_FL_CALENDAR;
        dcalendar = 1;
        break;
      case LO_FORMAT:
        format = std::string(optarg);
        break;
#endif
#ifdef WITH_TEXTINFO
      case LO_TEXT_INFO:
        dialog = DIALOG_TEXTINFO;
        dtextinfo = 1;
        break;
      case LO_AUTO_SCROLL:
        autoscroll = true;
        break;
      case LO_CHECKBOX:
        checkbox = std::string(optarg);
        break;
#endif
#ifdef WITH_WINDOW_ICON
      case LO_WINDOW_ICON:
        window_icon = std::string(optarg);
        break;
#endif
      default:
        std::cerr << "See `" << argv[0] << " --help' for available commands"
          << std::endl;
        return 1;
    }
  }

  if ((dabout + dalert + dcalendar + dchecklist + dchoice + ddirchoser + ddnd +
       ddropdown + dfilechooser + dhtml + dinput + dpassword + dcolor + dnotify +
       dprogress + dradiolist + dvalslider + dtextinfo) >= 2)
  {
    std::cerr << argv[0] << ": "
      << "two or more dialog options specified" << std::endl;
    return 1;
  }

  if (dialog != DIALOG_FL_CHOICE && (but_yes != NULL || but_no != NULL ||
                                     but_alt != NULL))
  {
    return use_only_with(argv[0], "--yes-label/--no-label/--alt-label", "--question");
  }

#ifdef WITH_FILE
  if (native && (dialog != DIALOG_FL_FILE_CHOOSER &&
                 dialog != DIALOG_FL_DIR_CHOOSER))
  {
    return use_only_with(argv[0], "--native", "--file or --directory");
  }
#endif

#ifdef WITH_NOTIFY
  if (notify_timeout != NULL && dialog != DIALOG_NOTIFY)
  {
    return use_only_with(argv[0], "--timeout", "--notification");
  }

  if (notify_icon != "" && dialog != DIALOG_NOTIFY)
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

#ifdef WITH_SCALE
  if (dialog != DIALOG_FL_VALUE_SLIDER && (minval != NULL || maxval != NULL ||
                                           stepval != NULL || initval != NULL))
  {
    return use_only_with(argv[0], "--value/--min-value/--max-value/--step", "--scale");
  }
#endif

#if defined(WITH_RADIOLIST) || defined(WITH_DROPDOWN)
  if (return_number && (dialog != DIALOG_FL_RADIO_ROUND_BUTTON &&
                        dialog != DIALOG_DROPDOWN))
  {
    return use_only_with(argv[0], "--return-number", "--radiolist or --dropdown");
  }
#endif

#ifdef WITH_CALENDAR
  if (format != "" && dialog != DIALOG_FL_CALENDAR)
  {
    return use_only_with(argv[0], "--format", "--calendar");
  }
#endif

#ifdef WITH_TEXTINFO
  if (dialog != DIALOG_TEXTINFO && (autoscroll == true || checkbox != ""))
  {
    return use_only_with(argv[0], "--auto-scroll/--checkbox", "--text-info");
  }
#endif

#ifdef WITH_HTML
  /* keep fltk's '@' symbols only enabled for HTML viewer */
  if (dialog != DIALOG_HTML)
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
      << "Available schemes are: default gtk+ gleam plastic simple"
      << std::endl;
    return 1;
  }

#ifdef WITH_WINDOW_ICON
  if (window_icon != "")
  {
    set_window_icon(window_icon);
  }
#endif

  switch (dialog)
  {
    case DIALOG_ABOUT:
      return about();
    case DIALOG_FL_MESSAGE:
      return dialog_fl_message(MESSAGE);
    case DIALOG_ALERT:
      return dialog_fl_message(ALERT);
    case DIALOG_FL_CHOICE:
      return dialog_fl_choice(but_yes, but_no, but_alt);

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

#ifdef WITH_ENTRY
    case DIALOG_FL_INPUT:
      return dialog_fl_input();
#endif

#ifdef WITH_HTML
    case DIALOG_HTML:
      return dialog_html_viewer(html);
#endif

#ifdef WITH_PASSWORD
    case DIALOG_FL_PASSWORD:
      return dialog_fl_password();
#endif

#ifdef WITH_COLOR
    case DIALOG_FL_COLOR:
      return dialog_fl_color();
#endif

#ifdef WITH_NOTIFY
    case DIALOG_NOTIFY:
      return dialog_notify(argv[0], notify_timeout, notify_icon);
#endif

#ifdef WITH_PROGRESS
    case DIALOG_FL_PROGRESS:
      return dialog_fl_progress(autoclose, hide_cancel);
#endif

#ifdef WITH_SCALE
    case DIALOG_FL_VALUE_SLIDER:
      return dialog_fl_value_slider(minval, maxval, stepval, initval);
#endif

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
      return dialog_dropdown(dropdown_options);
#endif

#ifdef WITH_CALENDAR
    case DIALOG_FL_CALENDAR:
      return dialog_fl_calendar(format);
#endif

#ifdef WITH_TEXTINFO
    case DIALOG_TEXTINFO:
      return dialog_textinfo(autoscroll, checkbox);
#endif
  }
}

