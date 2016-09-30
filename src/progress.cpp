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
#include <FL/Fl_Window.H>

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fltk-dialog.hpp"
#include "misc/readstdio.hpp"


static Fl_Window *progress_win;

static void progress_exit0_cb(Fl_Widget*)
{
  progress_win->hide();
}

static void progress_exit1_cb(Fl_Widget*)
{
  progress_win->hide();
  ret = 1;
}

/* run a test:
 * (for n in `seq 1 100`; do echo "$n" && sleep 0.0$n; done) | ./fltk-dialog --progress; echo "exit: $?"
 */
int dialog_fl_progress(bool autoclose,
                       bool hide_cancel)
{
  Fl_Progress      *progress_bar;
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
    title = (char *)"FLTK progress window";
  }

  int stdin;
  READSTDIO(stdin);

  if (stdin == -1)
  {
    msg = (char *)"ERROR: select()";
    dialog_fl_message(ALERT);
    return 1;
  }
  else if (!stdin)
  {
    msg = (char *)"ERROR: no input data receiving";
    dialog_fl_message(ALERT);
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
  progress_win->callback(progress_exit1_cb);
  {
    g = new Fl_Group(0, 0, 320, win_h);
    {
      box = new Fl_Box(0, 0, 10, box_h, s.c_str());
      box->box(FL_NO_BOX);
      box->align(FL_ALIGN_RIGHT);

      progress_bar = new Fl_Progress(10, box_h, 300, 30);
      progress_bar->minimum(0);
      progress_bar->maximum(100);
      progress_bar->color(0x88888800);  /* background color */
      progress_bar->selection_color(0x4444ff00);  /* progress bar color */
      progress_bar->labelcolor(FL_WHITE);  /* percent text color */
      progress_bar->value(0);
      progress_bar->label("0%");

      int but_ok_x = 220;

      if (!autoclose)
      {
        if (!hide_cancel)
        {
          but_cancel = new Fl_Button(220, mod_h, 90, 26, fl_cancel);
          but_cancel->callback(progress_exit1_cb);
          but_ok_x = 120;
        }
        but_ok = new Fl_Return_Button(but_ok_x, mod_h, 90, 26, fl_ok);
        but_ok->deactivate();
        but_ok->callback(progress_exit0_cb);
      }
      dummy = new Fl_Box(but_ok_x - 1, mod_h - 1, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    g->resizable(dummy);
    g->end();
  }
  if (resizable)
  {
    progress_win->resizable(g);
  }
  progress_win->end();
  progress_win->show();

  /* initialize with 0% bar */
  progress_win->wait_for_expose();
  Fl::flush();

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
       /*
        if (percent >= 0 && percent < 25)
        {
          progress_bar->color(0x4444ff00);
          progress_bar->selection_color(0x88888800);
          progress_bar->value(percent*4);
        }
        else if (percent >= 25 && percent < 50)
        {
          progress_bar->color(0x88888800);
          progress_bar->selection_color(0x4444ff00);
          progress_bar->value((percent-25)*4);
        }
        else if (percent >= 50 && percent < 75)
        {
          progress_bar->color(0x4444ff00);
          progress_bar->selection_color(0x88888800);
          progress_bar->value((percent-50)*4);
        }
        else if (percent >= 75 && percent <= 100)
        {
          progress_bar->color(0x88888800);
          progress_bar->selection_color(0x4444ff00);
          progress_bar->value((percent-75)*4);
        }
        progress_bar->label("");
        */

        progress_bar->value(percent);  /* update progress bar */
        sprintf(percent_label, "%d%%", percent);
        progress_bar->label(percent_label);  /* update progress bar's label */
        Fl::check();  /* update the screen */

        if (percent == 100)
        {
          if (!autoclose)
          {
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

  Fl::run();
  return ret;
}

