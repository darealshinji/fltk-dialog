/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020, djcj <djcj@gmx.de>
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

/* include this first
 * source: https://github.com/Taywee/args */
#include "../args/args.hxx"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fltk-dialog.hpp"
#include "icon_png.h"

typedef args::Flag ARG_T;
typedef args::ValueFlag<int> ARGI_T;
typedef args::ValueFlag<long> ARGL_T;
typedef args::ValueFlag<float> ARGF_T;
typedef args::ValueFlag<double> ARGD_T;
typedef args::ValueFlag<std::string> ARGS_T;

#define GETCSTR(a,b)  if (b) { a = args::get(b).c_str(); }
#define GETVAL(a,b)   if (b) { a = args::get(b); }

const char *title = NULL;
const char *msg = NULL;
const char *quote = "";

int override_x = 0
,   override_y = 0
,   override_w = 0
,   override_h = 0
,   override_pos = 0;

bool resizable = true
,    window_taskbar = true
,    window_decoration = true;

static void draw_cb(const Fl_Label *o, int x, int y, int w, int h, Fl_Align a) {
  fl_font(o->font, o->size);
  fl_color(o->color);
  fl_draw(o->value, x, y, w, h, a, o->image, 0);
}

static void measure_cb(const Fl_Label *o, int &w, int &h) {
  fl_font(o->font, o->size);
  fl_measure(o->value, w, h, 0);
}

