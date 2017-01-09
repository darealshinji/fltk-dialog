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
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Double_Window.H>

#include <cstdio>
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


static Fl_Double_Window *progress_win;

static int check_pid(void)
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
    title = "Error: PID";
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
  if (kill_pid > 1)
  {
    kill((pid_t) kill_pid, 1);
  }

  progress_close_cb(o, 1);
}

int dialog_progress(std::string progress_command,
                    bool pulsate,
                    bool autoclose,
                    bool hide_cancel)
{
  Fl_Progress      *progress_bar = NULL;
  Fl_Slider        *slider = NULL;
  Fl_Box           *box, *dummy;
  Fl_Group         *g;
  Fl_Return_Button *but_ok = NULL;
  Fl_Button        *but_cancel = NULL;

  FILE *fpipe;
  std::string progress_msg, script, s, linesubstr;
  int percent = 0;
  int pulsate_val = 1;
  int textlines = 1;
  char percent_label[5];
  char line[256];
  bool full_percentage = false;

  std::string pid_line = "PID=";
  size_t pid_len = pid_line.size();

  std::string return_code_line = "RETURN_CODE=";
  size_t ret_len = return_code_line.size();

  if (msg == NULL)
  {
    progress_msg = "Simple FLTK progress bar";
  }
  else
  {
    progress_msg = translate(msg);
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

  progress_win = new Fl_Double_Window(320, win_h, title);
  progress_win->callback(progress_cancel_cb);
  {
    g = new Fl_Group(0, 0, 320, win_h);
    {
      box = new Fl_Box(0, 0, 10, box_h, progress_msg.c_str());
      box->box(FL_NO_BOX);
      box->align(FL_ALIGN_RIGHT);

      if (pulsate)
      {
        slider = new Fl_Slider(10, box_h, 300, 30, NULL);
        slider->type(1);
        slider->minimum(0);
        slider->maximum(100);
        slider->color(fl_darker(FL_GRAY));
        slider->value(0);
        slider->slider_size(0.25);
      }
      else
      {
        progress_bar = new Fl_Progress(10, box_h, 300, 30);
        progress_bar->minimum(0);
        progress_bar->maximum(100);
        progress_bar->color(fl_darker(FL_GRAY));
        progress_bar->selection_color(fl_lighter(FL_BLUE));
        progress_bar->labelcolor(FL_WHITE);
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
          but_cancel->deactivate();
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
  set_taskbar(progress_win);
  progress_win->show();
  set_undecorated(progress_win);

  /* initialize with 0% bar */
  progress_win->wait_for_expose();
  Fl::flush();

  if (pulsate)
  {
    script = "(" + progress_command + ") >/dev/null & "
      "PID=$! ; "
      "echo " + pid_line + "$PID ; "
      "while ps -p$PID >/dev/null ; "
      "do "
      "  echo ; "  /* print a line for fgets(3) */
      "  sleep 0.003 ; "  /* sleep(1) adjusts the pulsating speed */
      "done ; "
      "wait $PID ; "
      "echo " + return_code_line + "$? ; ";
  }
  else
  {
    script = "log=$(mktemp " P_tmpdir "/fltk-dialog-XXXXXXXX) ; "
      "( (" + progress_command + ") >$log) & "
      "PID=$! ; "
      "echo " + pid_line + "$PID ; "
      "while ps -p$PID >/dev/null ; "
      "do "
      /* repeat last line; a hack to get non-blocking stdin */
      "  tail -n1 $log 2>/dev/null ; "
      "done ; "
      "rm -f $log ; "
      "wait $PID ; "
      "echo " + return_code_line + "$? ; ";
  }

  if ((fpipe = (FILE *)popen(script.c_str(), "r")) == NULL)
  {
    perror("problems with pipe");
    return 1;
  }

  while (fgets(line, sizeof(line), fpipe))
  {
    std::string s(line);

    if (s.size() > pid_len && s.substr(0, pid_len) == pid_line)
    {
      /* get PID */
      s = s.substr(pid_len, s.size() - pid_len);
      kill_pid = atoi(s.c_str());

      if (check_pid() == 0)
      {
        if (!hide_cancel)
        {
          but_cancel->activate();
          Fl::check();
        }
      }
      else
      {
        progress_win->hide();
        pclose(fpipe);
        return 1;
      }
    }
    else if (s.size() > ret_len && s.substr(0, ret_len) == return_code_line)
    {
      /* get return code */
      s = s.substr(ret_len, s.size() - ret_len);
      ret = atoi(s.c_str());
    }

    if (pulsate)
    {
      if (percent >= 100)
      {
        pulsate_val = -1;
      }
      else if (percent <= 0)
      {
        pulsate_val = 1;
      }
      percent += pulsate_val;

      slider->value(percent);
    }
    else
    {
      if (s.compare(0, 1, "#") != 0)  /* ignore lines beginning with a '#' */
      {
        linesubstr = s.substr(0, 3);
        percent = atoi(linesubstr.c_str());

        if (percent >= 0 && percent < 100)
        {
          if (!hide_cancel)
          {
            but_cancel->activate();
          }
          but_ok->deactivate();
          progress_bar->value(percent);
          sprintf(percent_label, "%d%%", percent);
          progress_bar->label(percent_label);
        }
        else if (percent >= 100)
        {
          full_percentage = true;
        }
      }
    }
    Fl::check();
  } /* while fgets(...) */

  pclose(fpipe);

  if (pulsate)
  {
    slider->value(100);
    slider->deactivate();
  }
  else if (!pulsate && full_percentage)
  {
    progress_bar->value(100);
    progress_bar->label("100%");
  }

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

  Fl::run();
  return ret;
}

