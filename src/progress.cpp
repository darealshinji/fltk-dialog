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

// Tests:
// (echo 40; sleep 1; echo 80; sleep 1; echo 99; sleep 1; echo '#Done' && echo '100') | ./fltk-dialog --progress
// (echo '#Work in progress... ' && sleep 4 && echo '#Work in progress... done.' && echo 'STOP') | ./fltk-dialog --progress --pulsate

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Double_Window.H>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "fltk-dialog.hpp"


#define PULSATE_USLEEP 10000

static Fl_Double_Window *progress_win = NULL;
static Fl_Box           *progress_box = NULL;
static Fl_Return_Button *progress_but_ok = NULL;
static Fl_Button        *progress_but_cancel = NULL;
static Fl_Slider        *slider = NULL;
static Fl_Progress      *progress_bar = NULL;

static int pulsate_val = 0;
static int progress_percent = 0;
static bool progress_pulsate = false;
static bool progress_running = true;
static bool progress_autoclose = false;
static bool progress_hide_cancel = false;
static long progress_kill_pid = -1;
static char *progress_box_label = NULL;

static void progress_close_cb(Fl_Widget *, long p)
{
  progress_win->hide();
  ret = (int) p;

  if (progress_box_label)
  {
    free(progress_box_label);
  }
}

static void progress_cancel_cb(Fl_Widget *o)
{
  if (progress_kill_pid > 1)
  {
    kill((pid_t) progress_kill_pid, 1);
  }
  progress_close_cb(o, 1);
}

static void progress_finished(void)
{
  if (progress_autoclose)
  {
    progress_close_cb(nullptr, 0);
  }
  else
  {
    if (!progress_hide_cancel)
    {
      progress_but_cancel->deactivate();
    }
    progress_but_ok->activate();
  }

  if (progress_pulsate)
  {
    slider->value(100);
    slider->deactivate();
  }
}

static void progress_parse_line(const char *ch)
{
  if (progress_running)
  {
    if (ch[0] == '#' && ch[1] != '\0')
    {
      if (progress_box_label)
      {
        free(progress_box_label);
      }
      progress_box_label = strdup(ch + 1);
      progress_box->label(progress_box_label);
    }
    else if (!progress_pulsate && ch[0] >= '0' && ch[0] <= '9')
    {
      std::stringstream ss;
      std::string l;
      progress_percent = atoi(ch);

      if (progress_percent >= 100)
      {
        progress_percent = 100;
        progress_running = false;
      }

      ss << progress_percent;
      l = ss.str() + "%";
      progress_bar->value(progress_percent);
      progress_bar->label(l.c_str());
    }
    else if (progress_pulsate && strcmp(ch, "STOP") == 0)
    {
      progress_running = false;
    }
  }

  if (!progress_running)
  {
    progress_finished();
  }

  Fl::redraw();
}

extern "C" void *progress_getline(void *)
{
  std::fstream fs;
  std::string line;

  for (/**/; std::getline(std::cin, line); /**/)
  {
    Fl::lock();
    progress_parse_line(line.c_str());
    Fl::unlock();
    Fl::awake(progress_win);
  }

  return nullptr;
}

extern "C" void *pulsate_bar_thread(void *)
{
  while (progress_running)
  {
    if (progress_percent >= 100)
    {
      pulsate_val = -1;
    }
    else if (progress_percent <= 0)
    {
      pulsate_val = 1;
    }
    progress_percent += pulsate_val;

    Fl::lock();
    slider->value(progress_percent);

    if (progress_kill_pid > 1 && kill((pid_t) progress_kill_pid, 0) == -1)
    {
      progress_running = false;
    }

    Fl::unlock();
    Fl::awake(progress_win);

    usleep(PULSATE_USLEEP);
  }

  if (!progress_running)
  {
    progress_finished();
    Fl::redraw();
  }

  return nullptr;
}

int dialog_progress(bool pulsate, long kill_pid, bool autoclose, bool hide_cancel)
{
  Fl_Group  *g;
  Fl_Box    *dummy;
  int        win_h = 140;
  pthread_t  progress_thread_1, progress_thread_2;

  if (title == NULL)
  {
    title = "FLTK progress window";
  }

  if (msg == NULL)
  {
    msg = "Progress indicator";
  }

  progress_pulsate = pulsate;
  progress_kill_pid = kill_pid;
  progress_autoclose = autoclose;
  progress_hide_cancel = hide_cancel;

  if (hide_cancel && autoclose)
  {
    win_h -= 36;
  }

  progress_win = new Fl_Double_Window(320, win_h, title);
  progress_win->callback(progress_cancel_cb);
  {
    g = new Fl_Group(0, 0, 320, win_h);
    {
      progress_box = new Fl_Box(0, 10, 10, 30, msg);
      progress_box->box(FL_NO_BOX);
      progress_box->align(FL_ALIGN_RIGHT);

      if (pulsate)
      {
        slider = new Fl_Slider(10, 50, 300, 30);
        slider->type(1);
        slider->minimum(0);
        slider->maximum(100);
        slider->color(fl_darker(FL_GRAY));
        slider->value(0);
        slider->slider_size(0.25);
      }
      else
      {
        progress_bar = new Fl_Progress(10, 50, 300, 30, "0%");
        progress_bar->minimum(0);
        progress_bar->maximum(100);
        progress_bar->color(fl_darker(FL_GRAY));
        progress_bar->selection_color(fl_lighter(FL_BLUE));
        progress_bar->labelcolor(FL_WHITE);
        progress_bar->value(0);
      }

      int but_ok_x = 210;

      if (!hide_cancel)
      {
        progress_but_cancel = new Fl_Button(210, 104, 100, 26, fl_cancel);
        progress_but_cancel->callback(progress_cancel_cb);
        but_ok_x = 100;
      }

      if (!autoclose)
      {
        progress_but_ok = new Fl_Return_Button(but_ok_x, 104, 100, 26, fl_ok);
        progress_but_ok->deactivate();
        progress_but_ok->callback(progress_close_cb, 0);
      }

      dummy = new Fl_Box(but_ok_x - 1, 103, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    g->resizable(dummy);
    g->end();
  }
  set_size(progress_win, g);
  set_position(progress_win);
  progress_win->end();

  Fl::lock();

  set_taskbar(progress_win);
  progress_win->show();
  set_undecorated(progress_win);

  if (pulsate)
  {
    pthread_create(&progress_thread_1, 0, &pulsate_bar_thread, nullptr);
  }
  pthread_create(&progress_thread_2, 0, &progress_getline, nullptr);

  Fl::run();

  return ret;
}

