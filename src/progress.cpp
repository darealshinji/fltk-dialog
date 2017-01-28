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

/*****

# usage example 1:
( \
  sleep 3 && echo '5#Working on it...' && \
  sleep 1 && echo 30 && \
  sleep 2 && echo 91 && \
  sleep 1 && echo '100#Done.' \
) \
>/tmp/log & pid=$!
./fltk-dialog --progress --watch-file=/tmp/log --watch-pid=$pid

# usage example 2:
sleep 5 & pid=$!; ./fltk-dialog --progress --pulsate --watch-pid=$pid

*****/

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Double_Window.H>

#include <string>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "fltk-dialog.hpp"

#define FGETS_LIMIT 256

static Fl_Double_Window *progress_win;
static Fl_Box           *progress_box;
static Fl_Return_Button *progress_but_ok;
static Fl_Button        *progress_but_cancel;
static Fl_Slider        *slider;
static Fl_Progress      *progress_bar;

static int pulsate_val = 0;
static int progress_percent = 0;
static double interval_sec = 0.01; /* 10ms */
static bool progress_running = true;

static void progress_close_cb(Fl_Widget *, long p)
{
  progress_win->hide();
  ret = (int) p;
}

static void progress_cancel_cb(Fl_Widget *o)
{
  if (kill_pid > 1)
  {
    kill((pid_t) kill_pid, 1);
  }
  progress_close_cb(o, 1);
}

static int check_pid()
{
  //char errstr[512];
  int kill_ret = 0;

  if (kill_pid <= 1)
  {
    return 1;
  }
  else
  {
    kill_ret = kill((pid_t) kill_pid, 0);
  }

  if (kill_ret == -1)
  {
    /*
    if (errno == EPERM)
    {
      sprintf(errstr, "PID %d: no permission to kill the target process", kill_pid);
    }
    else if (errno == ESRCH)
    {
      sprintf(errstr, "PID %d: the PID or process group does not exist", kill_pid);
    }
    else
    {
      sprintf(errstr, "PID %d: errno %d", kill_pid, errno);
    }

    msg = errstr;
    title = "Error: PID";
    dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO);
    */
    return 1;
  }

  return 0;
}

static void set_progress_box_label(char *ch)
{
  for (int i = 0; (i <= 3 || ch[i] == '\0'); ++i)
  {
    if (ch[i] == '#')
    {
      char *l = ch + i + 1;
      progress_box->label(strdup(l));
      Fl::redraw();
      i = 3;
    }
  }
}

// investigate time-based pulsating effect using ftime(3)

static void progress_cb(void *)
{
  char line[FGETS_LIMIT];

  if (progress_running)
  {
    if (kill_pid != -1 && check_pid() != 0)
    {
      progress_running = false;
    }

    std::string command = "test ! -f '" + watchfile + "' || tail -n1 '" + watchfile + "'";
    FILE *stream = popen(command.c_str(), "r");

    if (stream == NULL)
    {
      perror("problems with pipe");
      pclose(stream);
      progress_close_cb(nullptr, 1);
    }
    else
    {
      if (pulsate)
      {
        while (fgets(line, FGETS_LIMIT, stream))
        {
          if (STREQ(line, "END") || STREQ(line, "EOF") || STREQ(line, "EOL") || STREQ(line, "100"))
          {
            progress_running = false;
          }
          else
          {
            set_progress_box_label(line);
          }
        }

        if (progress_percent >= 100)
        {
          pulsate_val = -1;
        }
        else if (progress_percent <= 0)
        {
          pulsate_val = 1;
        }

        progress_percent += pulsate_val;
        slider->value(progress_percent);
      }
      else
      {
        while (fgets(line, FGETS_LIMIT, stream))
        {
          std::string s(line);
          s.substr(0, 3);
          int i = atoi(s.c_str());

          if (i > progress_percent && i <= 100)
          {
            progress_percent = i;
          }
          else if (i > 100)
          {
            progress_percent = 100;
          }

          set_progress_box_label(line);
        }

        if (progress_percent == 100)
        {
          progress_running = false;
        }

        char *l = new char[5];
        sprintf(l, "%d%%", progress_percent);
        progress_bar->value(progress_percent);
        progress_bar->label(l);
      }
    }
    pclose(stream);
  }
  else
  {
    if (pulsate)
    {
      slider->value(100);
      slider->deactivate();
    }

    if (autoclose)
    {
      progress_close_cb(nullptr, 0);
    }
    else
    {
      if (!hide_cancel)
      {
        progress_but_cancel->deactivate();
      }
      progress_but_ok->activate();
    }
  }

  Fl::repeat_timeout(interval_sec, progress_cb);
}

int dialog_progress()
{
  Fl_Group *g;
  Fl_Box   *dummy;

  if (title == NULL)
  {
    title = "FLTK progress window";
  }

  if (msg == NULL)
  {
    msg = "Progress indicator";
  }

  progress_win = new Fl_Double_Window(320, 140, title);
  progress_win->callback(progress_cancel_cb);
  {
    g = new Fl_Group(0, 0, 320, 140);
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

      if (!autoclose)
      {
        if (!hide_cancel)
        {
          progress_but_cancel = new Fl_Button(210, 104, 100, 26, fl_cancel);
          progress_but_cancel->callback(progress_cancel_cb);
          but_ok_x = 100;
        }
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
  set_taskbar(progress_win);
  progress_win->show();
  set_undecorated(progress_win);

  Fl::add_timeout(interval_sec, progress_cb);
  Fl::run();
  return ret;
}

