/*
 * Copyright 1999-2000 by the Flek development team.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * Authors: James Dean Palmer <james@tiger-marmalade.com>
 *          2016-2018  djcj <djcj@gmx.de> (modified for fltk-dialog)
 */

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Repeat_Button.H>

#include <Fl_Calendar.H>
#include <FDate.H>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static bool arabic_ = false;

void
Fl_Calendar_Base::arabic (int i)
{
  arabic_ = (i == 0) ? false : true;
  update();
}

int
Fl_Calendar_Base::arabic ()
{
  return arabic_ ? 1 : 0;
}

static void fl_calendar_prv_month_cb (Fl_Widget *, void *b);
static void fl_calendar_nxt_month_cb (Fl_Widget *, void *b);

static void fl_calendar_button_cb (Fl_Widget *a, void *b)
{
  long j = 0;
  Fl_Calendar *c = (Fl_Calendar *)b;
  Fl_Button   *sb;
  int numdays = c->days_in_month () + 1;
  for (int i = 1; i < numdays; i++) {
    sb = c->day_button(i);
    sb->color (52);
    if ((Fl_Button *)a == sb) {
      c->selected_day (i);
      j = i;
      sb->color (sb->selection_color());
    }
  }
  c->redraw();
  c->do_callback(c, j);
}

Fl_Calendar_Base::Fl_Calendar_Base (int x, int y, int w, int h, const char *l)
  : Fl_Group (x, y, w, h, l), FDate ()
{
  labelsize_ = 12;

  for (int i = 0; i < (7*6); i++) {
    days[i] = new Fl_Button ((w/7)*(i%7) + x,
                             (h/6)*(i/7) + y,
                             (w/7),
                             (h/6));
    days[i]->labelsize (labelsize_);
    days[i]->down_box (FL_FLAT_BOX);
    days[i]->box (FL_FLAT_BOX);
    days[i]->color (color());
  }
}

void Fl_Calendar_Base::csize (int cx, int cy, int cw, int ch)
{
  /*
  int oi = (cw-(7*(cw/7)))/2;
  int of = (cw-(7*(cw/7))) - oi;
  int xi, wxi;
  */

  for (int i = 0; i < (7*6); i++) {
    /*
    if ((i%7) == 0) {
      xi = 0;
    } else {
      xi = oi;
    }
    if ((i%7) == 0) {
      wxi = oi;
    } else if ((i%7) == 6) {
      wxi = of;
    } else {
      wxi = 0;
    }
    */

    days[i]->resize ((cw/7)*(i%7) + cx /* + xi */,
                     (ch/6)*(i/7) + cy,
                     (cw/7) /* + wxi */,
                     (ch/6));
  }
}

void
Fl_Calendar_Base::int_to_str (char *t, int n)
{
  if (arabic_) {
    const char *f;
    char c[32];
    sprintf (c, "%d", n);
    memset (t, '\0', strlen(t));
    for (size_t i = 0; i < strlen (c) && c[i] != '\0'; i++) {
      if      (c[i] == '1') { f = "\xD9\xA1"; }
      else if (c[i] == '2') { f = "\xD9\xA2"; }
      else if (c[i] == '3') { f = "\xD9\xA3"; }
      else if (c[i] == '4') { f = "\xD9\xA4"; }
      else if (c[i] == '5') { f = "\xD9\xA5"; }
      else if (c[i] == '6') { f = "\xD9\xA6"; }
      else if (c[i] == '7') { f = "\xD9\xA7"; }
      else if (c[i] == '8') { f = "\xD9\xA8"; }
      else if (c[i] == '9') { f = "\xD9\xA9"; }
      else /*if (c[i] == '0')*/{ f = "\xD9\xA0"; }
      strcat (t, f);
    }
  } else {
    sprintf (t, "%d", n);
  }
}

