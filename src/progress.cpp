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

/* to run a test:
( (for n in `seq 1 100`; do echo "$n" && sleep 0.0$n; done) & echo $! >&3 ) 3>/tmp/mypid | \
  ./fltk-dialog --progress --auto-kill=$(cat /tmp/mypid); echo "exit: $?"
 */

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Window.H>

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "misc/readstdio.hpp"


static Fl_Window *progress_win;

static int check_pid()
{
  char errstr[512];
  int kill_ret = 0;

  if (kill_pid <= 1)
  {
    std::cerr << "Warning: ignoring process ID `" << kill_pid << "'" << std::endl;
  }
  else
  {
    kill_ret = kill((pid_t) kill_pid, 0);
  }

  if (kill_ret == -1)
  {
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
    title = "Error: auto-kill PID";
    dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO);
    return 1;
  }

  return 0;
}

static void progress_close_cb(Fl_Widget *, long p)
{
  progress_win->hide();
  ret = (int) p;
}

static void progress_cancel_cb(Fl_Widget *o)
{
  if (kill_parent)
  {
    kill_pid = (int) getppid();
  }

  if (kill_pid > 1)
  {
    kill((pid_t) kill_pid, 1);
  }

  progress_close_cb(o, 1);
}

int dialog_progress(bool pulsate,
                    bool autoclose,
                    bool hide_cancel)
{
  Fl_Progress      *progress_bar = NULL;
  Fl_Slider        *slider = NULL;
  Fl_Box           *box, *dummy;
  Fl_Group         *g;
  Fl_Return_Button *but_ok = NULL;
  Fl_Button        *but_cancel = NULL;

  std::string s, line, linesubstr;
  int percent = 0;
  int textlines = 1;
  char percent_label[5];

  if (msg == NULL)
  {
    s = "Simple FLTK progress bar";
  }
  else
  {
    s = translate(msg);
    for (size_t i = 0; i < strlen(msg); i++)
    {
      if (msg[i] == '\n')
      {
        textlines++;
      }
    }
  }

  if (title == NULL)
  {
    title = "FLTK progress window";
  }

  int stdin;
  READSTDIO(stdin);

  if (stdin == -1)
  {
    msg = "ERROR: select()";
    dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO);
    return 1;
  }
  else if (!stdin)
  {
    msg = "ERROR: no input data receiving";
    dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO);
    return 1;
  }

  if (check_pid() == 1)
  {
    return 1;
  }

  int box_h = (textlines * 18) + 20;
  int mod_h = box_h + 44;
  int win_h = 0;
  if (!autoclose)
  {
    win_h = box_h + 81;
  }
  else
  {
    win_h = box_h + 45;
  }

  progress_win = new Fl_Window(320, win_h, title);
  progress_win->callback(progress_cancel_cb);
  {
    g = new Fl_Group(0, 0, 320, win_h);
    {
      box = new Fl_Box(0, 0, 10, box_h, s.c_str());
      box->box(FL_NO_BOX);
      box->align(FL_ALIGN_RIGHT);

      if (pulsate)
      {
        slider = new Fl_Slider(10, box_h, 300, 30, NULL);
        slider->type(1);
        slider->minimum(0);
        slider->maximum(100);
        slider->value(0);
        slider->slider_size(0.25);
      }
      else
      {
        progress_bar = new Fl_Progress(10, box_h, 300, 30);
        progress_bar->minimum(0);
        progress_bar->maximum(100);
        progress_bar->color(0x88888800);  /* background color */
        progress_bar->selection_color(0x4444ff00);  /* progress bar color */
        progress_bar->labelcolor(FL_WHITE);  /* percent text color */
        progress_bar->value(0);
        progress_bar->label("0%");
      }

      int but_ok_x = 210;

      if (!autoclose)
      {
        if (!hide_cancel)
        {
          but_cancel = new Fl_Button(210, mod_h, 100, 28, fl_cancel);
          but_cancel->callback(progress_cancel_cb);
          but_ok_x = 100;
        }
        but_ok = new Fl_Return_Button(but_ok_x, mod_h, 100, 28, fl_ok);
        but_ok->deactivate();
        but_ok->callback(progress_close_cb, 0);
      }
      dummy = new Fl_Box(but_ok_x - 1, mod_h - 1, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    g->resizable(dummy);
    g->end();
  }
  set_size(progress_win, g);
  set_position(progress_win);
  progress_win->end();
  progress_win->show();

  /* initialize with 0% bar */
  progress_win->wait_for_expose();
  Fl::flush();

  if (pulsate)
  {
    char c;
    int a = 2;
    std::ifstream stdin("/dev/stdin");  /* TODO: make more portable */

    while (stdin.get(c))
    {
      if (percent >= 100)
      {
        a = -2;
      }
      else if (percent <= 0)
      {
        a = 2;
      }
      percent += a;

      slider->value(percent);
      Fl::check();  /* update the screen */
    }

    slider->value(100);
    slider->deactivate();
    Fl::check();

    if (autoclose)
    {
      progress_win->hide();
    }
    else
    {
      progress_win->callback(progress_close_cb, 0);
      but_ok->activate();

      if (!hide_cancel)
      {
        but_cancel->deactivate();
      }
    }

    if (!stdin.eof())
    {
      title = "error";
      msg = "Error: stdin";
      dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO);
    }

    stdin.close();
  }
  else /* if (!pulsate) */
  {
    /* get stdin line by line */
    for (/**/; std::getline(std::cin, line); /**/)
    {
      /* ignore lines beginning with a '#' */
      if (line.compare(0, 1, "#") != 0)
      {
        linesubstr = line.substr(0, 3);
        percent = atoi(linesubstr.c_str());
        if (percent >= 0 && percent <= 100)
        {
          progress_bar->value(percent);
          sprintf(percent_label, "%d%%", percent);
          progress_bar->label(percent_label);
          Fl::check();

          if (percent == 100)
          {
            if (autoclose)
            {
              progress_win->hide();
            }
            else
            {
              progress_win->callback(progress_close_cb, 0);
              but_ok->activate();

              if (!hide_cancel)
              {
                but_cancel->deactivate();
              }
            }
          }
        }
      }
    }
  }

  Fl::run();
  return ret;
}

