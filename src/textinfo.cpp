/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020, djcj <djcj@gmx.de>
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

#include <iostream>
#include <errno.h>
#include <stdio.h>
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
 ./build/fltk_dialog/fltk-dialog --text-info --auto-scroll --checkbox="I confirm"

***/

static Fl_Double_Window *win;
static Fl_Multi_Browser *browser;
static Fl_Check_Button *checkbutton = NULL;
static Fl_Return_Button *but_ok;
static int ret = 1;
static pthread_t th;

static bool checkbutton_set = false
,           autoscroll = false
,           autoclose = false
,           hide_cancel = false;

static void close_cb(Fl_Widget *, long p)
{
  pthread_cancel(th);
  win->hide();
  ret = p;
}

static void callback(Fl_Widget *)
{
  if (checkbutton_set) {
    checkbutton_set = false;
    but_ok->deactivate();
  } else {
    checkbutton_set = true;
    but_ok->activate();
  }
}

extern "C" void *ti_getline(void *)
{
  char *line = NULL;
  size_t len = 0;
  int i = 0;

  while (getline(&line, &len, stdin) != -1) {
    Fl::lock();
    i++;
    browser->add(line);
    if (autoscroll) {
      browser->bottomline(i);
    }
    Fl::unlock();
    Fl::awake();
  }

  if (line) {
    free(line);
  }

  Fl::lock();

  if (autoclose) {
    close_cb(NULL, 0);
  }

  if (checkbutton) {
    checkbutton->activate();
  } else {
    but_ok->activate();
  }

  Fl::unlock();
  Fl::awake();

  return nullptr;
}

int dialog_textinfo(bool autoscroll_, const char *checkbox, bool autoclose_, bool hide_cancel_)
{
  Fl_Group *g;
  Fl_Box *dummy;
  Fl_Button *but_cancel;
  int browser_h = checkbox ? 422 : 444;
  int but_w = 90, win_ret = 0;

  autoscroll = autoscroll_;
  autoclose = autoclose_;
  hide_cancel = hide_cancel_;

  if (!title) {
    title = "FLTK text info window";
  }

  win = new Fl_Double_Window(400, 500, title);
  {
    browser = new Fl_Multi_Browser(10, 10, 380, browser_h);

    if (checkbox || !autoclose || !hide_cancel) {
      win_ret = 1;

      g = new Fl_Group(0, browser_h, 400, 500);
      {
        int but_x = win->w();

        if (checkbox) {
          std::string s = std::string{" "} + checkbox;
          checkbutton = new Fl_Check_Button(10, browser_h + 12, 380, 26);
          checkbutton->callback(callback);
          checkbutton->copy_label(s.c_str());
          checkbutton->deactivate();
          checkbutton_set = false;
        }

        if (!hide_cancel) {
          but_w = measure_button_width(fl_cancel, 20);
          but_cancel = new Fl_Button(win->w() - 10 - but_w, win->h() - 36, but_w, 26, fl_cancel);
          but_cancel->callback(close_cb, 1);
          but_x = but_cancel->x() - 1;
        }

        if (!autoclose) {
          const char *l = (!checkbox && hide_cancel) ? fl_close : fl_ok;
          but_w = measure_button_width(l, 40);
          but_ok = new Fl_Return_Button(but_x - 10 - but_w, win->h() - 36, but_w, 26, l);
          but_ok->callback(close_cb, 0);
          but_ok->deactivate();
          but_x = but_ok->x() - 1;
        }

        dummy = new Fl_Box(but_x, browser_h + 10, 1, 1);
        dummy->box(FL_NO_BOX);
      }
      g->resizable(dummy);
      g->end();
    }
  }
  set_size(win, browser);
  set_size_range(win, but_w + 40, checkbox ? 120 : 90);
  set_position(win);
  win->end();
  win->callback(close_cb, win_ret);

  Fl::lock();

  int errsv = pthread_create(&th, 0, &ti_getline, NULL);

  if (errsv != 0) {
    errno = errsv;
    perror("pthread_create()");
    return 1;
  }

  set_taskbar(win);
  win->show();
  set_undecorated(win);
  set_always_on_top(win);

  Fl::run();

  return ret;
}

