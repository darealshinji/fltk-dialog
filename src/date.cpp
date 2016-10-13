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
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Window.H>

#include <iostream>
#include <time.h>

#include "Flek/FDate.H"
#include "fltk-dialog.hpp"


static Fl_Window  *fdate_win;
static Fl_Choice  *fdate_month;
static Fl_Spinner *fdate_year;
static Fl_Spinner *fdate_day;

static void max_days_cb(Fl_Widget *)
{
  int dim = FDate::days[fdate_month->value() + 1];

  if (FDate::leap_year(fdate_year->value()) && dim == 28)
  {
    ++dim;
  }

  if (fdate_day->value() > dim)
  {
    fdate_day->value(dim);
  }

  fdate_day->maximum(dim);
}

static void date_close_cb(Fl_Widget *, long p)
{
  fdate_win->hide();
  ret = (int) p;
}

int dialog_fdate(std::string format)
{
  Fl_Group         *g, *buttongroup;
  Fl_Box           *dummy1, *dummy2;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  Fl_Menu_Item item_month[] = {
    { FDate::month_name[ JANUARY   - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ FEBRUARY  - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ MARCH     - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ APRIL     - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ MAY       - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ JUNE      - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ JULY      - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ AUGUST    - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ SEPTEMBER - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ OCTOBER   - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ NOVEMBER  - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { FDate::month_name[ DECEMBER  - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  };

  time_t t          = time(0);
  struct tm *time   = localtime(&t);
  int current_year  = time->tm_year + 1900;
  int current_month = time->tm_mon;  /* January = 0 */
  int current_day   = time->tm_mday;

  if (title == NULL)
  {
    title = (char *)"FLTK date";
  }

  fdate_win = new Fl_Window(400, 114, title);
  fdate_win->size_range(400, 114, max_w, max_h);
  fdate_win->callback(date_close_cb, 1);
  {
    g = new Fl_Group(0, 0, 400, 114);
    {
      fdate_month = new Fl_Choice(10, 28, 120, 30, "Month:");
      fdate_month->down_box(FL_BORDER_BOX);
      fdate_month->align(FL_ALIGN_TOP_LEFT);
      fdate_month->menu(item_month);
      fdate_month->value(current_month);
      fdate_month->callback(max_days_cb);

      fdate_year = new Fl_Spinner(270, 28, 120, 30, "Year:");
      fdate_year->labelsize(14);
      fdate_year->align(FL_ALIGN_TOP_LEFT);
      fdate_year->minimum(1);
      fdate_year->maximum(9999);
      fdate_year->step(1);
      fdate_year->value(current_year);
      fdate_year->callback(max_days_cb);

      fdate_day = new Fl_Spinner(140, 28, 120, 30, "Day:");
      fdate_day->labelsize(14);
      fdate_day->align(FL_ALIGN_TOP_LEFT);
      fdate_day->minimum(1);
      fdate_day->step(1);
      fdate_day->value(current_day);
      max_days_cb(NULL);

      dummy1 = new Fl_Box(10, 58, 380, 1);
      dummy1->box(FL_NO_BOX);
    }
    g->resizable(dummy1);
    g->end();

    buttongroup = new Fl_Group(0, 74, 400, 46);
    {
      dummy2 = new Fl_Box(179, 74, 1, 1);
      dummy2->box(FL_NO_BOX);
      but_ok = new Fl_Return_Button(180, 78, 100, 26, fl_ok);
      but_ok->callback(date_close_cb, 0);
      but_cancel = new Fl_Button(290, 78, 100, 26, fl_cancel);
      but_cancel->callback(date_close_cb, 1);
    }
    buttongroup->resizable(dummy2);
    buttongroup->end();
  }
  set_size(fdate_win, g);
  set_position(fdate_win);
  fdate_win->end();
  fdate_win->show();
  Fl::run();

  if (ret == 0)
  {
    print_date(format, fdate_year->value(), fdate_month->value() + 1, fdate_day->value());
  }
  return ret;
}
