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

#ifdef ISO_WEEK_NUMBERS
# define XSP 32
static Fl_Box *weeknum[8];

static void callback(Fl_Widget *, void *)
{
  char tmp[32];
  for (int i = 0; i < 6; i++) {
    calendar->int_to_str(tmp, calendar->weeknumber(i));
    weeknum[i+2]->copy_label(tmp);
  }
  win->redraw();
}
#else
# define XSP 0
#endif  /* !ISO_WEEK_NUMBERS */

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = (int) p;
}

int dialog_calendar(std::string format)
{
#ifdef ISO_WEEK_NUMBERS
  Fl_Group         *g0;
#endif
  Fl_Group         *g;
  Fl_Box           *dummy;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  if (!title) {
    title = "FLTK calendar";
  }

  /* one calendar unit = 32px
   * calendar widget width = 32px * 7 = 224px
   */
  win = new Fl_Double_Window(244 + XSP, 281, title);
  calendar = new Fl_Calendar(10 + XSP, 10, 224, 224);
  win->begin();  /* don't remove! */
  win->size_range(244 + XSP, 281, max_w, max_h);
  win->callback(close_cb, 1);
  {
    if (calendar_arabic) {
      calendar->arabic(1);
    }

#ifdef ISO_WEEK_NUMBERS
    calendar->callback(callback);

    g0 = new Fl_Group(10, 10, 224, 224);
    {
      for (int i = 0; i < 8; i++) {
        weeknum[i] = new Fl_Box (10, 10 + i*28, 32, 28);
        if (i > 1) {
          weeknum[i]->labelsize(12);
          weeknum[i]->box(FL_FLAT_BOX);
        } else {
          weeknum[i]->box(FL_NO_BOX);
        }
      }
      callback(NULL, NULL);
    }
    g0->resizable(calendar);
    g0->end();
#endif  /* ISO_WEEK_NUMBERS */

    g = new Fl_Group(0, 244, 244 + XSP, 37);
    {
      int but_w = measure_button_width(fl_cancel, 20);
      but_cancel = new Fl_Button(234 + XSP - but_w, 244, but_w, 26, fl_cancel);
      but_cancel->callback(close_cb, 1);
      but_w = measure_button_width(fl_ok, 40);
      but_ok = new Fl_Return_Button(but_cancel->x() - 10 - but_w, 244, but_w, 26, fl_ok);
      but_ok->callback(close_cb, 0);
      dummy = new Fl_Box(but_ok->x() - 1, 244, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    g->resizable(dummy);
    g->end();
  }

#ifdef ISO_WEEK_NUMBERS
  run_window(win, g0);
#else
  run_window(win, calendar);
#endif

  if (ret == 0) {
    std::cout << format_date(format, calendar->year(), calendar->month(), calendar->day()) << std::endl;
  }
  return ret;
}

