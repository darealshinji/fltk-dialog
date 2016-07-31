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
 *          2016  djcj <djcj@gmx.de> (removed code not used by fltk-dialog)
 */

#include <FL/Fl.H>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <Flek/Fl_Calendar.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Repeat_Button.H>

static void fl_calendar_button_cb (Fl_Button *a, void *b)
{
  long j=0;
  Fl_Calendar *c = (Fl_Calendar *)b;
  Fl_Button   *sb;
  int numdays = c->days_in_month () + 1;
  for (int i=1; i < numdays; i++) {
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
  int i;

  for (i = 0; i<(7*6); i++) {
    days[i] = new Fl_Button ((w/7)*(i%7) + x,
                             (h/6)*(i/7) + y,
                             (w/7),
                             (h/6));
    days[i]->down_box (FL_THIN_DOWN_BOX);
    days[i]->labelsize (10);
    days[i]->box (FL_THIN_UP_BOX);
    days[i]->color (52);
    days[i]->callback ((Fl_Callback*)&fl_calendar_button_cb, (void *)this);
  }
}

void Fl_Calendar_Base::csize (int cx, int cy, int cw, int ch)
{
  int i;
  int oi = (cw-(7*(int)(cw/7)))/2;
  int of = (cw-(7*(int)(cw/7))) - oi;
  int xi, wxi;

  for (i = 0; i<(7*6); i++) {
   if ((i%7)==0) xi = 0;
    else xi = oi;
    if ((i%7)==0) wxi = oi;
    else if ((i%7)==6) wxi = of;
    else wxi = 0;

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

  for (i=0; i<dow; i++) {
    days[i]->hide ();
  }

  for (i=(dim+dow); i<(6*7); i++) {
    days[i]->hide ();
  }

  for (i=dow; i<(dim+dow); i++) {
    char t[8];
    sprintf (t, "%d", (i-dow+1));
    days[i]->label (strdup(t));
    days[i]->color (52);
    if ((i-dow+1) == day ())
      days[i]->color (selection_color());
    days[i]->show ();
  }
}

Fl_Button *
Fl_Calendar_Base::day_button (int i)
{
  if ((i > 0) && (i <= days_in_month ()))
    return days[i + day_of_week (year (), month (), 0) - 1];
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
  int i;
  selected_day_ = 0;

  // If the Calendar width isn't divisible by 7 there will be a gap
  // on the right or left side.
  // So we will distribute this extra space between Sunday and Saturday
  int oi = (w-(7*(int)(w/7)))/2;
  int of = (w-(7*(int)(w/7))) - oi;
  int xi, wxi;

  for (i = 0; i<7; i++) {
    if (i==0) xi = 0;
    else xi = oi;
    if (i==0) wxi = oi;
    else if (i==6) wxi = of;
    else wxi = 0;
    weekdays[i] = new Fl_Box ((w/7)*(i%7) + x + xi,
                              ((h - title_height)/7)*((i/7)) + y + title_height,
                              (w/7) + wxi,
                              ((h - title_height)/7));
    weekdays[i]->box (FL_THIN_UP_BOX);
    weekdays[i]->labelsize (10);
    weekdays[i]->color (52);
  }

  weekdays[MONDAY]->label ("Mo");
  weekdays[TUESDAY]->label ("Tu");
  weekdays[WEDNESDAY]->label ("We");
  weekdays[THURSDAY]->label ("Th");
  weekdays[FRIDAY]->label ("Fr");
  weekdays[SATURDAY]->label ("Sa");
  weekdays[SUNDAY]->label ("Su");

  prv_year = new Fl_Repeat_Button ((x - of + w - (int)(w/7) * 4), y, (w/7), (h/8), "Y-");
  prv_year->box (FL_THIN_UP_BOX);
  prv_year->labelsize (10);
  prv_year->down_box (FL_THIN_DOWN_BOX);
  prv_year->callback ((Fl_Callback*)&fl_calendar_prv_year_cb, (void *)this);

  prv_month = new Fl_Repeat_Button (x - of + w - (int)(w/7) * 3, y, (w/7), (h/8), "M-");
  prv_month->box (FL_THIN_UP_BOX);
  prv_month->labelsize (10);
  prv_month->down_box (FL_THIN_DOWN_BOX);
  prv_month->callback ((Fl_Callback*)&fl_calendar_prv_month_cb, (void *)this);

  nxt_month = new Fl_Repeat_Button (x - of + w - (int)(w/7) * 2, y, (w/7), (h/8), "M+");
  nxt_month->box (FL_THIN_UP_BOX);
  nxt_month->labelsize (10);
  nxt_month->down_box (FL_THIN_DOWN_BOX);
  nxt_month->callback ((Fl_Callback*)&fl_calendar_nxt_month_cb, (void *)this);

  nxt_year = new Fl_Repeat_Button (x - of + w - (int)(w/7) * 1, y, (w/7) + of, (h/8), "Y+");
  nxt_year->box (FL_THIN_UP_BOX);
  nxt_year->labelsize (10);
  nxt_year->down_box (FL_THIN_DOWN_BOX);
  nxt_year->callback ((Fl_Callback*)&fl_calendar_nxt_year_cb, (void *)this);

  caption = new Fl_Box (x, y, (w/7)*3 + oi, (h/8));
  caption->box (FL_THIN_UP_BOX);
  caption->labelfont (FL_HELVETICA_BOLD);
  caption->labelsize (10);

  Fl_Calendar_Base::csize (x, y + title_height + (h - title_height) / 7, w, h - title_height - (h - title_height) / 7);
  update ();
}

void Fl_Calendar::csize (int cx, int cy, int cw, int ch)
{
  int i;
  int oi = (cw-(7*(int)(cw/7)))/2;
  int of = (cw-(7*(int)(cw/7))) - oi;
  int xi, wxi;

  for (i = 0; i<7; i++) {
    if (i==0) xi = 0;
    else xi = oi;
    if (i==0) wxi = oi;
    else if (i==6) wxi = of;
    else wxi = 0;

    weekdays[i] = new Fl_Box ((cw/7)*(i%7) + cx + xi,
                              (ch/8)*((i/7)+1) + cy,
                              (cw/7) + wxi,
                              (ch/8));
  }

  prv_month->resize (cx + (cw/10), cy, (cw/10), (ch/8));
  nxt_month->resize (cx + (cw/10)*8, cy, (cw/10), (ch/8));
  prv_year->resize (cx, cy, (cw/10), (ch/8));
  nxt_year->resize (cx + (cw/10)*9, cy, (cw/10), (ch/8));
  caption->resize (cx + (cw/10)*2, cy, (cw/10)*6, (ch/8));

  Fl_Calendar_Base::csize (cx, cy + (2*ch/8), cw, (6*ch/8));
}

void
Fl_Calendar::update ()
{
  int dow = day_of_week (year (), month (), 1);
  int dim = days_in_month (month (), leap_year (year ()));
  int i;

  for (i=dow; i<(dim+dow); i++) {
    char t[8];
    sprintf (t, "%d", (i-dow+1));
    days[i]->label (strdup(t));
  }

  char tmp[32];
  sprintf (tmp, "%.3s %d", month_name[month ()-1], year ());
  Fl_Calendar_Base::update ();
  if (caption->label ())
    free ((void *) caption->label ());
  caption->label (strdup(tmp));
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
  int m, d, y, o=0, md;

  switch (event) {
  case FL_FOCUS:
  case FL_UNFOCUS:
    return 1;

  case FL_KEYBOARD:
    m = month ();
    d = day ();
    y = year ();
    switch(Fl::event_key ()) {
    /*
    case FL_Enter:
      selected_day_ = d;
      do_callback(this, d);
      break;
     */
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
    if (valid (y, m, d + o))
      set_date (y, m, d + o);
    else  {
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
    return 1;
  }
  return Fl_Group::handle (event);
}
