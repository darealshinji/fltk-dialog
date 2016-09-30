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
#include <FL/Fl_Window.H>

#include <iostream>
#include <string>

#include "Flek/Fl_Calendar.H"
#include "fltk-dialog.hpp"


static Fl_Window *calendar_win;

static void calendar_ok_cb(Fl_Widget*)
{
  calendar_win->hide();
}

static void calendar_cancel_cb(Fl_Widget*)
{
  calendar_win->hide();
  ret = 1;
}

int dialog_fl_calendar(std::string format)
{
  Fl_Calendar      *calendar;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  if (title == NULL)
  {
    title = (char *)"FLTK calendar";
  }

  /* one calendar unit = 32px
   * calendar widget width/height = 32px * 7 = 224px
   */
  calendar_win = new Fl_Window(244, 281, title);
  calendar = new Fl_Calendar(10, 10, 224, 224);
  calendar_win->begin();  /* don't remove! */
  calendar_win->callback(calendar_cancel_cb);
  {
    but_ok = new Fl_Return_Button(10, 244, 107, 26, fl_ok);
    but_ok->callback(calendar_ok_cb);
    but_cancel = new Fl_Button(127, 244, 107, 26, fl_cancel);
    but_cancel->callback(calendar_cancel_cb);
  }
  calendar_win->end();
  calendar_win->show();
  Fl::run();

  if (ret == 0)
  {
    print_date(format, calendar->year(), calendar->month(), calendar->day());
  }
  return ret;
}

