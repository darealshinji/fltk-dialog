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
#include <FL/Fl_Choice.H>
#include <FL/Fl_Spinner.H>

#include <iostream>  /* std::cout, std::endl */
#include <time.h>    /* time, localtime */

#include "Flek/FDate.H"
#include "fltk-dialog.hpp"


#define FDATE_LABELSIZE 14

Fl_Choice  *fdate_month;
Fl_Spinner *fdate_year;
Fl_Spinner *fdate_day;

Fl_Menu_Item item_month[] = {
  { FDate::month_name[ JANUARY   - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ FEBRUARY  - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ MARCH     - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ APRIL     - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ MAY       - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ JUNE      - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ JULY      - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ AUGUST    - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ SEPTEMBER - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ OCTOBER   - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ NOVEMBER  - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { FDate::month_name[ DECEMBER  - 1 ], 0, 0, 0, 0, FL_NORMAL_LABEL, 0, FDATE_LABELSIZE, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 ,0 }
};

void set_max_days()
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

static void max_days_cb(Fl_Widget*)
{
  set_max_days();
}

static void date_ok_cb(Fl_Widget *w)
{
  w->window()->hide();
  return;
}

static void date_cancel_cb(Fl_Widget*)
{
  delete fdate_day;
  delete fdate_year;
  delete fdate_month;
  exit(1);
}

int dialog_fdate(std::string format)
{
  Fl_Window        *win;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  int bord = 10;
  int droph = 30;
  int dropw = 120;
  int textheight = 18;
  int buth = 26;
  int butw = 100;
  int winw = bord*4+dropw*3;
  int winh = textheight+droph+buth+bord*4;

  time_t t = time(0);
  struct tm *time = localtime(&t);
  int current_year  = time->tm_year + 1900;
  int current_month = time->tm_mon;  /* January = 0 */
  int current_day   = time->tm_mday;

  int ret;

  if (title == NULL)
  {
    title = (char *)"FLTK date";
  }

  win = new Fl_Window(winw, winh, title);
  win->callback(date_cancel_cb);  /* exit(1) */
  {
    fdate_month = new Fl_Choice(bord, bord+textheight, dropw, droph, "Month:");
    fdate_month->down_box(FL_BORDER_BOX);
    fdate_month->align(FL_ALIGN_TOP_LEFT);
    fdate_month->menu(item_month);
    fdate_month->value(current_month);
    fdate_month->callback(max_days_cb);

    fdate_year = new Fl_Spinner(bord*3+dropw*2, bord+textheight, dropw, droph, "Year:");
    fdate_year->labelsize(FDATE_LABELSIZE);
    fdate_year->align(FL_ALIGN_TOP_LEFT);
    fdate_year->minimum(1);
    fdate_year->maximum(9999);
    fdate_year->step(1);
    fdate_year->value(current_year);
    fdate_year->callback(max_days_cb);

    fdate_day = new Fl_Spinner(bord*2+dropw, bord+textheight, dropw, droph, "Day:");
    fdate_day->labelsize(FDATE_LABELSIZE);
    fdate_day->align(FL_ALIGN_TOP_LEFT);
    fdate_day->minimum(1);
    fdate_day->step(1);
    fdate_day->value(current_day);
    set_max_days();

    but_ok = new Fl_Return_Button(winw-butw*2-bord*2, winh-buth-bord, butw, buth, fl_ok);
    but_ok->callback(date_ok_cb);

    but_cancel = new Fl_Button(winw-butw-bord, winh-buth-bord, butw, buth, fl_cancel);
    but_cancel->callback(date_cancel_cb);
  }
  win->end();
  win->show();

  ret = Fl::run();
  print_date(format, fdate_year->value(), fdate_month->value() + 1, fdate_day->value());

  delete fdate_day;
  delete fdate_year;
  delete fdate_month;
  return ret;
}
