/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2020, djcj <djcj@gmx.de>
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

#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

#include "fltk-dialog.hpp"

#define MIN_YEAR 1
#define MAX_YEAR 9999

static Fl_Double_Window *win;
static Fl_Box *box_weekn[8];
static Fl_Button *but[7*6];
static Fl_Choice *box_month;
static Fl_Spinner *box_year;

static const Fl_Color clight = fl_lighter(FL_GRAY);
static const Fl_Color cdark = fl_darker(FL_GRAY);
static int selected_year = 0, selected_month = 0, selected_day = 0;
static int last_days_of_prev_m = 0;

static int ret = 1;

static void callback(Fl_Widget *, long n);
static void prev_month_cb(Fl_Widget *, long n);
static void next_month_cb(Fl_Widget *, long n);
static void set_calendar(bool get_current_day=true, int y=0, int m=0, int d=0);

static char *itostr(int n, char *buf, size_t buf_size)
{
  if (selected_language == LANG_AR) {
    std::string s;
    while (n != 0) {
      s.insert(0, "\xD9\xA0");
      s[1] += n % 10;
      n /= 10;
    }
    strncpy(buf, s.c_str(), buf_size - 1);
  } else {
    snprintf(buf, buf_size - 1, "%d", n);
  }

  return buf;
}

static int get_weekday(int y, int m, int d)
{
  /* Michael Keith and Tom Craver's algorithm */
  d += (m < 3) ? y-- : y - 2;
  return (23*m/9 + d + 4 + y/4 - y/100 + y/400) % 7;

  /* Tomohiko Sakamoto's algorithm */
  //const int t[] = { 0, 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  //if (m < 3) {
  //  y--;
  //}
  //return (y + y/4 - y/100 + y/400 + t[m] + d) % 7;
}

static int get_iso_week_number(int y, int m, int d)
{
  struct tm tm;
  char tmp[64];
  char wn[32];

  memset(&tm, 0, sizeof(struct tm));
  snprintf(tmp, sizeof(tmp) - 1, "%d %d %d", y, m, d);
  strptime(tmp, "%Y %m %d", &tm);
  strftime(wn, sizeof(wn), "%-V", &tm);
  return atoi(wn);
}

static void set_calendar(bool get_current_day, int y, int m, int d)
{
  time_t t;
  struct tm *tm = NULL;
  const Fl_Color cfont_active = FL_BLACK;
  const Fl_Color cfont_inactive = fl_darker(FL_WHITE);
  int wd, prev_m, next_m, prev_m_year, next_m_year;
  int days_of_m, days_of_prev_m, first_days_of_next_m;
  int i, j, n, wk[4];
  char buf[32] = {0};

  if (get_current_day) {
    t = time(NULL);
    tm = localtime(&t);
    y = tm->tm_year + 1900;
    m = tm->tm_mon + 1;
    d = tm->tm_mday;
    wd = tm->tm_wday;
  } else {
    if (y < MIN_YEAR || y > MAX_YEAR) {
      t = time(NULL);
      tm = localtime(&t);
      y = tm->tm_year + 1900;
    }

    if (m < 1 || m > 12) {
      if (tm == NULL) {
        t = time(NULL);
        tm = localtime(&t);
      }
      m = tm->tm_mon + 1;
    }

    int dim = days_in_month[leap_year(y)][m];

    if (d < 1 || d > dim) {
      if (tm == NULL) {
        t = time(NULL);
        tm = localtime(&t);
      }
      d = tm->tm_mday;

      if (d > dim) {
        d = dim;
      }
    }

    wd = get_weekday(y, m, d);
  }

  selected_year = y;
  selected_month = m;
  selected_day = d;

  /* shift around to make monday 0 and sunday 6 */
  if (wd == 0) {
    wd = 6;
  } else {
    wd--;
  }

  switch (m) {
    case 1:
      prev_m = 12;
      next_m = 2;
      prev_m_year = y - 1;
      next_m_year = y;
      break;
    case 12:
      prev_m = 11;
      next_m = 1;
      prev_m_year = y;
      next_m_year = y + 1;
      break;
    default:
      prev_m = m - 1;
      next_m = m + 1;
      prev_m_year = next_m_year = y;
      break;
  }

  days_of_m = days_in_month[leap_year(y)][m];
  days_of_prev_m = days_in_month[leap_year(prev_m_year)][prev_m];
  last_days_of_prev_m = 7 - ((d - wd - 1) % 7);
  if (last_days_of_prev_m > 7) {
    last_days_of_prev_m %= 7;
  }
  first_days_of_next_m = 7*6 - last_days_of_prev_m - days_of_m;

  box_month->value(m-1);
  box_year->value(y);

  /* last days of previous month */
  i = days_of_prev_m - last_days_of_prev_m + 1;
  j = 0;
  for ( ; i <= days_of_prev_m; ++i, ++j) {
    but[j]->copy_label(itostr(i, buf, sizeof(buf)));
    but[j]->labelcolor(cfont_inactive);
    but[j]->color(clight);
    but[j]->callback(prev_month_cb, prev_m);
  }

  /* days of current month */
  n = 0;
  for (i = 1; i <= days_of_m; ++i, ++j) {
    if (j % 7 == 0) {
      wk[n] = j;
      n++;
    }
    but[j]->copy_label(itostr(i, buf, sizeof(buf)));
    but[j]->labelcolor(cfont_active);
    but[j]->color((i == d) ? cdark : clight);
    but[j]->callback(callback, j);
  }

  /* first days of next month */
  for (i = 1; i <= first_days_of_next_m; ++i, ++j) {
    but[j]->copy_label(itostr(i, buf, sizeof(buf)));
    but[j]->labelcolor(cfont_inactive);
    but[j]->color(clight);
    but[j]->callback(next_month_cb, next_m);
  }

  /* ISO week numbers */
  n = get_iso_week_number(prev_m_year, prev_m, days_of_prev_m);
  box_weekn[1]->copy_label(itostr(n, buf, sizeof(buf)));

  if (m > 1 && m < 12) {
    n++;
    for (i = 2; i < 7; ++i, ++n) {
      box_weekn[i]->copy_label(itostr(n, buf, sizeof(buf)));
    }
  } else {
    for (i = 0; i < 4; ++i) {
      n = get_iso_week_number(y, m, wk[i]);
      box_weekn[i + 2]->copy_label(itostr(n, buf, sizeof(buf)));
    }

    n = get_iso_week_number(next_m_year, next_m, first_days_of_next_m);
    box_weekn[6]->copy_label(itostr(n, buf, sizeof(buf)));
  }

  win->redraw();
}

