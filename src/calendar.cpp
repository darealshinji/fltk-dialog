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
#include <FL/fl_ask.H>  /* fl_ok, fl_cancel */
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Window.H>

#include <iostream>  /* std::cout, std::endl */
#include <string>    /* std::string */
#include <stdlib.h>  /* exit */

#include "Flek/Fl_Calendar.H"
#include "fltk-dialog.hpp"


Fl_Calendar *calendar;

static void calendar_ok_cb(Fl_Widget *w)
{
  w->window()->hide();
  return;
}

static void calendar_cancel_cb(Fl_Widget*)
{
  delete calendar;
  exit(1);
}

int dialog_fl_calendar(std::string format)
{
  Fl_Window        *win;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  int bord = 10;
  int calu = 32;
  int calw = calu*7;             /* 224; make sure it's divisible by 7 */
  int winw = calw+bord*2;        /* 244 */
  int winh = winw+calu+bord/2;   /* 281 */
  int buth = 26;
  int butw = winw/2-(bord*3)/2;  /* 107 */

  int ret;

  if (title == NULL)
  {
    title = (char *)"FLTK calendar";
  }

  win = new Fl_Window(winw, winh, title);
  calendar = new Fl_Calendar(bord, bord, calw, calw);
  win->begin();  /* don't remove! */
  win->callback(calendar_cancel_cb);  /* exit(1) */
  {
    but_ok = new Fl_Return_Button(calw-butw*2, winw, butw, buth, fl_ok);
    but_ok->callback(calendar_ok_cb);
    but_cancel = new Fl_Button(winw-butw-bord, winw, butw, buth, fl_cancel);
    but_cancel->callback(calendar_cancel_cb);
  }
  win->end();
  win->show();

  ret = Fl::run();
  print_date(format, calendar->year(), calendar->month(), calendar->day());

  delete calendar;
  return ret;
}

