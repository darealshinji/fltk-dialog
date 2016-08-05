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
#include <FL/fl_draw.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_RGB_Image.H>

#include <iostream>  /* std::cout, std::cerr, std::endl */
#include <string>    /* std::string, c_str */
#include <getopt.h>  /* getopt_long_only */
#include <stdlib.h>  /* exit */
#include <string.h>  /* strcmp */

#include "fltk-dialog.h"
#include "main.h"
#ifdef WITH_ICON
#  include "icon.xpm"
#endif


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
  if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
    return 1; /* ignore Escape key */
  }
  return 0;
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
  "  --warning                  Display warning dialog\n"
  "  --question                 Display question dialog\n"
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
#ifdef WITH_HTML
  "  --html=FILE                Display HTML viewer\n"
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
#endif  /* WITH_FILE */
#ifdef WITH_PROGRESS
  "\n"
  "Progress options:\n"
  "  --auto-close               Dismiss the dialog when 100% has been reached\n"
  "  --no-cancel                Hide cancel button\n"
#endif  /* WITH_PROGRESS */
#ifdef WITH_CALENDAR
  "\n"
  "Calendar options:\n"
  "  --format=FORMAT            Set a custom output format\n"
  "                             Interpreted sequences for FORMAT are:\n"
  "                             (using the date 2006-01-08)\n"
  "                             d  day (8)\n"
  "                             D  day (08)\n"
  "                             m  month (1)\n"
  "                             M  month (01)\n"
  "                             y  year (06)\n"
  "                             Y  year (2006)\n"
  "                             j  day of the year (8)\n"
  "                             J  day of the year (008)\n"
  "                             W  weekday name (Sunday)\n"
  "                             w  weekday name (Sun)\n"
  "                             n  ISO 8601 week number (1)\n"
  "                             N  ISO 8601 week number (01)\n"
  "                             B  month name (January)\n"
  "                             b  month name (Jan)\n"
  "                             u  day of the week, Monday being 1 (7)\n"
#endif  /* WITH_CALENDAR */
#ifdef WITH_SCALE
  "\n"
  "Scale options:\n"
  "  --value=VALUE              Set initial value\n"
  "  --min-value=VALUE          Set minimum value\n"
  "  --max-value=VALUE          Set maximum value\n"
  "  --step=VALUE               Set step size\n"
  "                             VALUE can be float point or integer\n"
#endif  /* WITH_SCALE */
    << std::endl;
}

