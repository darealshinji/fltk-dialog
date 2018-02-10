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
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <time.h>

#include "FDate.H"
#include "fltk-dialog.hpp"


static Fl_Double_Window *win;
static Fl_Choice *month;
static Fl_Spinner *year, *day;

static void callback(Fl_Widget *)
{
  int dim = FDate::days[month->value() + 1];

  if (FDate::leap_year(year->value()) && dim == 28) {
    ++dim;
  }
  if (day->value() > dim) {
    day->value(dim);
  }
  day->maximum(dim);
}

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = (int) p;
}

int dialog_date(std::string format)
{
  Fl_Group         *g1, *g2;
  Fl_Box           *dummy1, *dummy2;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  const char *mnames[] = {
    fdate_mon_jan, fdate_mon_feb, fdate_mon_mar, fdate_mon_apr,
    fdate_mon_may, fdate_mon_jun, fdate_mon_jul, fdate_mon_aug,
    fdate_mon_sep, fdate_mon_oct, fdate_mon_nov, fdate_mon_dec
  };

#define MONTH_ITEM(x) mnames[x - 1], 0,0,0,0, FL_NORMAL_LABEL, 0, 14, 0

  Fl_Menu_Item item_month[] = {
    { MONTH_ITEM(JANUARY)   },
    { MONTH_ITEM(FEBRUARY)  },
    { MONTH_ITEM(MARCH)     },
    { MONTH_ITEM(APRIL)     },
    { MONTH_ITEM(MAY)       },
    { MONTH_ITEM(JUNE)      },
    { MONTH_ITEM(JULY)      },
    { MONTH_ITEM(AUGUST)    },
    { MONTH_ITEM(SEPTEMBER) },
    { MONTH_ITEM(OCTOBER)   },
    { MONTH_ITEM(NOVEMBER)  },
    { MONTH_ITEM(DECEMBER)  },
    { 0,0,0,0,0,0,0,0,0 }
  };

  time_t t          = time(0);
  struct tm *time   = localtime(&t);
  int current_year  = time->tm_year + 1900;
  int current_month = time->tm_mon;  /* January = 0 */
  int current_day   = time->tm_mday;

  if (!title) {
    title = "FLTK date";
  }

  win = new Fl_Double_Window(400, 114, title);
  win->size_range(400, 114, max_w, max_h);
  win->callback(close_cb, 1);
  {
    g1 = new Fl_Group(0, 0, 400, 114);
    {
      month = new Fl_Choice(10, 28, 120, 30, "Month:");
      month->down_box(FL_BORDER_BOX);
      month->align(FL_ALIGN_TOP_LEFT);
      month->menu(item_month);
      month->value(current_month);
      month->callback(callback);

      day = new Fl_Spinner(20 + 120, 28, 120, 30, "Day:");
      day->labelsize(14);
      day->align(FL_ALIGN_TOP_LEFT);
      day->minimum(1);
      day->step(1);
      day->value(current_day);

      year = new Fl_Spinner(30 + 2*120, 28, 120, 30, "Year:");
      year->labelsize(14);
      year->align(FL_ALIGN_TOP_LEFT);
      year->minimum(1);
      year->maximum(9999);
      year->step(1);
      year->value(current_year);
      year->callback(callback);

      /* run callback to update days */
      callback(NULL);

      dummy1 = new Fl_Box(10, 58, 380, 1);
      dummy1->box(FL_NO_BOX);
    }
    g1->resizable(dummy1);
    g1->end();

    g2 = new Fl_Group(0, 74, 400, 46);
    {
      int but_w = measure_button_width(fl_cancel, 20);
      but_cancel = new Fl_Button(win->w() - 10 - but_w, 78, but_w, 26, fl_cancel);
      but_cancel->callback(close_cb, 1);
      but_w = measure_button_width(fl_ok, 40);
      but_ok = new Fl_Return_Button(but_cancel->x() - 10 - but_w, 78, but_w, 26, fl_ok);
      but_ok->callback(close_cb, 0);
      dummy2 = new Fl_Box(but_ok->x() - 1, 74, 1, 1);
      dummy2->box(FL_NO_BOX);
    }
    g2->resizable(dummy2);
    g2->end();
  }
  run_window(win, g1);

  if (ret == 0) {
    std::cout << format_date(format, year->value(), month->value() + 1, day->value()) << std::endl;
  }
  return ret;
}
