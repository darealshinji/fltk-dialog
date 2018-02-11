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

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <string>

#include "Fl_Calendar.H"
#include "fltk-dialog.hpp"

static Fl_Double_Window *win;
static Fl_Calendar *calendar;

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = (int) p;
}

int dialog_calendar(std::string format)
{
  Fl_Group         *g;
  Fl_Box           *dummy;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  if (!title) {
    title = "FLTK calendar";
  }

  /* one calendar unit = 32*26 px */
  win = new Fl_Double_Window(32*8 + 20, 26*8 + 20 + 37, title);
  calendar = new Fl_Calendar(10, 10, 32*8, 26*8);
  win->begin();  /* don't remove! */
  win->size_range(32*8 + 20, 26*8 + 20 + 37, max_w, max_h);
  win->callback(close_cb, 1);
  {
    if (calendar_arabic) {
      calendar->arabic(1);
    }

    g = new Fl_Group(0, 26*8+20, win->w(), 37);
    {
      int but_w = measure_button_width(fl_cancel, 20);
      but_cancel = new Fl_Button(win->w() - 10 - but_w, 26*8 + 20, but_w, 26, fl_cancel);
      but_cancel->callback(close_cb, 1);
      but_w = measure_button_width(fl_ok, 40);
      but_ok = new Fl_Return_Button(but_cancel->x() - 10 - but_w, 26*8 + 20, but_w, 26, fl_ok);
      but_ok->callback(close_cb, 0);
      dummy = new Fl_Box(but_ok->x() - 1, 26*8 + 20, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    g->resizable(dummy);
    g->end();
  }
  run_window(win, calendar);

  if (ret == 0) {
    std::cout << format_date(format, calendar->year(), calendar->month(), calendar->day()) << std::endl;
  }
  return ret;
}

