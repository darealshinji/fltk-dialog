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

/***

(echo "Line 1/5"; n=2; \
 while (test $n -le 5); do \
 sleep 1; echo "Line $n/5"; n=$(($((n))+1)); done \
) | \
 ./fltk-dialog --text-info --auto-scroll --checkbox="I confirm" --no-system-colors

***/

static Fl_Double_Window *win;
static Fl_Multi_Browser *browser;
static Fl_Check_Button *checkbutton = NULL;
static Fl_Return_Button *but_ret;
static bool checkbutton_set, autoscroll;
static int ret = 1;

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = p;
}

static void callback(Fl_Widget *)
{
  if (checkbutton_set) {
    checkbutton_set = false;
    but_ret->deactivate();
  } else {
    checkbutton_set = true;
    but_ret->activate();
  }
}

extern "C" void *ti_getline(void *)
{
  std::string line;

  for (int i = 0; std::getline(std::cin, line); ) {
    Fl::lock();

#ifdef WITH_FRIBIDI
    char *tmp = NULL;
    if (use_fribidi) {
      tmp = fribidi_parse_line(line.c_str());
    }
    if (tmp) {
      browser->add(tmp);
      delete tmp;
    } else
#endif
    {
      browser->add(line.c_str());
    }

    if (autoscroll) {
      i++;
      browser->bottomline(i);
    }

    Fl::unlock();
    Fl::awake(win);
  }

  if (checkbutton) {
    Fl::lock();
    checkbutton->activate();
    Fl::unlock();
  }
  Fl::awake(win);

  return nullptr;
}

int dialog_textinfo(bool autoscroll_, const char *checkbox)
{
  Fl_Group *g;
  Fl_Box *dummy;
  Fl_Button *but;
  int browser_h = checkbox ? 418 : 444;
  int but_y = browser_h + 20;
  int but_w, range;
  pthread_t t;

  autoscroll = autoscroll_;

  if (!title) {
    title = "FLTK text info window";
  }

  win = new Fl_Double_Window(400, 500, title);
  {
    browser = new Fl_Multi_Browser(10, 10, 380, browser_h);

    g = new Fl_Group(0, browser_h, 400, 500);
    {
      if (!checkbox) {
        win->callback(close_cb, 0);
        range = but_w = measure_button_width(fl_close, 40);
        but_ret = new Fl_Return_Button(win->w() - 10 - but_w, but_y, but_w, 26, fl_close);
        but_ret->callback(close_cb, 0);
      } else {
        win->callback(close_cb, 1);
        but_y = browser_h + 10;

        std::string s = " ";
        checkbutton = new Fl_Check_Button(10, but_y + 2, 380, 26);
        checkbutton->callback(callback);
#ifdef WITH_FRIBIDI
        char *tmp = NULL;
        if (use_fribidi) {
          tmp = fribidi_parse_line(checkbox);
        }
        if (tmp) {
          s.append(tmp);
          delete tmp;
        } else
#endif
        {
          s.append(checkbox);
        }
        checkbutton->copy_label(s.c_str());
        checkbutton->deactivate();
        checkbutton_set = false;

        but_w = measure_button_width(fl_cancel, 20);
        range = but_w + 40;
        but = new Fl_Button(win->w() - 10 - but_w, but_y + 36, but_w, 26, fl_cancel);
        but->callback(close_cb, 1);

        but_w = measure_button_width(fl_ok, 40);
        but_ret = new Fl_Return_Button(but->x() - 10 - but_w, but_y + 36, but_w, 26, fl_ok);
        but_ret->callback(close_cb, 0);
        but_ret->deactivate();
      }
      dummy = new Fl_Box(but_ret->x() - 1, but_y - 1, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    g->resizable(dummy);
    g->end();
  }
  set_size(win, browser);
  set_size_range(win, range, checkbox ? 120 : 90);
  set_position(win);
  win->end();

  Fl::lock();

  set_taskbar(win);
  win->show();
  set_undecorated(win);
  set_always_on_top(win);

  pthread_create(&t, 0, &ti_getline, NULL);

  Fl::run();

  return ret;
}

