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


Fl_Calendar *cal;

/* must be global */
char formattedDate[255];

char *getFormattedDate(int y, int m, int d, char *fmt)
{
  /* based on strptime(3) example code */

  struct tm tm;
  char buf[255];
  char date[31];

  sprintf(date, "%d-%d-%d", y, m, d);
  memset(&tm, 0, sizeof(struct tm));
  strptime(date, "%Y-%m-%d", &tm);
  strftime(buf, sizeof(buf), fmt, &tm);
  strncpy(formattedDate, buf, sizeof(buf));
  return formattedDate;
}

static void calendar_ok_cb(Fl_Widget *w)
{
  w->window()->hide();
  return;
}

static void calendar_cancel_cb(Fl_Widget*)
{
  delete cal;
  exit(1);
}

int dialog_fl_calendar(       char *calendar_title,
                       std::string  fmt)
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

  int ret, y, m, d;
  char *fmtc, *date_formatted;

  if (calendar_title == NULL)
  {
    calendar_title = (char *)"FLTK calendar";
  }

  if (fmt == "")
  {
    fmt = "%Y-%m-%d";
  }
  else
  {
    /* glibc date formats
     * example date: 2006-01-08 */
    fmt = repstr(fmt, "%", "%%");   /* literal %; add first! */
    fmt = repstr(fmt, "d", "%-d");  /* day (8) */
    fmt = repstr(fmt, "D", "%d");   /* day (08) */
    fmt = repstr(fmt, "m", "%-m");  /* month (1) */
    fmt = repstr(fmt, "M", "%m");   /* month (01) */
    fmt = repstr(fmt, "y", "%y");   /* year (06) */
    fmt = repstr(fmt, "Y", "%Y");   /* year (2006) */
    fmt = repstr(fmt, "j", "%-j");  /* day of the year (8) */
    fmt = repstr(fmt, "J", "%j");   /* day of the year (008) */
    fmt = repstr(fmt, "W", "%A");   /* weekday name (Sunday) */
    fmt = repstr(fmt, "w", "%a");   /* weekday name (Sun) */
    fmt = repstr(fmt, "n", "%-V");  /* ISO 8601 week number (1) */
    fmt = repstr(fmt, "N", "%V");   /* ISO 8601 week number (01) */
    fmt = repstr(fmt, "B", "%B");   /* month name (January) */
    fmt = repstr(fmt, "b", "%b");   /* month name (Jan) */
    fmt = repstr(fmt, "u", "%u");   /* day of the week, Monday being 1 (7) */
  }

  win = new Fl_Window(winw, winh, calendar_title);
  cal = new Fl_Calendar(bord, bord, calw, calw);
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

  y = cal->year();
  m = cal->month();
  d = cal->day();
  fmtc = (char *)fmt.c_str();
  date_formatted = getFormattedDate(y, m, d, fmtc);
  std::cout << date_formatted << std::endl;

  delete cal;
  return ret;
}