void
Fl_Calendar_Base::update ()
{
  int dow = day_of_week (year (), month (), 0);
  int dim = days_in_month (month (), leap_year (year ()));
  int i, n;
  char tmp[32];
#ifdef ISO_WEEK_NUMBERS
  int j = 0;
  int wk[6][3] = { 0, 0 };
#endif

  int dipm_month;  /* previous month */
  int dipm_year;   /* year of previous month */
  if (month () == JANUARY) {
    dipm_month = DECEMBER;
    dipm_year = year () - 1;
  } else {
    dipm_month = month () - 1;
    dipm_year = year ();
  }
  int dipm = days_in_month (dipm_month, dipm_year);

  /* last days of previous month */
  for (i = 0; i < dow; i++) {
    int_to_str (tmp, i-dow+dipm+1);
    days[i]->copy_label (tmp);
    days[i]->down_box (FL_FLAT_BOX);
    days[i]->box (FL_FLAT_BOX);
    days[i]->color (color());
    days[i]->callback (fl_calendar_prv_month_cb, this);
    days[i]->show ();
  }

  /* current month */
  for (i = dow; i < (dim+dow); i++) {
    n = i+1-dow;
#ifdef ISO_WEEK_NUMBERS
    if (n == 1 || i % 7 == 0) {
      wk[j][0] = n;
      wk[j][1] = month ();
      wk[j][2] = year ();
      j++;
    }
#endif
    int_to_str (tmp, n);
    days[i]->copy_label (tmp);
    days[i]->down_box (FL_THIN_DOWN_BOX);
    days[i]->box (FL_THIN_UP_BOX);
    days[i]->color (52);
    if ((i-dow+1) == day ()) {
      days[i]->color (selection_color());
    }
    days[i]->callback (fl_calendar_button_cb, this);
    days[i]->show ();
  }

  /* first days of next month */
  for (i = (dim+dow); i < (6*7); i++) {
    n = i+1-dow-dim;
#ifdef ISO_WEEK_NUMBERS
    if (i % 7 == 0) {
      wk[j][0] = n;
      if (month () == DECEMBER) {
        wk[j][1] = JANUARY;
        wk[j][2] = year () + 1;
      } else {
        wk[j][1] = month () + 1;
        wk[j][2] = year ();
      }
      j++;
    }
#endif
    int_to_str (tmp, n);
    days[i]->copy_label (tmp);
    days[i]->down_box (FL_FLAT_BOX);
    days[i]->box (FL_FLAT_BOX);
    days[i]->color (color());
    days[i]->callback (fl_calendar_nxt_month_cb, this);
    days[i]->show ();
  }

#ifdef ISO_WEEK_NUMBERS
  j = 0;
  for (i = 0; i < 6; i++) {
    weeknumbers[i] = iso_week_number(wk[i][2], wk[i][1], wk[i][0]);
    sprintf(tmp, "week %d", weeknumbers[i]);
    for (n = 0; n < 7; n++, j++) {
      days[j]->copy_tooltip (tmp);
    }
  }
#endif
}

Fl_Button *
Fl_Calendar_Base::day_button (int i)
{
  if ((i > 0) && (i <= days_in_month ())) {
    return days[i + day_of_week (year (), month (), 0) - 1];
  }
  return 0;
}

static void
fl_calendar_prv_month_cb (Fl_Widget *, void *b) {
  Fl_Calendar *c = (Fl_Calendar *)b;
  c->previous_month ();
  c->do_callback(c, 0L);
}

static void
fl_calendar_nxt_month_cb (Fl_Widget *, void *b) {
  Fl_Calendar *c = (Fl_Calendar *)b;
  c->next_month ();
  c->do_callback(c, 0L);
}

static void
fl_calendar_prv_year_cb (Fl_Widget *, void *b) {
  Fl_Calendar *c = (Fl_Calendar *)b;
  c->previous_year ();
  c->do_callback(c, 0L);
}

static void
fl_calendar_nxt_year_cb (Fl_Widget *, void *b) {
  Fl_Calendar *c = (Fl_Calendar *)b;
  c->next_year ();
  c->do_callback(c, 0L);
}

Fl_Calendar::Fl_Calendar (int x, int y, int w, int h, const char *l)
  : Fl_Calendar_Base (x, y, w, h, l)
{
  int title_height = h / 8;
  selected_day_ = 0;

  /**
   * If the Calendar width isn't divisible by 7 there will be a gap
   * on the right or left side, so we will distribute this extra space.
   */
  /*
  int oi = (w-(7*(w/7)))/2;
  int of = (w-(7*(w/7))) - oi;
  int xi, wxi;
  */

  for (int i = 0; i < 7; i++) {
    /*
    if (i == 0) {
      xi = 0;
    } else {
      xi = oi;
    }
    if (i == 0) {
      wxi = oi;
    } else if (i == 6) {
      wxi = of;
    } else {
      wxi = 0;
    }
    */
    weekdays[i] = new Fl_Box ((w/7)*(i%7) + x /* + xi */,
                              ((h - title_height)/7)*(i/7) + y + title_height,
                              (w/7) /* + wxi */,
                              (h - title_height)/7);
    weekdays[i]->box (FL_THIN_UP_BOX);
    weekdays[i]->labelsize (labelsize_);
    weekdays[i]->color (color());
  }

  weekdays[MONDAY]->label (fdate_mo);
  weekdays[TUESDAY]->label (fdate_tu);
  weekdays[WEDNESDAY]->label (fdate_we);
  weekdays[THURSDAY]->label (fdate_th);
  weekdays[FRIDAY]->label (fdate_fr);
  weekdays[SATURDAY]->label (fdate_sa);
  weekdays[SUNDAY]->label (fdate_su);

  /*  « MONTH »  */

  prv_month = new Fl_Repeat_Button (x,  /* x - of + w - w */
                                    y,
                                    w/14,
                                    h/8,
                                    "\xc2\xab");
  prv_month->box (FL_FLAT_BOX);
  prv_month->down_box (FL_FLAT_BOX);
  prv_month->labelsize (labelsize_);
  prv_month->callback (fl_calendar_prv_month_cb, this);

  caption_m = new Fl_Box (x + (w/14),  /* x - of + w - (w/7)*6 - (w/14) */
                          y,
                          (w/7)*3,
                          h/8);
  caption_m->box (FL_FLAT_BOX);  /* FL_THIN_UP_BOX */
  caption_m->labelfont (FL_HELVETICA_BOLD);
  caption_m->labelsize (labelsize_);

  nxt_month = new Fl_Repeat_Button (x + (w/2),  /* x - of + w - (w/14)*7 */
                                    y,
                                    w/14,
                                    h/8,
                                    "\xc2\xbb");
  nxt_month->box (FL_FLAT_BOX);
  nxt_month->down_box (FL_FLAT_BOX);
  nxt_month->labelsize (labelsize_);
  nxt_month->callback (fl_calendar_nxt_month_cb, this);

  /*  « YEAR »  */

  prv_year = new Fl_Repeat_Button (x + (w/14)*9,  /* x - of + w - (w/14)*5 */
                                   y,
                                   w/14,
                                   h/8,
                                   "\xc2\xab");
  prv_year->box (FL_FLAT_BOX);
  prv_year->down_box (FL_FLAT_BOX);
  prv_year->labelsize (labelsize_);
  prv_year->callback (fl_calendar_prv_year_cb, this);

  caption_y = new Fl_Box (x + (w/7)*5,  /* x - of + w - (w/14)*3 - (w/14) */
                          y,
                          (w/14)*3,
                          h/8);
  caption_y->box (FL_FLAT_BOX);
  caption_y->labelfont (FL_HELVETICA_BOLD);
  caption_y->labelsize (labelsize_);

  nxt_year = new Fl_Repeat_Button (x + (w/14)*13,  /* x - of + w - (w/14) */
                                   y,
                                   w/14,
                                   h/8,
                                   "\xc2\xbb");
  nxt_year->box (FL_FLAT_BOX);
  nxt_year->down_box (FL_FLAT_BOX);
  nxt_year->labelsize (labelsize_);
  nxt_year->callback (fl_calendar_nxt_year_cb, this);

  Fl_Calendar_Base::csize (x, y + title_height + (h - title_height) / 7,
                           w, h - title_height - (h - title_height) / 7);
  update ();
}

