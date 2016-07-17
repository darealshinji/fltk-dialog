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

/* std::cout, std::cerr, std::endl */
#include <iostream>
/* getopt_long_only */
#include <getopt.h>
/* strcmp */
#include <string.h>

#include "fltk-dialog.h"
#include "icon.xpm"

#define DIALOG_FL_MESSAGE 1
#define DIALOG_ALERT 2
#define DIALOG_FL_CHOICE 3
#define DIALOG_FL_FILE_CHOOSER 4
#define DIALOG_FL_DIR_CHOOSER 5
#define DIALOG_FL_INPUT 6
#define DIALOG_FL_PASSWORD 7
#define DIALOG_FL_COLOR 8
#define DIALOG_FL_COLOR_HTML 9
#define DIALOG_FL_PROGRESS 10
#define DIALOG_FL_VALUE_SLIDER 11


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

#if !defined(__APPLE__) && !defined(WIN32)
#  define USINGGTK << " (GTK)" <<
#else
#  define USINGGTK <<
#endif
#define H0 std::cout <<
#define H1 << std::endl
void print_usage(char *prog)
{
  H0 "Usage:" H1;
  H0 "  " << prog << " OPTION [...]" H1
    H1;
  H0 "Options:" H1;
  H0 "  -h, --help                 Show help options" H1;
  H0 "  -v, --version              Show FLTK and program version" H1;
  H0 "  --about                    About FLTK dialog" H1;
  H0 "  --text=TEXT                Set the dialog text" H1;
  H0 "  --title=TITLE              Set the dialog title" H1;
  H0 "  --warning                  Display warning dialog" H1;
  H0 "  --question                 Display question dialog" H1;
  H0 "  --file                     Display file selection dialog" H1;
  H0 "  --directory                Display directory selection dialog" H1;
  H0 "  --entry                    Display text entry dialog" H1;
  H0 "  --password                 Display password dialog" H1;
  H0 "  --progress                 Display progress indication dialog" H1;
  H0 "  --color                    Display color selection dialog; RGB output" H1;
  H0 "  --color-html               Display color selection dialog; HTML output" H1;
  H0 "  --scale                    Display scale dialog" H1
    H1;
  H0 "Question options:" H1;
  H0 "  --yes-label=TEXT           Sets the label of the Yes button" H1;
  H0 "  --no-label=TEXT            Sets the label of the No button" H1
    H1;
  H0 "File/directory selection options:" H1;
  H0 "  --native                   Use the operating system's native file" H1
  << "                             chooser" USINGGTK " if available, otherwise" H1
  << "                             fall back to FLTK's own version" H1
    H1;
  H0 "Progress options:" H1;
  H0 "  --auto-close               Set initial value" H1
    H1;
  H0 "Scale options:" H1;
  H0 "  --value=VALUE              Set initial value" H1;
  H0 "  --min-value=VALUE          Set minimum value" H1;
  H0 "  --max-value=VALUE          Set maximum value" H1;
  H0 "  --step=VALUE               Set step size" H1;
  H0 " VALUE can be float point or integer" H1
    H1;
}
#undef H0
#undef H1
#undef USINGGTK

