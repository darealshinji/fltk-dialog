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
 * Authors: David Freese <dfreese@intrepid.net>
 *          2016  djcj <djcj@gmx.de> (modified for fltk-dialog)
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <FDate.H>

const int FDate::days[] =
  { 0, 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30, 31 };

const int FDate::julian_days[2][13] = {
  { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
  { 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
};

const char *fdate_mo = "Mo";
const char *fdate_tu = "Tu";
const char *fdate_we = "We";
const char *fdate_th = "Th";
const char *fdate_fr = "Fr";
const char *fdate_sa = "Sa";
const char *fdate_su = "Su";

const char *fdate_mon_jan = "January";
const char *fdate_mon_feb = "Febuary";
const char *fdate_mon_mar = "March";
const char *fdate_mon_apr = "April";
const char *fdate_mon_may = "May";
const char *fdate_mon_jun = "June";
const char *fdate_mon_jul = "July";
const char *fdate_mon_aug = "August";
const char *fdate_mon_sep = "September";
const char *fdate_mon_oct = "October";
const char *fdate_mon_nov = "November";
const char *fdate_mon_dec = "December";

const char *FDate::month_name[] = {
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

void FDate::today () {
  time_t t;
  struct tm *now;
  time (&t);
  now = localtime (&t);
  Year = now->tm_year + 1900;
  Month = now->tm_mon + 1;
  Day = now->tm_mday;
}

FDate::FDate () {
  today ();
}

FDate::FDate (const FDate &dt) : FBase (dt) {
  set_date (dt);
}

FDate::FDate (int y, int m, int d) {
  set_date (y, m, d);
}

FBase::Ptr FDate::copy (void) const {
  return new FDate (*this);
}

void FDate::set_date (int y, int m, int d) {
  if (valid (y, m, d)) {
    Year = y;
    Month = m;
    Day = d;
  } else {
    today ();
  }
}

void FDate::set_date (const FDate &dt) {
  Year = dt.Year;
  Month = dt.Month;
  Day = dt.Day;
}

void FDate::year (int y) {
  Year = y;
}

int FDate::year () {
  return Year;
}

void FDate::month (int m) {
  Month = m;
}

int FDate::month () {
  return Month;
}

void FDate::day (int d) {
  Day = d;
}

int FDate::day () {
  return Day;
}

bool FDate::leap_year (int y) {
  if (y % 400 == 0 || ( y % 100 != 0 && y % 4 == 0 )) {
    return true;
  }
  return false;
}

bool FDate::valid (int y, int m, int d) {
  if (y < 1970 || y > 2035) { return false; }
  if (m < 1 || m > 12) { return false; }
  if (d < 1 ) { return false; }
  if (leap_year (y)) {
    if ((m == 2) && (d > 29)) {
      return false;
    } else {
      return true;
    }
  }
  if (d > days[m]) {
    return false;
  }
  return true;
}

bool FDate::end_of_month (int d) {
  if (Month == 2 && leap_year (Year)) {
    return (d == 29);  /* last day of Feb in leap year */
  } else {
    return (d == days[Month]);
  }
}

void FDate::help_increment () {
  if (end_of_month (Day) && Month == 12) {  /* end year */
    Day = 1;
    Month = 1;
    ++Year;
  } else if (end_of_month (Day)) {
    Day = 1;
    ++Month;
  } else {
    ++Day;
  }
}

FDate &FDate::operator++ () {
  help_increment ();
  return *this;  /* reference return to create an lvalue */
}

FDate FDate::operator++ (int) {
  FDate temp = *this;
  help_increment ();
  return temp;  /* return non-increment, saved temporary object */
}

const FDate &FDate::operator+= (int ndays) {
  for (int i = 0; i < ndays; i++) {
    help_increment();
  }
  return *this;  /* enables cascading */
}

bool FDate::operator== (const FDate &d) {
  if (this->Year != d.Year) { return false; }
  if (this->Month != d.Month) { return false; }
  if (this->Day != d.Day) { return false; }
  return true;
}

bool FDate::operator!= (const FDate &d) {
  return (!(*this == d));
}

bool FDate::operator< (const FDate &d) {
  if (this->Year < d.Year) { return true; }
  if (this->Year > d.Year) { return false; }
  if (this->Month < d.Month) { return true; }
  if (this->Month > d.Month) { return false; }
  if (this->Day < d.Day) { return true; }
  return false;
}

bool FDate::operator> (const FDate &d) {
  if (*this < d) { return false; }
  if (*this == d) { return false; }
  return true;
}

void FDate::operator= (const FDate &d) {
  this->Year = d.Year;
  this->Month = d.Month;
  this->Day = d.Day;
}

void FDate::next_month () {
  if (Month == 12) {
    Month = 1;
    Year++;
  } else {
    Month++;
  }

  while (Day > days_in_month (Month, leap_year (Year))) {
    Day--;
  }
}

void FDate::previous_month () {
  if (Month == 1) {
    Month = 12;
    Year--;
  } else {
    Month--;
  }

  while (Day > days_in_month (Month, leap_year (Year))) {
    Day--;
  }
}

void FDate::previous_year () {
  if (Month == 2 && Day == 29) {
    Day = 28;
  }
  Year--;
}

void FDate::next_year () {
  if (Month == 2 && Day == 29) {
    Day = 28;
  }
  Year++;
}

int FDate::days_in_month (int month, int leap) {
  /* Validate the month. */
  if (month < JANUARY || month > DECEMBER) {
    return -1;
  }

  /* Return 28, 29, 30, or 31 based on month/leap. */
  if (month == FEBRUARY) {
    return leap ? 29 : 28;
  }
  return days[month];
}

int FDate::day_of_year (int year, int mon, int mday) {
  /* Return day of year. */
  return mday + julian_days[leap_year (year) ? 1 : 0][mon];
}

int FDate::day_of_epoch (int year, int mon, int mday) {
  int doe, era, cent, quad, rest;

  /* break down the year into 400, 100, 4, and 1 year multiples */
  rest = year - 1;

  quad = rest / 4;
  rest %= 4;

  cent = quad / 25;
  quad %= 25;

  era = cent / 4;
  cent %= 4;

  /* set up doe */
  doe = day_of_year (year, mon, mday);
  doe += era * (400 * 365 + 97);
  doe += cent * (100 * 365 + 24);
  doe += quad * (4 * 365 + 1);
  doe += rest * 365;

  return doe;
}

int FDate::day_of_week (int year, int mon, int mday) {
  return day_of_epoch (year, mon, mday) % 7;
}
