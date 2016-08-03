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
#include "icon.xpm"


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

void window_cb(Fl_Widget*)
{
  exit(1);
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
  "  --file                     Display file selection dialog\n"
  "  --directory                Display directory selection dialog\n"
  "  --entry                    Display text entry dialog\n"
  "  --password                 Display password dialog\n"
  "  --progress                 Display progress indication dialog\n"
  "  --calendar                 Display calendar dialog; returns date as Y-M-D\n"
  "  --color                    Display color selection dialog\n"
  "  --scale                    Display scale dialog\n"
  "  --html=FILE                Display HTML viewer\n"
  "  --no-escape                Don't close window on hitting ESC button\n"
  "  --scheme=NAME              Set the window scheme to use: default, gtk+,\n"
  "                             gleam, plastic or simple; default is gtk+\n"
  "\n"
  "Question options:\n"
  "  --yes-label=TEXT           Sets the label of the Yes button\n"
  "  --no-label=TEXT            Sets the label of the No button\n"
  "  --alt-label=TEXT           Adds a third button and sets its label;\n"
  "                             exit code is 2\n"
  "\n"
  "File/directory selection options:\n"
  "  --native                   Use the operating system's native file\n"
  "                             chooser (GTK) if available, otherwise\n"
  "                             fall back to FLTK's own version\n"
  "\n"
  "Progress options:\n"
  "  --auto-close               Dismiss the dialog when 100% has been reached\n"
  "  --no-close                 Block the window's close button until 100% has\n"
  "                             been reached\n"
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
  "\n"
  "Scale options:\n"
  "  --value=VALUE              Set initial value\n"
  "  --min-value=VALUE          Set minimum value\n"
  "  --max-value=VALUE          Set maximum value\n"
  "  --step=VALUE               Set step size\n"
  "                             VALUE can be float point or integer\n"
    << std::endl;
}