void
Fl_Calendar::update ()
{
  char tmp_m[32];
  char tmp_y[32];
  const char *_month_name[] = {
    fdate_mon_jan,
    fdate_mon_feb,
    fdate_mon_mar,
    fdate_mon_apr,
    fdate_mon_may,
    fdate_mon_jun,
    fdate_mon_jul,
    fdate_mon_aug,
    fdate_mon_sep,
    fdate_mon_oct,
    fdate_mon_nov,
    fdate_mon_dec
  };

  sprintf (tmp_m, "%s", _month_name[month ()-1]);
  //sprintf (tmp_y, "%d", year ());
  int_to_str (tmp_y, year ());

  Fl_Calendar_Base::update ();

  caption_m->copy_label (tmp_m);
  caption_y->copy_label (tmp_y);

  redraw ();
}

void Fl_Calendar::previous_month ()
{
  FDate::previous_month ();
  update ();
}

void
Fl_Calendar::next_month ()
{
  FDate::next_month ();
  update ();
}

void
Fl_Calendar::previous_year ()
{
  FDate::previous_year ();
  update ();
}

void Fl_Calendar::next_year ()
{
  FDate::next_year ();
  update ();
}

int
Fl_Calendar::handle (int event)
{
  int m, d, y, o, md;
  o = 0;

  switch (event) {
    case FL_FOCUS:
    case FL_UNFOCUS:
      return 1;

    case FL_KEYBOARD:
      m = month ();
      d = day ();
      y = year ();
      switch(Fl::event_key ()) {
       /***
        case FL_Enter:
        selected_day_ = d;
        do_callback(this, d);
        break;
        ***/
        case FL_Up:
          o = -7;
          break;
        case FL_Down:
          o = 7;
          break;
        case FL_Right:
          o = 1;
          break;
        case FL_Left:
          o = -1;
          break;
        case FL_Page_Up:
          previous_month ();
          return 1;
        case FL_Page_Down:
          next_month ();
          return 1;
        default:
          return Fl_Group::handle (event);
      }
      if (valid (y, m, d + o)) {
        set_date (y, m, d + o);
      } else {
        if (o < 0) {
          previous_month ();
          m = month ();
          y = year ();
          md = days_in_month (m, leap_year (y));
          d = d + o + md;
          set_date (y, m, d);
        } else {
          next_month ();
          m = month ();
          y = year ();
          md = days_in_month (m, leap_year (y));
          d = d + o - md;
          set_date (y, m, d);
        }
      }
      update ();
      return 1;  /* case FL_KEYBOARD */
  }  /* switch (event) */
  return Fl_Group::handle (event);
}

void
Fl_Calendar::selection_color (Fl_Color c)
{
  Fl_Widget::selection_color(c);
  for (int i = 0; i < (6*7); i++) {
    if (days[i]->color() == days[i]->selection_color()) {
      days[i]->color(c);
    }
    days[i]->selection_color(c);
  }
}

int
Fl_Calendar::selected_day (int d)
{
  selected_day_ = d;
  if (d) day(d);
  return d;
}

Fl_Calendar_Base::~Fl_Calendar_Base ()
{
}

Fl_Calendar::~Fl_Calendar ()
{
}

