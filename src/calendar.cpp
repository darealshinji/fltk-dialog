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
#include <FL/Fl_Window.H>

#include <iostream>  /* std::cout, std::endl */

#include "Flek/Fl_Calendar.H"


int dialog_fl_calendar(char *calendar_title)
{
  if (calendar_title == NULL) {
    calendar_title = (char *)"FLTK calendar";
  }

  Fl_Window win(250, 250);
  Fl_Agenda_Calendar *cal = new Fl_Agenda_Calendar(10, 10, win.w()-20, win.h()-20);
  win.resizable(cal);
  win.label(calendar_title);
  win.end();
  win.show();

  int ret = Fl::run();
  /* custom format; see Flek/FDate.cpp */
  cal->set_format(5);
  std::cout << cal->to_string() << std::endl;
  return ret;
}