int main(int argc, char **argv)
{
  char *msg = NULL;
  char *title = NULL;
  char *but_yes = NULL;
  char *but_no = NULL;
  char *but_alt = NULL;
  char *minval = NULL;
  char *maxval = NULL;
  char *stepval = NULL;
  char *initval = NULL;
  std::string fmt = "";
  const char *scheme = "default";
  const char *scheme_default = "gtk+";
  const char *html = NULL;
  int opt = 0;
  int long_index = 0;
  int dialog = DIALOG_FL_MESSAGE;  /* default message type */
  int native = 0;
  int autoclose = 0;
  int dont_close = 0;

  /* using these to check if two or more dialog options were specified */
  int dabout, dalert, dcalendar, dchoice, dfilechooser, ddirchoser, dhtml,
    dinput, dpassword, dcolor, dprogress, dvalslider;
  dabout = dalert = dcalendar = dchoice = dfilechooser = ddirchoser = dhtml =
    dinput = dpassword = dcolor = dprogress = dvalslider = 0;

  /* set global default icon for all windows */
  Fl_Pixmap win_pixmap(icon_xpm);
  Fl_RGB_Image win_icon(&win_pixmap, Fl_Color(0));
  Fl_Window::default_icon(&win_icon);

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
    if ((strcmp("--help", argv[i]) == 0) ||
        (strcmp("-h", argv[i]) == 0))
    {
      print_usage(argv[0]);
      return 0;
    } else if ((strcmp("--version", argv[i]) == 0) ||
               (strcmp("-v", argv[i]) == 0))
    {
      print_fltk_version();
      return 0;
    }
  }

  static struct option long_options[] = {
    { "about",      no_argument,       0, LO_ABOUT      },
    { "no-escape",  no_argument,       0, LO_NO_ESCAPE  },
    { "no-close",   no_argument,       0, LO_NO_CLOSE   },
    { "scheme",     required_argument, 0, LO_SCHEME     },
    { "format",     required_argument, 0, LO_FORMAT     },
    { "text",       required_argument, 0, LO_TEXT       },
    { "title",      required_argument, 0, LO_TITLE      },
    { "yes-label",  required_argument, 0, LO_YES_LABEL  },
    { "no-label",   required_argument, 0, LO_NO_LABEL   },
    { "alt-label",  required_argument, 0, LO_ALT_LABEL  },
    { "value",      required_argument, 0, LO_VALUE      },
    { "min-value",  required_argument, 0, LO_MIN_VALUE  },
    { "max-value",  required_argument, 0, LO_MAX_VALUE  },
    { "step",       required_argument, 0, LO_STEP       },
    { "html",       required_argument, 0, LO_HTML       },
    { "warning",    no_argument,       0, LO_WARNING    },
    { "question",   no_argument,       0, LO_QUESTION   },
    { "file",       no_argument,       0, LO_FILE       },
    { "directory",  no_argument,       0, LO_DIRECTORY  },
    { "native",     no_argument,       0, LO_NATIVE     },
    { "entry",      no_argument,       0, LO_ENTRY      },
    { "password",   no_argument,       0, LO_PASSWORD   },
    { "color",      no_argument,       0, LO_COLOR      },
    { "progress",   no_argument,       0, LO_PROGRESS   },
    { "auto-close", no_argument,       0, LO_AUTO_CLOSE },
    { "scale",      no_argument,       0, LO_SCALE      },
    { "calendar",   no_argument,       0, LO_CALENDAR   },
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
      case LO_NO_CLOSE:
        dont_close=1;
        break;
      case LO_SCHEME:
        scheme = optarg;
        break;
      case LO_FORMAT:
        fmt = std::string(optarg);
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
      case LO_HTML:
        dialog = DIALOG_HTML;
        html = optarg;
        dhtml = 1;
        break;
      case LO_WARNING:
        dialog = DIALOG_ALERT;
        dalert = 1;
        break;
      case LO_QUESTION:
        dialog = DIALOG_FL_CHOICE;
        dchoice = 1;
        break;
      case LO_FILE:
        dialog = DIALOG_FL_FILE_CHOOSER;
        dfilechooser = 1;
        break;
      case LO_DIRECTORY:
        dialog = DIALOG_FL_DIR_CHOOSER;
        ddirchoser = 1;
        break;
      case LO_NATIVE:
        native = 1;
        break;
      case LO_ENTRY:
        dialog = DIALOG_FL_INPUT;
        dinput = 1;
        break;
      case LO_PASSWORD:
        dialog = DIALOG_FL_PASSWORD;
        dpassword = 1;
        break;
      case LO_COLOR:
        dialog = DIALOG_FL_COLOR;
        dcolor = 1;
        break;
      case LO_PROGRESS:
        dialog = DIALOG_FL_PROGRESS;
        dprogress = 1;
        break;
      case LO_AUTO_CLOSE:
        autoclose = 1;
        break;
      case LO_SCALE:
        dialog = DIALOG_FL_VALUE_SLIDER;
        dvalslider = 1;
        break;
      case LO_CALENDAR:
        dialog = DIALOG_FL_CALENDAR;
        dcalendar = 1;
        break;
      default:
        P_ERRX("See `" << argv[0] << " --help' for available commands");
    }
  }

  if ((dabout + dalert + dcalendar + dchoice + dfilechooser + ddirchoser +
       dhtml + dinput + dpassword + dcolor + dprogress + dvalslider) >= 2)
  {
    P_ERR("two or more dialog options specified");
  }

  if ((native == 1) && ((dialog != DIALOG_FL_FILE_CHOOSER) &&
                        (dialog != DIALOG_FL_DIR_CHOOSER)))
  {
    P_ERR("--native can only be used with --file or --directory");
  }

  if ((dialog != DIALOG_FL_CHOICE) &&
      ((but_yes != NULL) || (but_no != NULL) || (but_alt != NULL)))
  {
    P_ERR("--yes-label/--no-label/--alt-label can only be used with --question");
  }

  if ((autoclose == 1) && (dialog != DIALOG_FL_PROGRESS)) {
    P_ERR("--auto-close can only be used with --progress");
  }

  if ((dont_close == 1) && (dialog != DIALOG_FL_PROGRESS)) {
    P_ERR("--no-close can only be used with --progress");
  }

  if ((fmt != "") && (dialog != DIALOG_FL_CALENDAR)) {
    P_ERR("--format can only be used with --calendar");
  }

  /* keep fltk's '@' symbols only enabled for HTML viewer */
  if (dialog != DIALOG_HTML) {
    Fl::set_labeltype(FL_NORMAL_LABEL, draw_cb, measure_cb);
  }

  if (strcmp("gtk", scheme) == 0) {
    scheme = "gtk+";
  } else if (strcmp("simple", scheme) == 0) {
    scheme = "none";
  }
  if (strcmp("default", scheme) == 0) {
    Fl::scheme(scheme_default);
  } else if ((strcmp("none", scheme) == 0) ||
             (strcmp("gtk+", scheme) == 0) ||
             (strcmp("gleam", scheme) == 0) ||
             (strcmp("plastic", scheme) == 0))
  {
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

    case DIALOG_FL_FILE_CHOOSER:
      if (native == 1) {
        return dialog_fl_native_file_chooser(title, FILE_CHOOSER);
      } else {
        return dialog_fl_file_chooser(title);
      }

    case DIALOG_FL_DIR_CHOOSER:
      if (native == 1) {
        return dialog_fl_native_file_chooser(title, DIR_CHOOSER);
      } else {
        return dialog_fl_dir_chooser(title);
      }

    case DIALOG_FL_INPUT:
      return dialog_fl_input(msg, title);

    case DIALOG_HTML:
      return dialog_html_viewer(html);

    case DIALOG_FL_PASSWORD:
      return dialog_fl_password(msg, title);

    case DIALOG_FL_COLOR:
      return dialog_fl_color(title);

    case DIALOG_FL_PROGRESS:
      return dialog_fl_progress(msg, title, autoclose, dont_close);

    case DIALOG_FL_VALUE_SLIDER:
      return dialog_fl_value_slider(msg, title, minval, maxval, stepval, initval);

    case DIALOG_FL_CALENDAR:
      return dialog_fl_calendar(title, fmt);
  }
}

