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
#include <FL/Fl_Progress.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

#include <string>    /* std::string, std::getline, c_str, compare, substr */
#include <iostream>  /* std::cin */
#include <stdio.h>   /* sprintf */
#include <stdlib.h>  /* exit, atoi */
#include <string.h>  /* strlen */

#include "fltk-dialog.hpp"
#include "misc/readstdio.hpp"


Fl_Window *prog_win;
Fl_Progress *prog_bar;

static void progress_exit(int ret)
{
  prog_win->remove(prog_bar);
  delete prog_bar;
  exit(ret);
}

static void progress_exit0_cb(Fl_Widget*)
{
  progress_exit(0);
}

static void progress_exit1_cb(Fl_Widget*)
{
  progress_exit(1);
}

/* run a test:
 * (for n in `seq 1 100`; do echo "$n" && sleep 0.0$n; done) | ./fltk-dialog --progress; echo "exit: $?"
 */
int dialog_fl_progress(bool autoclose,
                       bool hide_cancel)
{
  Fl_Box *box;
  Fl_Button *but_ok = NULL;
  Fl_Button *but_cancel = NULL;

  int winw = 320;
  int winh = 0;
  int barh = 30;
  int bord = 10;
  int textheight = 18;
  int textlines = 1;
  int butw = 100;
  int buth = 26;
  int but_right = winw-butw-bord;
  int but_left = winw-butw*2-bord*2;

  std::string s, line, linesubstr;
  int percent = 0;
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
    s = "error: select()";
    msg = s.c_str();
    dialog_fl_message(ALERT);
    return 1;
  }
  else if (!stdin)
  {
    s = "error: no input data receiving";
    msg = s.c_str();
    dialog_fl_message(ALERT);
    return 1;
  }

  int boxh = textlines*textheight + bord*2;
  if (!autoclose)
  {
    winh = boxh+barh+buth+bord*2+5;
  }
  else
  {
    winh = boxh+barh+bord+5;
  }

  prog_win = new Fl_Window(winw, winh, title);
  prog_win->begin();
  prog_win->callback(progress_exit1_cb);
  {
    box = new Fl_Box(0, 0, bord, boxh, s.c_str());
    box->box(FL_NO_BOX);
    box->align(FL_ALIGN_RIGHT);

    prog_bar = new Fl_Progress(bord, boxh, winw-bord*2, barh);
    prog_bar->minimum(0);
    prog_bar->maximum(100);
    prog_bar->color(0x88888800);  /* background color */
    prog_bar->selection_color(0x4444ff00);  /* progress bar color */
    prog_bar->labelcolor(FL_WHITE);  /* percent text color */
    prog_bar->value(0);
    prog_bar->label("0%");

    if (!autoclose)
    {
      int but_ok_x = but_right;
      if (!hide_cancel)
      {
        but_cancel = new Fl_Button(but_right, boxh+textheight+buth, butw, buth, fl_cancel);
        but_cancel->callback(progress_exit1_cb);
        but_ok_x = but_left;
      }
      but_ok = new Fl_Button(but_ok_x, boxh+textheight+buth, butw, buth, fl_ok);
      but_ok->deactivate();
      but_ok->callback(progress_exit0_cb);
    }
  }
  prog_win->end();
  prog_win->show();

  /* initialize with 0% bar */
  prog_win->wait_for_expose();
  Fl::flush();

  /* get stdin line by line */
  for (/**/; std::getline(std::cin, line); /**/)
  {
    /* ignore lines beginning with a '#' */
    if (line.compare(0, 1, "#") != 0)
    {
      linesubstr = line.substr(0,3);
      percent = atoi(linesubstr.c_str());
      if (percent >= 0 && percent <= 100)
      {
        prog_bar->value(percent);  /* update progress bar */
        sprintf(percent_label, "%d%%", percent);
        prog_bar->label(percent_label);  /* update progress bar's label */
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
          else
          {
            progress_exit(0);
          }
        }
      }
    }
  }

  int ret = Fl::run();
  prog_win->remove(prog_bar);
  delete prog_bar;
  return ret;
}