static void callback(Fl_Widget *, long n)
{
  selected_day = n + 1 - last_days_of_prev_m;

  for (int i = 0; i < 7*6; ++i) {
    but[i]->color(clight);
  }
  but[n]->color(cdark);

  win->redraw();
}

static void month_year_cb(Fl_Widget *) {
  selected_month = box_month->value() + 1;
  selected_year = box_year->value();
  set_calendar(false, selected_year, selected_month, selected_day);
}

static void prev_month_cb(Fl_Widget *, long n)
{
  selected_month = n;
  if (selected_month == 12) {
    selected_year--;
  }
  set_calendar(false, selected_year, selected_month, selected_day);
}

static void next_month_cb(Fl_Widget *, long n)
{
  selected_month = n;
  if (selected_month == 1) {
    selected_year++;
  }
  set_calendar(false, selected_year, selected_month, selected_day);
}

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = p;
}

int dialog_calendar(const char *format)
{
  Fl_Group         *g1, *g2, *g3;
  Fl_Box           *dummy1, *dummy2;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  const int but_h = 28;
  int but_w = 36, line = 1;

  if (!title) {
    title = "FLTK calendar";
  }

  win = new Fl_Double_Window(but_w*8 + 20, but_h*9 + 40, title);
  win->callback(close_cb, 1);
  {
    g1 = new Fl_Group(0, 0, win->w(), but_h + 10);
    {
      box_month = new Fl_Choice(10, 10, 120, but_h);
      box_month->box(FL_THIN_DOWN_BOX);
      box_month->down_box(FL_BORDER_BOX);
      box_month->menu(item_month);
      box_month->callback(month_year_cb);

      box_year = new Fl_Spinner(win->w() - 100, 10, 90, but_h);
      box_year->box(FL_THIN_DOWN_BOX);
      box_year->minimum(MIN_YEAR);
      box_year->maximum(MAX_YEAR);
      box_year->step(1);
      box_year->callback(month_year_cb);

      dummy1 = new Fl_Box(box_month->x() + box_month->w(), 10, 1, 1);
      dummy1->box(FL_NO_BOX);
    }
    g1->resizable(dummy1);
    g1->end();

    g2 = new Fl_Group(10, but_h + 20, win->w() - 20, but_h*7);
    {
      for (int i = 0; i < 7; ++i) {
        box_weekn[i] = new Fl_Box(10, but_h * i + g2->y(), but_w, but_h);
        box_weekn[i]->box(FL_FLAT_BOX);
        box_weekn[i]->color(cdark);
        //box_weekn[i]->labelcolor(???);
      }

      for (int i = 0; i < 7; ++i) {
        { Fl_Box *o = new Fl_Box(but_w*i + 10 + but_w, but_h + 20, but_w, but_h, weekdays[selected_language][i]);
         o->box(FL_FLAT_BOX);
         //o->labelcolor(???);
         o->color(cdark); }
      }

      for (int i = 0; i < 7*6; ++i) {
        but[i] = new Fl_Button(but_w * (i%7) + 10 + but_w, but_h * line + g2->y(), but_w, but_h);
        but[i]->box(FL_FLAT_BOX);
        but[i]->color(clight);
        but[i]->selection_color(cdark);
        but[i]->clear_visible_focus();
        if (i % 7 == 6) {
          line++;
        }
      }
    }
    g2->end();

    g3 = new Fl_Group(10, g2->y() + g2->h() + 10, win->w() - 20, but_h + 10);
    {
      but_w = measure_button_width(fl_cancel, 20);
      but_cancel = new Fl_Button(win->w() - 10 - but_w, g3->y(), but_w, but_h, fl_cancel);
      but_cancel->callback(close_cb, 1);

      but_w = measure_button_width(fl_ok, 40);
      but_ok = new Fl_Return_Button(but_cancel->x() - 10 - but_w, g3->y(), but_w, but_h, fl_ok);
      but_ok->callback(close_cb, 0);

      dummy2 = new Fl_Box(but_ok->x() - 1, g3->y(), 1, 1);
      dummy2->box(FL_NO_BOX);
    }
    g3->resizable(dummy2);
    g3->end();
  }
  set_calendar();
  run_window(win, g2, 234, 196);

  if (ret == 0) {
    std::cout << quote << format_date(format, selected_year, selected_month, selected_day) << quote << std::endl;
  }
  return ret;
}