int main(int argc, char **argv)
{
  char *msg = NULL;
  char *title = NULL;
  char *but_yes = NULL;
  char *but_no = NULL;
  char *but_alt = NULL;
#ifdef WITH_SCALE
  char *minval = NULL;
  char *maxval = NULL;
  char *stepval = NULL;
  char *initval = NULL;
#endif  /* WITH_SCALE */
#ifdef WITH_CALENDAR
  std::string fmt = "";
#endif
  const char *scheme = "default";
  const char *scheme_default = "gtk+";
#ifdef WITH_HTML
  const char *html = NULL;
#endif
  int opt = 0;
  int long_index = 0;
  int dialog = DIALOG_FL_MESSAGE;  /* default message type */
#ifdef WITH_FILE
  bool native = false;
#endif
#ifdef WITH_PROGRESS
  bool autoclose = false;
  bool hide_cancel = false;
#endif

  /* using these to check if two or more dialog options were specified */
  int dabout, dalert, dcalendar, dchoice, dfilechooser, ddirchoser, dhtml,
    dinput, dpassword, dcolor, dprogress, dvalslider;
  dabout = dalert = dcalendar = dchoice = dfilechooser = ddirchoser = dhtml =
    dinput = dpassword = dcolor = dprogress = dvalslider = 0;

#ifdef WITH_ICON
  /* set global default icon for all windows */
  Fl_Pixmap win_pixmap(icon_xpm);
  Fl_RGB_Image win_icon(&win_pixmap, Fl_Color(0));
  Fl_Window::default_icon(&win_icon);
#endif

  /* use a slightly brighter gray than the default one in FLTK */
  Fl::background(204, 204, 204);

  /* run "About" dialog if invoked
   * without command line options */
  if (argc < 2) {
    /* disable fltk's '@' symbols */
    Fl::set_labeltype(FL_NORMAL_LABEL, draw_cb, measure_cb);
    Fl::scheme(scheme_default);
    return about();
  }

  for (int i = 1; i < argc; ++i) {
    if (strcmp("--help", argv[i]) == 0 || strcmp("-h", argv[i]) == 0) {
      print_usage(argv[0]);
      return 0;
    } else if (strcmp("--version", argv[i]) == 0 || strcmp("-v", argv[i]) == 0) {
      print_fltk_version();
      return 0;
    }
  }

  static struct option long_options[] = {
    { "about",      no_argument,       0, LO_ABOUT      },
    { "no-escape",  no_argument,       0, LO_NO_ESCAPE  },
    { "scheme",     required_argument, 0, LO_SCHEME     },
    { "text",       required_argument, 0, LO_TEXT       },
    { "title",      required_argument, 0, LO_TITLE      },
    { "yes-label",  required_argument, 0, LO_YES_LABEL  },
    { "no-label",   required_argument, 0, LO_NO_LABEL   },
    { "alt-label",  required_argument, 0, LO_ALT_LABEL  },
#ifdef WITH_HTML
    { "html",       required_argument, 0, LO_HTML       },
#endif
    { "warning",    no_argument,       0, LO_WARNING    },
    { "question",   no_argument,       0, LO_QUESTION   },
#ifdef WITH_FILE
    { "file",       no_argument,       0, LO_FILE       },
    { "directory",  no_argument,       0, LO_DIRECTORY  },
    { "native",     no_argument,       0, LO_NATIVE     },
#endif  /* WITH_FILE */
#ifdef WITH_ENTRY
    { "entry",      no_argument,       0, LO_ENTRY      },
#endif
#ifdef WITH_PASSWORD
    { "password",   no_argument,       0, LO_PASSWORD   },
#endif
#ifdef WITH_COLOR
    { "color",      no_argument,       0, LO_COLOR      },
#endif
#ifdef WITH_PROGRESS
    { "progress",   no_argument,       0, LO_PROGRESS   },
    { "auto-close", no_argument,       0, LO_AUTO_CLOSE },
    { "no-cancel",  no_argument,       0, LO_NO_CANCEL  },
#endif  /* WITH_PROGRESS */
#ifdef WITH_SCALE
    { "scale",      no_argument,       0, LO_SCALE      },
    { "value",      required_argument, 0, LO_VALUE      },
    { "min-value",  required_argument, 0, LO_MIN_VALUE  },
    { "max-value",  required_argument, 0, LO_MAX_VALUE  },
    { "step",       required_argument, 0, LO_STEP       },
#endif  /* WITH_SCALE */
#ifdef WITH_CALENDAR
    { "calendar",   no_argument,       0, LO_CALENDAR   },
    { "format",     required_argument, 0, LO_FORMAT     },
#endif
    { 0, 0, 0, 0 }
  };

  while ((opt = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1) {
    switch (opt) {
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
      case LO_YES_LABEL:
        but_yes = optarg;
        break;
      case LO_NO_LABEL:
        but_no = optarg;
        break;
      case LO_ALT_LABEL:
        but_alt = optarg;
        break;
#ifdef WITH_HTML
      case LO_HTML:
        dialog = DIALOG_HTML;
        html = optarg;
        dhtml = 1;
        break;
#endif  /* WITH_HTML */
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
#endif  /* WITH_FILE */
#ifdef WITH_ENTRY
      case LO_ENTRY:
        dialog = DIALOG_FL_INPUT;
        dinput = 1;
        break;
#endif  /* WITH_ENTRY */
#ifdef WITH_PASSWORD
      case LO_PASSWORD:
        dialog = DIALOG_FL_PASSWORD;
        dpassword = 1;
        break;
#endif  /* WITH_PASSWORD */
#ifdef WITH_COLOR
      case LO_COLOR:
        dialog = DIALOG_FL_COLOR;
        dcolor = 1;
        break;
#endif  /* WITH_COLOR */
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
#endif  /* WITH_PROGRESS */
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
#endif  /* WITH_SCALE */
#ifdef WITH_CALENDAR
      case LO_CALENDAR:
        dialog = DIALOG_FL_CALENDAR;
        dcalendar = 1;
        break;
      case LO_FORMAT:
        fmt = std::string(optarg);
        break;
#endif  /* WITH_CALENDAR */
      default:
        P_ERRX("See `" << argv[0] << " --help' for available commands");
    }
  }

  if ((dabout + dalert + dcalendar + dchoice + dfilechooser + ddirchoser +
       dhtml + dinput + dpassword + dcolor + dprogress + dvalslider) >= 2) {
    P_ERR("two or more dialog options specified");
  }

#ifdef WITH_FILE
  if (native && (dialog != DIALOG_FL_FILE_CHOOSER && dialog != DIALOG_FL_DIR_CHOOSER)) {
    P_ERR("--native can only be used with --file or --directory");
  }
#endif

  if (dialog != DIALOG_FL_CHOICE && (but_yes != NULL || but_no != NULL || but_alt != NULL)) {
    P_ERR("--yes-label/--no-label/--alt-label can only be used with --question");
  }

#ifdef WITH_PROGRESS
  if (autoclose && dialog != DIALOG_FL_PROGRESS) {
    P_ERR("--auto-close can only be used with --progress");
  }
  if (hide_cancel && dialog != DIALOG_FL_PROGRESS) {
    P_ERR("--no-cancel can only be used with --progress");
  }
#endif  /* WITH_PROGRESS */

#ifdef WITH_SCALE
  if (dialog != DIALOG_FL_VALUE_SLIDER &&
      (minval != NULL || maxval != NULL || stepval != NULL || initval != NULL)) {
    P_ERR("--value/--min-value/--max-value/--step can only be used with --scale");
  }
#endif  /* WITH_SCALE */

#ifdef WITH_CALENDAR
  if (fmt != "" && dialog != DIALOG_FL_CALENDAR) {
    P_ERR("--format can only be used with --calendar");
  }
#endif

#ifdef WITH_HTML
  /* keep fltk's '@' symbols only enabled for HTML viewer */
  if (dialog != DIALOG_HTML) {
    Fl::set_labeltype(FL_NORMAL_LABEL, draw_cb, measure_cb);
  }
#endif  /* WITH_HTML */

  if (strcmp("gtk", scheme) == 0) {
    scheme = "gtk+";
  } else if (strcmp("simple", scheme) == 0) {
    scheme = "none";
  }
  if (strcmp("default", scheme) == 0) {
    Fl::scheme(scheme_default);
  } else if (strcmp("none", scheme) == 0 || strcmp("gtk+", scheme) == 0 ||
             strcmp("gleam", scheme) == 0 || strcmp("plastic", scheme) == 0) {
    Fl::scheme(scheme);
  } else {
    P_ERRX("\"" << scheme << "\" is not a valid scheme!\n"\
      "Available schemes are: default gtk+ gleam plastic simple");
  }

  switch (dialog) {
    case DIALOG_ABOUT:
      return about();
    case DIALOG_FL_MESSAGE:
      return dialog_fl_message(msg, title, MESSAGE);
    case DIALOG_ALERT:
      return dialog_fl_message(msg, title, ALERT);
    case DIALOG_FL_CHOICE:
      return dialog_fl_choice(msg, title, but_yes, but_no, but_alt);
#ifdef WITH_FILE
    case DIALOG_FL_FILE_CHOOSER:
      if (native) {
        return dialog_fl_native_file_chooser(title, FILE_CHOOSER);
      } else {
        return dialog_fl_file_chooser(title);
      }
    case DIALOG_FL_DIR_CHOOSER:
      if (native) {
        return dialog_fl_native_file_chooser(title, DIR_CHOOSER);
      } else {
        return dialog_fl_dir_chooser(title);
      }
#endif  /* WITH_FILE */
#ifdef WITH_ENTRY
    case DIALOG_FL_INPUT:
      return dialog_fl_input(msg, title);
#endif
#ifdef WITH_HTML
    case DIALOG_HTML:
      return dialog_html_viewer(html);
#endif
#ifdef WITH_PASSWORD
    case DIALOG_FL_PASSWORD:
      return dialog_fl_password(msg, title);
#endif
#ifdef WITH_COLOR
    case DIALOG_FL_COLOR:
      return dialog_fl_color(title);
#endif
#ifdef WITH_PROGRESS
    case DIALOG_FL_PROGRESS:
      return dialog_fl_progress(msg, title, autoclose, hide_cancel);
#endif
#ifdef WITH_SCALE
    case DIALOG_FL_VALUE_SLIDER:
      return dialog_fl_value_slider(msg, title, minval, maxval, stepval, initval);
#endif
#ifdef WITH_CALENDAR
    case DIALOG_FL_CALENDAR:
      return dialog_fl_calendar(title, fmt);
#endif
  }
}