static int esc_handler(int event) {
  if (event == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
    return 1; /* ignore Escape key */
  }
  return 0;
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    Fl::scheme("gtk+");
    Fl::visual(FL_DOUBLE|FL_INDEX);
    Fl::set_color(FL_BACKGROUND_COLOR, fl_lighter(FL_BACKGROUND_COLOR));
    Fl_Window::default_icon(new Fl_PNG_Image(NULL, icon_png, icon_png_len));
    l10n();
    override_pos = 5;
    return about();
  }

  args::ArgumentParser ap_main("FLTK dialog - run dialog boxes from shell scripts", "");
  ap_main.Prog(argv[0]);

  args::Group ap(ap_main, "Generic options:");
  args::HelpFlag help(ap, "help", "Show options", {'h', "help"});
  ARG_T  arg_version(ap, "version", "Show FLTK and program version", {'v', "version"})
  ,      arg_about(ap, "about", "About FLTK dialog", {"about"});
  ARGS_T arg_text(ap, "TEXT", "Set the dialog text", {"text"})
  ,      arg_title(ap, "TEXT", "Set the dialog title", {"title"})
  ,      arg_ok_label(ap, "TEXT", "Set the OK button text", {"ok-label"})
  ,      arg_cancel_label(ap, "TEXT", "Set the CANCEL button text", {"cancel-label"})
  ,      arg_close_label(ap, "TEXT", "Set the CLOSE button text", {"close-label"})
  ,      arg_separator(ap, "SEPARATOR", "Set common separator (single character; can be escape sequence \\n or \\t)",
                       {"separator"})
  ,      arg_icon(ap, "FILE", "Set the taskbar/notification/indicator icon; supported formats are: "
                  "bmp gif "
#ifdef USE_DLOPEN
                  "icns "
#endif
                  "ico jpg png svg svgz xbm xpm", {"icon"});
  ARG_T  arg_quoted_output(ap, "quoted-output", "Quote output", {"quoted-output"});
  ARGI_T arg_width(ap, "WIDTH", "Set the window width", {"width"})
  ,      arg_height(ap, "HEIGHT", "Set the window height", {"height"})
  ,      arg_posx(ap, "NUMBER", "Set the X position of a window", {"posx"})
  ,      arg_posy(ap, "NUMBER", "Set the Y position of a window", {"posy"});
  ARGI_T arg_position(ap, "1..9", "Set the window position from 1 to 9, analog to the positions "
                          "of the number keys on the num pad", {"position"});
  ARGS_T arg_geometry(ap, "WxH+X+Y", "Set the window geometry", {"geometry"});
  ARG_T  arg_center(ap, "center", "Place window at center of screen", {"center"})
  ,      arg_fixed(ap, "fixed", "Set window unresizable", {"fixed"})
  ,      arg_always_on_top(ap, "always-on-top", "Keep window always visible on top", {"always-on-top"})
  ,      arg_no_escape(ap, "no-escape", "Don't close window when hitting ESC button", {"no-escape"});
  ARGS_T arg_scheme(ap, "NAME", "Set the window scheme to use: fltk, gtk+, gleam or plastic; "
                    "default is gtk+", {"scheme"});
  ARG_T  arg_system_colors(ap, "system-colors", "Use the system's color scheme", {"system-colors"})
  ,      arg_undecorated(ap, "undecorated", "Set window undecorated", {"undecorated"})
  ,      arg_skip_taskbar(ap, "skip-taskbar", "Don't show window in taskbar", {"skip-taskbar"})
  ,      arg_message(ap, "message", "Display message dialog", {"message"})
  ,      arg_warning(ap, "warning", "Display warning dialog", {"warning"})
  ,      arg_error(ap, "error", "Display error dialog", {"error"})
  ,      arg_question(ap, "question", "Display question dialog", {"question"})
  ,      arg_dnd(ap, "dnd", "Display drag-n-drop box", {"dnd"})
  ,      arg_file(ap, "file", "Display file selection dialog", {"file"})
  ,      arg_directory(ap, "directory", "Display directory selection dialog", {"directory"})
  ,      arg_entry(ap, "entry", "Display text entry dialog", {"entry"})
  ,      arg_password(ap, "password", "Display password dialog", {"password"})
  ,      arg_progress(ap, "progress", "Display progress indication dialog", {"progress"})
  ,      arg_calendar(ap, "calendar", "Display calendar dialog; returns date as Y-M-D", {"calendar"})
  ,      arg_date(ap, "date", "Display date selection dialog; returns date as Y-M-D", {"date"})
  ,      arg_color(ap, "color", "Display color selection dialog; returns color as \"RGB [0.000-1.000]|RGB "
                   "[0-255]|HTML hex|HSV\"", {"color"})
  ,      arg_scale(ap, "scale", "Display scale dialog", {"scale"});
  ARGS_T arg_checklist(ap, "OPT1|OPT2[|..]", "Display a check button list", {"checklist"})
  ,      arg_radiolist(ap, "OPT1|OPT2[|..]", "Display a radio button list", {"radiolist"})
  ,      arg_dropdown(ap, "OPT1|OPT2[|..]", "Display a dropdown menu", {"dropdown"})
  ,      arg_html(ap, "FILE", "Display HTML viewer", {"html"});
  ARG_T  arg_text_info(ap, "text-info", "Display text information dialog", {"text-info"})
  ,      arg_notification(ap, "notification", "Display a notification pop-up", {"notification"});
  ARGS_T arg_indicator(ap, "COMMAND", "create an indicator/tray entry as a launcher for a given command; "
                       "use --text to set a tooltip message", {"indicator"});
  ARG_T  arg_font(ap, "font", "Display font selection dialog", {"font"});

  args::Group g_mweq_options(ap_main, "Message/warning/error/question options:");
  ARG_T  arg_no_symbol(g_mweq_options, "no-symbol", "Don't show symbol box", {"no-symbol"});

  args::Group g_question_options(ap_main, "Question options:");
  ARGS_T arg_yes_label(g_question_options, "TEXT", "Sets the label of the Yes button", {"yes-label"})
  ,      arg_no_label(g_question_options, "TEXT", "Sets the label of the No button", {"no-label"})
  ,      arg_alt_label(g_question_options, "TEXT", "Adds a third button and sets its label; exit code is 2",
                       {"alt-label"});

  args::Group g_file_dir_options(ap_main, "File/directory selection options:");
  ARG_T arg_classic(g_file_dir_options, "classic", "Use the classic FLTK file/directory selection widget (some "
                    "options may not work)", {"classic"})
  ,     arg_no_devices(g_file_dir_options, "no-devices", "Don't use look for available devices", {"no-devices"});
#ifdef USE_DLOPEN
  ARG_T arg_native(g_file_dir_options, "native", "Use the operating system's native file chooser if available, "
                   "otherwise fall back to FLTK's own version; some options may only work on FLTK's file chooser",
                   {"native"});
#ifdef HAVE_QT
  ARG_T arg_native_gtk(g_file_dir_options, "native-gtk", "Display the Gtk+ native file chooser", {"native-gtk"});
  ARG_T arg_native_qt(g_file_dir_options, "native-qt", "Display the Qt native file chooser", {"native-qt"});