int main(int argc, char **argv)
{
  char *msg = NULL;
  char *title = NULL;
  char *but_yes = NULL;
  char *but_no = NULL;
  char *minval = NULL;
  char *maxval = NULL;
  char *stepval = NULL;
  char *initval = NULL;
  int opt = 0;
  int long_index = 0;
  int dialog = DIALOG_FL_MESSAGE;  /* default message type */
  int native = 0;
  int autoclose = 0;

  /* using these to check if two or more dialog options were specified */
  int dalert, dchoice, dfilechooser, ddirchoser, dinput, dpassword,
    dcolor, dcolorhtml, dprogress, dvalslider;
  dalert = dchoice = dfilechooser = ddirchoser = dinput = dpassword =
    dcolor = dcolorhtml = dprogress = dvalslider = 0;

  /* disable fltk's '@' symbols */
  Fl::set_labeltype(FL_NORMAL_LABEL, draw_cb, measure_cb);

  /* set global default icon for all windows */
  Fl_Pixmap win_pixmap(icon_xpm);
  Fl_RGB_Image win_icon(&win_pixmap, Fl_Color(0));
  Fl_Window::default_icon(&win_icon);

  for (int i = 1; i < argc; ++i) {
    if (strcmp("--about", argv[i]) == 0) {
      return about();
    } else if ((strcmp("--help", argv[i]) == 0) ||
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
    { "text",       required_argument, 0, 'T' },
    { "title",      required_argument, 0, 't' },
    { "yes-label",  required_argument, 0, 'y' },
    { "no-label",   required_argument, 0, 'n' },
    { "value",      required_argument, 0, 'v' },
    { "min-value",  required_argument, 0, 'm' },
    { "max-value",  required_argument, 0, 'M' },
    { "step",       required_argument, 0, 's' },
    { "warning",    no_argument,       0, 'w' },
    { "question",   no_argument,       0, 'q' },
    { "file",       no_argument,       0, 'f' },
    { "directory",  no_argument,       0, 'd' },
    { "native",     no_argument,       0, 'N' },
    { "entry",      no_argument,       0, 'e' },
    { "password",   no_argument,       0, 'p' },
    { "color",      no_argument,       0, 'c' },
    { "color-html", no_argument,       0, 'C' },
    { "progress",   no_argument,       0, 'P' },
    { "auto-close", no_argument,       0, 'a' },
    { "scale",      no_argument,       0, 'S' },
    { 0, 0, 0, 0 }
  };

  while ((opt = getopt_long_only(argc, argv, "", long_options, &long_index))
         != -1)
  {
    switch (opt) {
      case 'T':
        msg = optarg;
        break;
      case 't':
        title = optarg;
        break;
      case 'y':
        but_yes = optarg;
        break;
      case 'n':
        but_no = optarg;
        break;
      case 'v':
        initval = optarg;
        break;
      case 'm':
        minval = optarg;
        break;
      case 'M':
        maxval = optarg;
        break;
      case 's':
        stepval = optarg;
        break;
      case 'w':
        dialog = DIALOG_ALERT;
        dalert = 1;
        break;
      case 'q':
        dialog = DIALOG_FL_CHOICE;
        dchoice = 1;
        break;
      case 'f':
        dialog = DIALOG_FL_FILE_CHOOSER;
        dfilechooser = 1;
        break;
      case 'd':
        dialog = DIALOG_FL_DIR_CHOOSER;
        ddirchoser = 1;
        break;
      case 'N':
        native = 1;
        break;
      case 'e':
        dialog = DIALOG_FL_INPUT;
        dinput = 1;
        break;
      case 'p':
        dialog = DIALOG_FL_PASSWORD;
        dpassword = 1;
        break;
      case 'c':
        dialog = DIALOG_FL_COLOR;
        dcolor = 1;
        break;
      case 'C':
        dialog = DIALOG_FL_COLOR_HTML;
        dcolorhtml = 1;
        break;
      case 'P':
        dialog = DIALOG_FL_PROGRESS;
        dprogress = 1;
        break;
      case 'a':
        autoclose = 1;
        break;
      case 'S':
        dialog = DIALOG_FL_VALUE_SLIDER;
        dvalslider = 1;
        break;
      default:
        print_usage(argv[0]);
        return 1;
    }
  }

  if ((dalert + dchoice + dfilechooser + ddirchoser + dinput +
       dpassword + dcolor + dcolorhtml + dprogress + dvalslider) >= 2)
  {
    std::cerr << argv[0] << ": two or more dialog options specified"
      << std::endl;
    return 1;
  }

  if ((native == 1) && ((dialog != DIALOG_FL_FILE_CHOOSER) &&
                        (dialog != DIALOG_FL_DIR_CHOOSER)))
  {
    std::cerr << argv[0] << ": --native can only be used with --file or "
      "--directory" << std::endl;
    return 1;
  }

  if ((dialog != DIALOG_FL_CHOICE) && ((but_yes != NULL) ||
                                       (but_no != NULL)))
  {
    std::cerr << argv[0] << ": --yes-label and --no-label can only be used "
      "with --question" << std::endl;
    return 1;
  }

  if ((autoclose == 1) && (dialog != DIALOG_FL_PROGRESS)) {
    std::cerr << argv[0] << ": --auto-close can only be used with --progress"
      << std::endl;
    return 1;
  }

  switch (dialog) {
    case DIALOG_FL_MESSAGE:
      return dialog_fl_message(msg, title);

    case DIALOG_ALERT:
      return dialog_fl_message(msg, title, 1);

    case DIALOG_FL_CHOICE:
      return dialog_fl_choice(msg, title, but_yes, but_no);

    case DIALOG_FL_FILE_CHOOSER:
      if (native == 1) {
        return dialog_fl_native_file_chooser(title);
      } else {
        return dialog_fl_file_chooser(title);
      }

    case DIALOG_FL_DIR_CHOOSER:
      if (native == 1) {
        return dialog_fl_native_file_chooser(title, 1);
      } else {
        return dialog_fl_dir_chooser(title);
      }

    case DIALOG_FL_INPUT:
      return dialog_fl_input(msg, title);

    case DIALOG_FL_PASSWORD:
      return dialog_fl_password(msg, title);

    case DIALOG_FL_COLOR:
      return dialog_fl_color(title);

    case DIALOG_FL_COLOR_HTML:
      return dialog_fl_color(title, 1);

    case DIALOG_FL_PROGRESS:
      return dialog_fl_progress(msg, title, autoclose);

    case DIALOG_FL_VALUE_SLIDER:
      return dialog_fl_value_slider(msg, title, minval,
                                    maxval, stepval, initval);
  }
}

