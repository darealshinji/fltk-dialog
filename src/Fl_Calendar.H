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

#ifndef _FL_CALENDAR_H_
#define _FL_CALENDAR_H_

#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Input.H>

#include <FDate.H>

/** @package libflek_ui
 * Fl_Calendar_Base is a widget that allows a user to select a date
 * (day, month, and year) with a wall style calendar.
 */
class Fl_Calendar_Base : public Fl_Group, public FDate {

public:
  /**
   * The constructor for an empty Fl_Calendar_Base.
   */
  Fl_Calendar_Base (int x, int y, int w = (7*20), int h = (6*20), const char *l = 0);

  void update ();

  /**
   * Resize the buttons contained in Fl_Calendar_Base to
   * fit in the passed dimensions.
   *
   * @param cx The calendar x position.
   * @param cy The calendar y position.
   * @param cw The calendar width.
   * @param ch The calendar height.
   */
  void csize (int cx, int cy, int cw, int ch);

  /**
   * Returns a pointer to the button for the day D in the Fl_Calendar.
   */
  Fl_Button * day_button (int i);

 protected:

  Fl_Button * days[7*6];
  Fl_Box * weeknumbers[6];

  ~Fl_Calendar_Base();

};

/**
 * Fl_Calendar is a subclass of Fl_Calendar.  This widget adds
 * day labels and controllers to the base calendar class.
 * <p><img src="Fl_Calendar.png">
 */
class Fl_Calendar : public Fl_Calendar_Base
{

 public:

  /**
   * The constructor for an empty Fl_Calendar.
   */
  Fl_Calendar (int x, int y, int w = (7*20), int h = (8*20), const char *l = 0);

  /**
   * Sets the value of the widget back by one month.
   */
  void previous_month ();

  /**
   * Sets the value of the widget forward by one month.
   */
  void next_month ();

  /**
   * Sets the value of the widget back by one year.
   */
  void previous_year ();

  /**
   * Sets the value of the widget forward by one year.
   */
  void next_year ();

  /**
   * Updates the widget after values have been changed.
   */
  void update ();

  /**
   * Resize the buttons contained in Fl_Calendar_Base to
   * fit in the passed dimensions.
   *
   * @param cx The calendar x position.
   * @param cy The calendar y position.
   * @param cw The calendar width.
   * @param ch The calendar height.
   */
  void csize (int cx, int cy, int cw, int ch);
  int  handle (int);

  /**
   * Sets the Calendar's selection color.
   *
   * @param color The calendar's selection color.
   */
  //void selection_color (Fl_Color c);

  /**
   * Gets the Calendar's selection color.
   *
   * @return The calendar's selection color.
   */
  //Fl_Color selection_color() const {
  //  return Fl_Group::selection_color();
  //}

  /**
   * Gets the user selected day.  This method differs
   * from the day() method in that if a user has not
   * selected a day the return value is zero.
   *
   * @return The selected day if the user has selected a day, 0 otherwise.
   */
  int selected_day() {
    return selected_day_;
  }

  /**
   * Sets the user selected day.  If d is non zero, this
   * method also sets the day.
   *
   * @return The selected day if the user has selected a day, 0 otherwise.
   */
  int selected_day(int d);

  /**
   * Set non-zero to enable Eastern Arabic numbers.
   */
  void arabic (int i);
  int arabic ();

 protected:

  Fl_Box * weekdays[7];
  Fl_Box * caption_m;
  Fl_Box * caption_y;
  Fl_Repeat_Button * nxt_month;
  Fl_Repeat_Button * prv_month;
  Fl_Repeat_Button * nxt_year;
  Fl_Repeat_Button * prv_year;
  int selected_day_;

  ~Fl_Calendar();

};

#endif
