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

#include <iostream>  /* std::cout, std::endl */

#include "Flek/Fl_Calendar.H"


static void calendar_ok_cb(Fl_Widget *w)
{
  w->window()->hide();
  return;
}

static void calendar_cancel_cb(Fl_Widget*)
{
  exit(1);
}

int dialog_fl_calendar(char *calendar_title)
{
  int winw = 250;
  int calh = winw;
  int bord = 10;
  int butw = 110;  /* instead of 100; it's symmetrical */
  int buth = 26;
  int winh = calh+buth+bord;

  if (calendar_title == NULL) {
    calendar_title = (char *)"FLTK calendar";
  }

  Fl_Window win(winw, winh, calendar_title);
  Fl_Agenda_Calendar *cal = new Fl_Agenda_Calendar(bord, bord,
                                                   winw-bord*2,
                                                   calh-bord*2);
  win.begin();
  Fl_Return_Button *but_ok = new Fl_Return_Button(winw-butw*2-bord*2,
                                                  calh, butw, buth,
                                                  fl_ok);
  but_ok->callback(calendar_ok_cb);
  Fl_Button *but_cancel = new Fl_Button(winw-butw-bord, calh,
                                        butw, buth, fl_cancel);
  but_cancel->callback(calendar_cancel_cb);
  win.end();
  win.show();

  int ret = Fl::run();
  /* custom format; see Flek/FDate.cpp */
  cal->set_format(5);
  std::cout << cal->to_string() << std::endl;
  return ret;
}

