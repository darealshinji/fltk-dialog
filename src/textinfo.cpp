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

// (while (true); do echo hello; done) | ./fltk-dialog --text-info --auto-scroll

static Fl_Double_Window *win;
static Fl_Multi_Browser *browser;
static Fl_Return_Button *but_ok;
static bool checkbutton_set = false;

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = (int) p;
}

static void callback(Fl_Widget *) {
  if (checkbutton_set) {
    checkbutton_set = false;
    but_ok->deactivate();
  } else {
    checkbutton_set = true;
    but_ok->activate();
  }
}

extern "C" void *ti_getline(void *p)
{
  std::string line;
  for (int i = 0; std::getline(std::cin, line); ) {
    Fl::lock();
    browser->add(line.c_str());
    if (p) {
      ++i;
      browser->bottomline(i);
    }
    Fl::unlock();
    Fl::awake(win);
  }
  return nullptr;
}

int dialog_textinfo(bool autoscroll, const char *checkbox)
{
  Fl_Group         *g;
  Fl_Box           *dummy;
  Fl_Check_Button  *checkbutton;
  Fl_Button        *but_cancel;

  if (!title) {
    title = "FLTK text info window";
  }

  int browser_h = checkbox ? 418 : 444;

  win = new Fl_Double_Window(400, 500, title);
  {
    browser = new Fl_Multi_Browser(10, 10, 380, browser_h);

    g = new Fl_Group(0, browser_h, 400, 500);
    {
      int but_x = 290;
      int but_y = browser_h + 20;
      if (!checkbox) {
        int but_w;
        win->callback(close_cb, 0);
        but_ok = new Fl_Return_Button(0,0,0,0, fl_close);
        measure_button_width(but_ok, but_w, 40);
        but_ok = new Fl_Return_Button(390 - but_w, but_y, but_w, 26, fl_close);
        but_ok->callback(close_cb, 0);
      } else {
        but_x = 180;
        but_y = browser_h + 10;

        win->callback(close_cb, 1);

        std::string s = " " + std::string(checkbox);
        checkbutton = new Fl_Check_Button(10, but_y + 2, 380, 26, s.c_str());
        checkbutton->callback(callback);

        but_ok = new Fl_Return_Button(but_x, but_y + 36, 100, 26, fl_ok);
        but_ok->deactivate();
        but_ok->callback(close_cb, 0);
        but_cancel = new Fl_Button(290, but_y + 36, 100, 26, fl_cancel);
        but_cancel->callback(close_cb, 1);
      }
      dummy = new Fl_Box(but_x - 1, but_y - 1, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    g->resizable(dummy);
    g->end();
  }
  set_size(win, browser);
  set_position(win);
  win->end();

  Fl::lock();

  set_taskbar(win);
  win->show();
  set_undecorated(win);
  set_always_on_top(win);

  pthread_t t;
  pthread_create(&t, 0, &ti_getline, (void *)autoscroll);

  Fl::run();

  return ret;
}

