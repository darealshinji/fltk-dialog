/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017, djcj <djcj@gmx.de>
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
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "fltk-dialog.hpp"

static Fl_Double_Window *textinfo_win;
static Fl_Multi_Browser *ti_browser;
static Fl_Return_Button *ti_but_ok;
static bool ti_checkbutton_set = false;
static bool ti_autoscroll = false;

static void textinfo_close_cb(Fl_Widget *, long p)
{
  textinfo_win->hide();
  ret = (int) p;
}

static void ti_checkbutton_cb(Fl_Widget *)
{
  if (ti_checkbutton_set == true)
  {
    ti_checkbutton_set = false;
    ti_but_ok->deactivate();
  }
  else
  {
    ti_checkbutton_set = true;
    ti_but_ok->activate();
  }
}

extern "C" void *ti_getline(void *)
{
  std::string line;
  int linecount;

  for (linecount = 0; std::getline(std::cin, line); /**/)
  {
    Fl::lock();
    ti_browser->add(line.c_str());

    if (ti_autoscroll)
    {
      ++linecount;
      ti_browser->bottomline(linecount);
    }

    Fl::unlock();
    Fl::awake(textinfo_win);
  }

  return nullptr;
}

int dialog_textinfo(bool autoscroll, const char *checkbox)
{
  Fl_Group         *buttongroup;
  Fl_Box           *dummy;
  Fl_Check_Button  *checkbutton;
  Fl_Button        *ti_but_cancel;

  std::string checkbox_s;
  int browser_h;
  pthread_t ti_thread;

  ti_autoscroll = autoscroll;

  if (title == NULL)
  {
    title = "FLTK text info window";
  }

  if (checkbox == NULL)
  {
    browser_h = 444;
  }
  else
  {
    browser_h = 418;
  }

  textinfo_win = new Fl_Double_Window(400, 500, title);
  {
    ti_browser = new Fl_Multi_Browser(10, 10, 380, browser_h);

    buttongroup = new Fl_Group(0, browser_h, 400, 500);
    {
      int but_x = 290;
      int but_y = browser_h + 20;
      if (checkbox == NULL)
      {
        int but_w;

        textinfo_win->callback(textinfo_close_cb, 0);

        ti_but_ok = new Fl_Return_Button(0,0,0,0, fl_close);
        measure_button_width(ti_but_ok, but_w, 40);

        ti_but_ok = new Fl_Return_Button(390 - but_w, but_y, but_w, 26, fl_close);
        ti_but_ok->callback(textinfo_close_cb, 0);
      }
      else
      {
        but_x = 180;
        but_y = browser_h + 10;

        textinfo_win->callback(textinfo_close_cb, 1);

        checkbox_s = " " + std::string(checkbox);
        checkbutton = new Fl_Check_Button(10, but_y + 2, 380, 26, checkbox_s.c_str());
        checkbutton->callback(ti_checkbutton_cb);

        ti_but_ok = new Fl_Return_Button(but_x, but_y + 36, 100, 26, fl_ok);
        ti_but_ok->deactivate();
        ti_but_ok->callback(textinfo_close_cb, 0);
        ti_but_cancel = new Fl_Button(290, but_y + 36, 100, 26, fl_cancel);
        ti_but_cancel->callback(textinfo_close_cb, 1);
      }
      dummy = new Fl_Box(but_x - 1, but_y - 1, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    buttongroup->resizable(dummy);
    buttongroup->end();
  }
  set_size(textinfo_win, ti_browser);
  set_position(textinfo_win);
  textinfo_win->end();

  Fl::lock();

  set_taskbar(textinfo_win);
  textinfo_win->show();
  set_undecorated(textinfo_win);

  pthread_create(&ti_thread, 0, &ti_getline, nullptr);

  Fl::run();

  return ret;
}

