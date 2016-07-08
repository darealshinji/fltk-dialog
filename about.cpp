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
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>

/* std::string, c_str */
#include <string>

#include "fltk.xpm"
#include "fltk-dialog.h"


std::string get_fltk_version();

static void about_but_cb(Fl_Widget*)
{
  exit(0);
}

int about() {
  Fl_Window *win;
  Fl_Box    *box;
  Fl_Pixmap *pix;
  Fl_Button *but;

  int winw = 360;
  int winh = 220;
  int bord = 10;
  int butw = 100;
  int buth = 25;

  std::string getver = get_fltk_version();
  std::string about_text = "\n"
    "- FLTK dialog -\n"
    "Display dialog boxes from shell scripts.\n\n"
    "Using FLTK version " + getver + " (www.fltk.org).";
  const char *about_text_c = about_text.c_str();

  win = new Fl_Window(winw, winh);
  box = new Fl_Box(0, 0, winw, winh-bord*3, about_text_c);
  pix = new Fl_Pixmap(fltk_xpm);
  but = new Fl_Button((winw-butw)/2, winh-buth-bord,
                      butw, buth, fl_close);
  box->box(FL_NO_BOX);
  box->image(pix);
  but->callback(about_but_cb);
  win->label("About FLTK dialog");
  win->end();
  win->show();
  return Fl::run();
}

