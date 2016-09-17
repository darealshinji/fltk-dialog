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
#include <string>    /* std::string, c_str */
#include <stdio.h>   /* sprintf */
#include <stdlib.h>  /* exit */
#include <string.h>  /* memset, strncpy */
#include <time.h>    /* strptime, strftime */

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
  Fl_Window *win;
  Fl_Return_Button *but_ok;
  Fl_Button *but_cancel;

  int bord = 10;
  int calu = 32;
  int calw = calu*7;             /* 224; make sure it's divisible by 7 */
  int winw = calw+bord*2;        /* 244 */
  int winh = winw+calu+bord/2;   /* 281 */
  int buth = 26;
  int butw = winw/2-(bord*3)/2;  /* 107 */

  struct tm time;
  int ret;
  char tmp[31];
  char date[255];

  if (title == NULL)
  {
    title = (char *)"FLTK calendar";
  }

  if (format == "")
  {
    format = "%Y-%m-%d";
  }
  else
  {
    /* glibc date formats
     * example date: 2006-01-08 */
# define REP(a, b)  format = repstr(format, a, b)
    REP("\\n", "\n"); /* newline */
    REP("\\t", "\t"); /* tab */
    REP("%", "%%");   /* literal % */
    REP("d", "%-d");  /* day (8) */
    REP("D", "%d");   /* day (08) */
    REP("m", "%-m");  /* month (1) */
    REP("M", "%m");   /* month (01) */
    REP("y", "%y");   /* year (06) */
    REP("Y", "%Y");   /* year (2006) */
    REP("j", "%-j");  /* day of the year (8) */
    REP("J", "%j");   /* day of the year (008) */
    REP("W", "%A");   /* weekday name (Sunday) */
    REP("w", "%a");   /* weekday name (Sun) */
    REP("n", "%-V");  /* ISO 8601 week number (1) */
    REP("N", "%V");   /* ISO 8601 week number (01) */
    REP("B", "%B");   /* month name (January) */
    REP("b", "%b");   /* month name (Jan) */
    REP("u", "%u");   /* day of the week, Monday being 1 (7) */
# undef REP
  }

  win = new Fl_Window(winw, winh, title);
  calendar = new Fl_Calendar(bord, bord, calw, calw);
  win->begin();
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

  sprintf(tmp, "%d-%d-%d", calendar->year(), calendar->month(), calendar->day());
  memset(&time, 0, sizeof(struct tm));
  strptime(tmp, "%Y-%m-%d", &time);
  strftime(date, sizeof(date), format.c_str(), &time);

  std::cout << date << std::endl;

  delete calendar;
  return ret;
}

