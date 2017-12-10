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
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <string>

#include "Flek/Fl_Calendar.H"
#include "fltk-dialog.hpp"

static Fl_Double_Window *cal_win;

static void calendar_close_cb(Fl_Widget *, long p)
{
  cal_win->hide();
  ret = (int) p;
}

int dialog_calendar(std::string format)
{
  Fl_Calendar      *calendar;
  Fl_Group         *g;
  Fl_Box           *dummy;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  if (title == NULL)
  {
    title = "FLTK calendar";
  }

  /* one calendar unit = 32px
   * calendar widget width/height = 32px * 7 = 224px
   */
  cal_win = new Fl_Double_Window(244, 281, title);
  calendar = new Fl_Calendar(10, 10, 224, 224);
  cal_win->begin();  /* don't remove! */
  cal_win->size_range(244, 281, max_w, max_h);
  cal_win->callback(calendar_close_cb, 1);
  {
    g = new Fl_Group(0, 244, 244, 37);
    {
      dummy = new Fl_Box(9, 244, 1, 1);
      dummy->box(FL_NO_BOX);
      but_ok = new Fl_Return_Button(10, 244, 107, 26, fl_ok);
      but_ok->callback(calendar_close_cb, 0);
      but_cancel = new Fl_Button(127, 244, 107, 26, fl_cancel);
      but_cancel->callback(calendar_close_cb, 1);
    }
    g->resizable(dummy);
    g->end();
  }
  set_size(cal_win, calendar);
  set_position(cal_win);
  cal_win->end();
  set_taskbar(cal_win);
  cal_win->show();
  set_undecorated(cal_win);
  Fl::run();

  if (ret == 0)
  {
    print_date(format, calendar->year(), calendar->month(), calendar->day());
  }
  return ret;
}