#endif
#endif  /* USE_DLOPEN */

  args::Group g_progress_options(ap_main, "Progress options:");
  ARG_T  arg_pulsate(g_progress_options, "pulsate", "Pulsating progress bar", {"pulsate"});
  ARGI_T arg_multi(g_progress_options, "NUMBER", "Use 2 progress bars; the main bar, showing the overall progress, "
                   "will reach 100% if the other bar has reached 100% after NUMBER iterations", {"multi"});
  ARGL_T arg_watch_pid(g_progress_options, "PID", "Process ID to watch", {"watch-pid"});

  args::Group g_progress_text_info_options(ap_main, "Progress/text information options:");
  ARG_T  arg_auto_close(g_progress_text_info_options, "auto-close", "Automatically close the dialog window",
                        {"auto-close"})
  ,      arg_no_cancel(g_progress_text_info_options, "no-cancel", "Hide cancel button", {"no-cancel"});

  args::Group g_checklist_options(ap_main, "Checklist options:");
  ARG_T  arg_check_all(g_checklist_options, "check-all", "Start with all items selected", {"check-all"})
  ,      arg_return_value(g_checklist_options, "return-value", "Return list of selected items instead of a "
                          "\"TRUE|FALSE\" list", {"return-value"});

  args::Group g_radiolist_dropdown_options(ap_main, "Radiolist/dropdown options:");
  ARG_T  arg_return_number(g_radiolist_dropdown_options, "return-number", "Return selected entry number instead of "
                           "label text", {"return-number"});

  args::Group g_calendar_options(ap_main, "Calendar/date options:");
  ARGS_T arg_format(g_calendar_options, "FORMAT",
                    "Set a custom output using glibc date formats; interpreted sequences for FORMAT are:\n"
                    "(using the date 2006-01-08)\n"
                    "%%: literal %\n"
                    "%-d, %d: day [8, 08]\n"
                    "%-m, %m: month [1, 01]\n"
                    "%y, %Y: year [06, 2006]\n"
                    "%-j, %j: day of the year [8, 008]\n"
                    "%A, %a: weekday name [Sunday, Sun]\n"
                    "%-V, %V: ISO 8601 week number [1, 01]\n"
                    "%B, %b: month name [January, Jan]\n"
                    "%u: day of the week, Monday being 1 [7]\n"
                    "See `man strftime' for a full list.",
                    {"format"});

  args::Group g_scale_options(ap_main, "Scale options:\n(VALUE can be float point or integer)");
  ARGD_T arg_value(g_scale_options, "VALUE", "Set initial value", {"value"})
  ,      arg_min_value(g_scale_options, "VALUE", "Set minimum value", {"min-value"})
  ,      arg_max_value(g_scale_options, "VALUE", "Set maximum value", {"max-value"})
  ,      arg_step(g_scale_options, "VALUE", "Set step size", {"step"});

  args::Group g_text_info_options(ap_main, "Text information options:");
  ARGS_T arg_checkbox(g_text_info_options, "TEXT", "Enable an \"I read and agree\" checkbox", {"checkbox"});
  ARG_T  arg_auto_scroll(g_text_info_options, "auto-scroll", "Always scroll to the bottom of the text",
                         {"auto-scroll"});

  args::Group g_notification_options(ap_main, "Notification options:");
  ARGI_T arg_timeout(g_notification_options, "SECONDS", "Set the timeout value for the notification in seconds",
                     {"timeout"});
#ifdef USE_DLOPEN
  ARG_T  arg_libnotify(g_notification_options, "libnotify", "Use libnotify to display the notification (timeout "
                       "value may be ignored by some desktop environments)", {"libnotify"});
#else
  const bool arg_libnotify = false;
#endif

  args::Group g_indicator_options(ap_main, "Indicator options:");
#ifdef USE_DLOPEN
  ARG_T unused_arg1(g_indicator_options, "native", "Use the operating system's native indicator system", {"native"});
#ifdef HAVE_QT
  ARG_T unused_arg2(g_indicator_options, "native-gtk", "Use the Gtk+ indicator (libappindicator)", {"native-gtk"})
  ,     unused_arg3(g_indicator_options, "native-qt", "Use the Qt5 indicator", {"native-qt"});
