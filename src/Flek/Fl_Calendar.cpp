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
 *          2016  djcj <djcj@gmx.de> (modified for fltk-dialog)
 */

#include <FL/Fl.H>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <Flek/Fl_Calendar.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Repeat_Button.H>

int calendar_labelsize = 12;

static void fl_calendar_button_cb (Fl_Button *a, void *b)
{
  long j = 0;
  Fl_Calendar *c = (Fl_Calendar *)b;
  Fl_Button   *sb;
  int numdays = c->days_in_month () + 1;
  for (int i = 1; i < numdays; i++) {
    sb = c->day_button(i);
    sb->color (52);
    if (a == sb) {
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
  for (int i = 0; i < (7*6); i++) {
    days[i] = new Fl_Button ((w/7)*(i%7) + x,
                             (h/6)*(i/7) + y,
                             (w/7),
                             (h/6));
    days[i]->labelsize (calendar_labelsize);
    days[i]->down_box (FL_FLAT_BOX);
    days[i]->box (FL_FLAT_BOX);
    days[i]->color (color());
  }
}

void Fl_Calendar_Base::csize (int cx, int cy, int cw, int ch)
{
  int oi = (cw-(7*(int)(cw/7)))/2;
  int of = (cw-(7*(int)(cw/7))) - oi;
  int xi, wxi;

  for (int i = 0; i < (7*6); i++) {
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

    days[i]->resize ((cw/7)*(i%7) + cx + xi,
                     (ch/6)*(i/7) + cy,
                     (cw/7) + wxi,
                     (ch/6));
  }
}

void
Fl_Calendar_Base::update ()
{
  int dow = day_of_week (year (), month (), 0);
  int dim = days_in_month (month (), leap_year (year ()));
  int i;
  char t[8];

  int dipm_month;  /* previous month */
  int dipm_year;   /* year of previous month */
  if (month () == JANUARY) {
    dipm_month = DECEMBER;
    dipm_year = leap_year (year () - 1);
  } else {
    dipm_month = month () - 1;
    dipm_year = leap_year (year ());
  }
  int dipm = days_in_month (dipm_month, dipm_year);

  /* last days of previous month */
  for (i = 0; i < dow; i++) {
    sprintf (t, "%d", (i-dow+dipm+1));
    days[i]->label (strdup(t));
    days[i]->down_box (FL_FLAT_BOX);
    days[i]->box (FL_FLAT_BOX);
    days[i]->color (color());
    days[i]->show ();
  }

  /* current month */
  for (i = dow; i < (dim+dow); i++) {
    sprintf (t, "%d", (i+1-dow));
    days[i]->label (strdup(t));
    days[i]->down_box (FL_THIN_DOWN_BOX);
    days[i]->box (FL_THIN_UP_BOX);
    days[i]->color (52);
    if ((i-dow+1) == day ()) {
      days[i]->color (selection_color());
    }
    days[i]->callback ((Fl_Callback*)&fl_calendar_button_cb, (void *)this);
    days[i]->show ();
  }

  /* first days of next month */
  for (i = (dim+dow); i < (6*7); i++) {
    sprintf (t, "%d", (i+1-dow-dim));
    days[i]->label (strdup(t));
    days[i]->down_box (FL_FLAT_BOX);
    days[i]->box (FL_FLAT_BOX);
    days[i]->color (color());
    days[i]->show ();
  }
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
fl_calendar_prv_month_cb (Fl_Button *, void *b) {
  Fl_Calendar *c = (Fl_Calendar *)b;
  c->previous_month ();
  c->do_callback(c, (long)0);
}

static void
fl_calendar_nxt_month_cb (Fl_Button *, void *b) {
  Fl_Calendar *c = (Fl_Calendar *)b;
  c->next_month ();
  c->do_callback(c, (long)0);
}

static void
fl_calendar_prv_year_cb (Fl_Button *, void *b) {
  Fl_Calendar *c = (Fl_Calendar *)b;
  c->previous_year ();
  c->do_callback(c, (long)0);
}

static void
fl_calendar_nxt_year_cb (Fl_Button *, void *b) {
  Fl_Calendar *c = (Fl_Calendar *)b;
  c->next_year ();
  c->do_callback(c, (long)0);
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
  int oi = (w-(7*(int)(w/7)))/2;
  int of = (w-(7*(int)(w/7))) - oi;
  int xi, wxi;

  for (int i = 0; i < 7; i++) {
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
    weekdays[i] = new Fl_Box ((w/7)*(i%7) + x + xi,
                              ((h - title_height)/7)*((i/7)) + y + title_height,
                              (w/7) + wxi,
                              ((h - title_height)/7));
    weekdays[i]->box (FL_THIN_UP_BOX);
    weekdays[i]->labelsize (calendar_labelsize);
    weekdays[i]->color (color());
  }

  weekdays[MONDAY]->label ("Mo");
  weekdays[TUESDAY]->label ("Tu");
  weekdays[WEDNESDAY]->label ("We");
  weekdays[THURSDAY]->label ("Th");
  weekdays[FRIDAY]->label ("Fr");
  weekdays[SATURDAY]->label ("Sa");
  weekdays[SUNDAY]->label ("Su");

  /*  « MONTH »  */

  prv_month = new Fl_Repeat_Button (x,  /* x - of + w - w */
                                    y,
                                    w/14,
                                    h/8,
                                    "\xc2\xab");  /* http://www.utf8-chartable.de/ */
  prv_month->box (FL_FLAT_BOX);
  prv_month->down_box (FL_FLAT_BOX);
  prv_month->labelsize (calendar_labelsize);
  prv_month->callback ((Fl_Callback*)&fl_calendar_prv_month_cb, (void *)this);

  caption_m = new Fl_Box (x + (w/14),  /* x - of + w - (w/7)*6 - (w/14) */
                          y,
                          (w/7)*3,
                          h/8);
  caption_m->box (FL_FLAT_BOX);  /* FL_THIN_UP_BOX */
  caption_m->labelfont (FL_HELVETICA_BOLD);
  caption_m->labelsize (calendar_labelsize);

  nxt_month = new Fl_Repeat_Button (x + (w/2),  /* x - of + w - (w/14)*7 */
                                    y,
                                    w/14,
                                    h/8,
                                    "\xc2\xbb");
  nxt_month->box (FL_FLAT_BOX);
  nxt_month->down_box (FL_FLAT_BOX);
  nxt_month->labelsize (calendar_labelsize);
  nxt_month->callback ((Fl_Callback*)&fl_calendar_nxt_month_cb, (void *)this);

  /*  « YEAR »  */

  prv_year = new Fl_Repeat_Button (x + (w/14)*9,  /* x - of + w - (w/14)*5 */
                                   y,
                                   w/14,
                                   h/8,
                                   "\xc2\xab");
  prv_year->box (FL_FLAT_BOX);
  prv_year->down_box (FL_FLAT_BOX);
  prv_year->labelsize (calendar_labelsize);
  prv_year->callback ((Fl_Callback*)&fl_calendar_prv_year_cb, (void *)this);

  caption_y = new Fl_Box (x + (w/7)*5,  /* x - of + w - (w/14)*3 - (w/14) */
                          y,
                          (w/14)*3,
                          h/8);
  caption_y->box (FL_FLAT_BOX);
  caption_y->labelfont (FL_HELVETICA_BOLD);
  caption_y->labelsize (calendar_labelsize);

  nxt_year = new Fl_Repeat_Button (x + (w/14)*13,  /* x - of + w - (w/14) */
                                   y,
                                   w/14,
                                   h/8,
                                   "\xc2\xbb");
  nxt_year->box (FL_FLAT_BOX);
  nxt_year->down_box (FL_FLAT_BOX);
  nxt_year->labelsize (calendar_labelsize);
  nxt_year->callback ((Fl_Callback*)&fl_calendar_nxt_year_cb, (void *)this);

  Fl_Calendar_Base::csize (x, y + title_height + (h - title_height) / 7,
                           w, h - title_height - (h - title_height) / 7);
  update ();
}

void
Fl_Calendar::update ()
{
  char tmp_m[32];
  char tmp_y[32];

  sprintf (tmp_m, "%s", month_name[month ()-1]);
  sprintf (tmp_y, "%d", year ());

  Fl_Calendar_Base::update ();

  if (caption_m->label ()) {
    free ((void *) caption_m->label ());
  }
  caption_m->label (strdup(tmp_m));

  if (caption_y->label ()) {
    free ((void *) caption_y->label ());
  }
  caption_y->label (strdup(tmp_y));

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