#endif
#endif  /* USE_DLOPEN */
  ARGS_T arg_listen(g_indicator_options, "PATH", "Listen for input from a named pipe (will be created if needed); "
                    "the following commands are available: run icon:<nameOfIcon> quit",
                    {"listen"});
  ARG_T unused_arg4(g_indicator_options, "auto-close", "Remove the indicator icon when the command was run",
                     {"auto-close"});

  const char *fltk_using = "using FLTK version " FLTK_VERSION_STRING " - http://www.fltk.org";

  /* ignore any errors and always print help if --help or -h was among the arguments */
  for (int i = argc - 1; i > 0; --i) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      std::cout << ap_main << "  " << fltk_using << "\n\n  " PROJECT_URL "\n" << std::endl;
      return 0;
    }
  }

  try {
    ap_main.ParseCLI(argc, argv);
  }
  catch (args::Error &e) {
    std::cerr << e.what() << "\nSee `" << argv[0] << " --help' for more information" << std::endl;
    return 1;
  }

  if (arg_version) {
    std::cout << fltk_using << std::endl;
    return 0;
  }

  if (arg_message +
      arg_warning +
      arg_error +
      arg_question +
      arg_dnd +
      arg_file +
      arg_directory +
      arg_entry +
      arg_password +
      arg_progress +
      arg_calendar +
      arg_date +
      arg_color +
      arg_scale +
      arg_checklist +
      arg_radiolist +
      arg_dropdown +
      arg_html +
      arg_text_info +
      arg_notification +
      arg_indicator +
      arg_font > 1)
  {
    std::cerr << argv[0] << ": two or more dialog options specified" << std::endl;
    return 1;
  }

#if defined(HAVE_QT) && defined(USE_DLOPEN)
  if (arg_native_gtk && arg_native_qt) {
    std::cerr << argv[0] << ": cannot use `--native-gtk' and `--native-qt' together" << std::endl;
    return 1;
  }
#endif

  /* do the localization BEFORE we set
   * the user-specified button labels */
  l10n();

  window_decoration = arg_undecorated ? false : true;
  window_taskbar = arg_skip_taskbar ? false : true;
  bool with_icon_box = arg_no_symbol ? false : true;
  resizable = arg_fixed ? false : true;
  override_pos = arg_center ? 5 : 0;
  always_on_top = arg_always_on_top;
  bool check_devices = arg_no_devices ? false : true;

  quote = arg_quoted_output ? "\"" : "";

  GETCSTR(msg, arg_text);
  GETCSTR(title, arg_title);
  GETCSTR(fl_ok, arg_ok_label);
  GETCSTR(fl_close, arg_close_label);
  GETCSTR(fl_cancel, arg_cancel_label);
  GETCSTR(fl_yes, arg_yes_label);
  GETCSTR(fl_no, arg_no_label);

  const char *but_alt = NULL;
  GETCSTR(but_alt, arg_alt_label);

  char separator = '|';
  if (arg_separator) {
    std::string s = args::get(arg_separator);
    if (s == "\\n") {
      separator = '\n';
    } else if (s == "\\t") {
      separator = '\t';
    } else {
      if (s.size() == 0) {
        std::cerr << argv[0] << ": error `--separator': empty string" << std::endl;
        return 1;
      } else if (s.size() > 1) {
        std::cerr << argv[0] << ": error `--separator': separator must be a single character" << std::endl;
        return 1;
      }
      separator = s[0];
    }
  }

  /* geometry */
  GETVAL(override_w, arg_width);
  GETVAL(override_h, arg_height);
  GETVAL(override_x, arg_posx);
  GETVAL(override_y, arg_posy);
  GETVAL(override_pos, arg_position);

  if (arg_geometry) {
    std::string s = args::get(arg_geometry);
    if (sscanf(s.c_str(), "%dx%d+%d+%d", &override_w, &override_h, &override_x, &override_y) != 4) {
      std::cerr << argv[0] << ": error `--geometry=WxH+X+Y': wrong format" << std::endl;
      return 1;
    }
  }

  int dialog = DIALOG_MESSAGE;  /* default message type */

  if (arg_about) {
    dialog = DIALOG_ABOUT;
  }
  if (arg_dnd) {
    dialog = DIALOG_DND;
  }
  if (arg_message) {
    dialog = DIALOG_MESSAGE;
  }
  if (arg_warning) {
    dialog = DIALOG_WARNING;
  }
  if (arg_error) {
    dialog = DIALOG_ERROR;
  }
  if (arg_question) {
    dialog = DIALOG_QUESTION;
  }
  if (arg_entry) {
    dialog = DIALOG_INPUT;
  }
  if (arg_password) {
    dialog = DIALOG_PASSWORD;
  }
  if (arg_color) {
    dialog = DIALOG_COLOR;
  }
  if (arg_font) {
    dialog = DIALOG_FONT;
  }

  const char *html = NULL;
  GETCSTR(html, arg_html);
  if (html) {
    dialog = DIALOG_HTML;
  }

  /* file / directory */
  if (arg_file) {
    dialog = DIALOG_FILE_CHOOSER;
  }
  if (arg_directory) {
    dialog = DIALOG_DIR_CHOOSER;
  }

  int native_mode = NATIVE_NONE;

#ifdef USE_DLOPEN
  if (arg_native) {
    native_mode = NATIVE_ANY;
  }

#ifdef HAVE_QT
  if (arg_native_gtk) {
    native_mode = NATIVE_GTK;
  } else if (arg_native_qt) {
    native_mode = NATIVE_QT;
  }
#endif
#endif  /* USE_DLOPEN */

  /* notification */
  int timeout = 5;
  if (arg_notification) {
    dialog = DIALOG_NOTIFY;
    GETVAL(timeout, arg_timeout);
  }

  /* indicator */
  const char *indicator_command = NULL;
  const char *named_pipe = NULL;
  if (arg_indicator) {
    dialog = DIALOG_INDICATOR;
    GETCSTR(indicator_command, arg_indicator);

    if (arg_listen) {
      named_pipe = args::get(arg_listen).c_str();

      if (strlen(named_pipe) == 0) {
        std::cerr << argv[0] << ": error `--listen': empty string" << std::endl;
        return 1;
      }
    }
  }

  /* progress */
  int multi = 1;
  long kill_pid = -1;
  if (arg_progress) {
    dialog = DIALOG_PROGRESS;

    if (arg_pulsate && arg_multi) {
      std::cerr << argv[0] << ": cannot use `--multi' and `--pulsate' together" << std::endl;
      return 1;
    }

    GETVAL(kill_pid, arg_watch_pid);
    GETVAL(multi, arg_multi);
    multi = (multi > 1) ? multi : 1;
  }

  /* scale */
  double scale_min = 0, scale_max = 100, scale_init = 0, scale_step = 1;
  if (arg_scale) {
    dialog = DIALOG_SCALE;

    GETVAL(scale_min, arg_min_value);
    scale_init = scale_min;
    GETVAL(scale_max, arg_max_value);
    GETVAL(scale_init, arg_value);
    GETVAL(scale_step, arg_step);

    if (scale_step <= 0) {
      std::cerr << argv[0] << ": error `--step': value cannot be negative or zero" << std::endl;
      return 1;
    }
  }

  /* checklist */
  std::string checklist_options = "";
  if (arg_checklist) {
    dialog = DIALOG_CHECKLIST;
    GETVAL(checklist_options, arg_checklist);
  }

  /* radiolist */
  std::string radiolist_options = "";
  if (arg_radiolist) {
    dialog = DIALOG_RADIOLIST;
    GETVAL(radiolist_options, arg_radiolist);
  }

  /* dropdown */
  std::string dropdown_options = "";
  if (arg_dropdown) {
    dialog = DIALOG_DROPDOWN;
    GETVAL(dropdown_options, arg_dropdown);
  }

  /* calendar / date */
  const char *format = NULL;
  GETCSTR(format, arg_format);
  if (arg_calendar) {
    dialog = DIALOG_CALENDAR;
  }
  if (arg_date) {
    dialog = DIALOG_DATE;
  }

  /* text-info */
  const char *checkbox = NULL;
  if (arg_text_info) {
    dialog = DIALOG_TEXTINFO;
    GETCSTR(checkbox, arg_checkbox);

    if (arg_checkbox && arg_auto_close) {
      std::cerr << argv[0] << ": cannot use `--checkbox' and `--auto-close' together" << std::endl;
      return 1;
    }
  }

  /* keep fltk's '@' symbols enabled for HTML, date and calendar dialogs */
  if (arg_html + arg_calendar + arg_date == 0) {
    Fl::set_labeltype(FL_NORMAL_LABEL, draw_cb, measure_cb);
  }

  /* set scheme */
  const char *scheme = "gtk+";
  GETCSTR(scheme, arg_scheme);

  if (strcasecmp(scheme, "fltk") == 0 ||
      strcasecmp(scheme, "base") == 0 ||
      strcasecmp(scheme, "simple") == 0 ||
      strcasecmp(scheme, "none") == 0 ||
      strcasecmp(scheme, "x11") == 0)
  {
    Fl::scheme("none");
  } else if (strcasecmp(scheme, "gleam") == 0) {
    Fl::scheme("gleam");
  } else if (strcasecmp(scheme, "plastic") == 0) {
    Fl::scheme("plastic");
  } else {
    Fl::scheme("gtk+");
  }

  /* set window icon and system colors */
  const char *icon = NULL;
  GETCSTR(icon, arg_icon);

  if (!arg_notification) {
    Fl_RGB_Image *rgb = img_to_rgb(icon);

    if (!rgb) {
      rgb = new Fl_PNG_Image(NULL, icon_png, icon_png_len);
    }

    Fl_Window::default_icon(rgb);

    if (arg_system_colors) {
      Fl::get_system_colors();
    } else {
      /* make default colors a bit lighter */
      Fl::set_color(FL_BACKGROUND_COLOR, fl_lighter(FL_BACKGROUND_COLOR));
      Fl::set_color(FL_SELECTION_COLOR, fl_lighter(FL_SELECTION_COLOR));
    }
  }

  /* recommended in Fl_Double_Window.H */
  Fl::visual(FL_DOUBLE|FL_INDEX);

  if (arg_no_escape) {
    Fl::add_handler(esc_handler);
  }

  switch (dialog) {
    case DIALOG_ABOUT:
      break;
    case DIALOG_MESSAGE:
      return dialog_message(MESSAGE_TYPE_INFO, with_icon_box, but_alt);
    case DIALOG_WARNING:
      return dialog_message(MESSAGE_TYPE_WARNING, with_icon_box, but_alt);
    case DIALOG_ERROR:
      return dialog_message(MESSAGE_TYPE_ERROR, with_icon_box, but_alt);
    case DIALOG_QUESTION:
      return dialog_message(MESSAGE_TYPE_QUESTION, with_icon_box, but_alt);
    case DIALOG_INPUT:
      return dialog_message(MESSAGE_TYPE_INPUT, false, but_alt);
    case DIALOG_PASSWORD:
      return dialog_message(MESSAGE_TYPE_PASSWORD, false, but_alt);
    case DIALOG_SCALE:
      return dialog_message(MESSAGE_TYPE_SCALE, false, but_alt, scale_min, scale_max, scale_step, scale_init);
    case DIALOG_FILE_CHOOSER:
      return dialog_file_chooser(FILE_CHOOSER, native_mode, arg_classic, check_devices);
    case DIALOG_DIR_CHOOSER:
      return dialog_file_chooser(DIR_CHOOSER, native_mode, arg_classic, check_devices);
    case DIALOG_NOTIFY:
      return dialog_notify(argv[0], timeout, icon, arg_libnotify);
    case DIALOG_PROGRESS:
      return dialog_progress(arg_pulsate, multi, kill_pid, arg_auto_close, arg_no_cancel);
    case DIALOG_TEXTINFO:
      return dialog_textinfo(arg_auto_scroll, checkbox, arg_auto_close, arg_no_cancel);
    case DIALOG_CHECKLIST:
      return dialog_checklist(checklist_options, arg_return_value, arg_check_all, separator);
    case DIALOG_RADIOLIST:
      return dialog_radiolist(radiolist_options, arg_return_number, separator);
    case DIALOG_DROPDOWN:
      return dialog_dropdown(dropdown_options, arg_return_number, separator);
    case DIALOG_CALENDAR:
      return dialog_calendar(format);
    case DIALOG_DATE:
      return dialog_date(format);
    case DIALOG_DND:
      return dialog_dnd();
    case DIALOG_HTML:
      return dialog_html_viewer(html);
    case DIALOG_COLOR:
      return dialog_color();
    case DIALOG_FONT:
      return dialog_font();
    case DIALOG_INDICATOR:
      return dialog_indicator(indicator_command, icon, native_mode, named_pipe, arg_auto_close);
    default:
      break;
  }

  return about();
}

