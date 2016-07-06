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
#include <FL/Fl_Progress.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

/* std::getline, c_str, compare, substr */
#include <string>
/* std::cin */
#include <iostream>
/* sprintf */
#include <stdio.h>
/* atoi */
#include <stdlib.h>
/* strlen */
#include <string.h>

#include "fltk-dialog.h"

Fl_Window *prog_win;
Fl_Progress *progress;

void progress_close_cb(Fl_Widget*, void*)
{
  prog_win->remove(progress);
  delete(progress);
  exit(0);
}

/* run a test:
 * (for n in `seq 1 100`; do echo "#$n" && sleep 0.0$n; done) | ./fltk-dialog --progress; echo "exit: $?"
 */
int dialog_fl_progress(char *progress_msg,
                       char *progress_title,
                       int autoclose)
{
  Fl_Box *prog_box;
  Fl_Button *prog_but = NULL;

  int winw = 320;
  int winh = 0;
  int barh = 30;
  int bord = 10;
  int textheight = FL_NORMAL_SIZE + 4;
  int textlines = 1;
  int butw = 100;
  int buth = 25;

  std::string line, linesubstr;
  int percent = 0;
  char percent_label[5];
  int ret = 1;

  if (progress_msg == NULL) {
    progress_msg = (char *)"Simple FLTK progress bar";
  } else {
    progress_msg = translate(progress_msg);
    for (size_t i = 0; i < strlen(progress_msg); i++) {
      if (progress_msg[i] == '\n')
        textlines++;
    }
  }

  if (progress_title == NULL) {
    progress_title = (char *)"FLTK progress window";
  }

  int boxh = textlines*textheight + bord*2;

  if (autoclose == 0) {
    winh = boxh+barh+buth+bord*2+5;
  } else {
    winh = boxh+barh+bord+5;
  }

  prog_win = new Fl_Window(winw, winh);
  prog_win->label(progress_title);
  prog_win->begin();

  /* message text */
  prog_box = new Fl_Box(0, 0, bord, boxh, progress_msg);
  prog_box->box(FL_NO_BOX);
  prog_box->align(FL_ALIGN_RIGHT);

  /* create the progress bar */
  progress = new Fl_Progress(bord, boxh, winw-bord*2, barh);
  progress->minimum(0);
  progress->maximum(100);
  progress->color(0x88888800);  /* background color */
  progress->selection_color(0x4444ff00);  /* progress bar color */
  progress->labelcolor(FL_WHITE);  /* percent text color */
  progress->value(0);
  progress->label("0%");

  if (autoclose == 0) {
    /* close button */
    prog_but = new Fl_Button(winw-butw-bord,
                             boxh+textheight+buth,
                             butw,
                             buth,
                             fl_close);
    prog_but->deactivate();
    prog_but->callback(progress_close_cb);
  }

  prog_win->end();
  prog_win->show();

  /* get stdin line by line */
  for (/**/; std::getline(std::cin, line); /**/) {
    if (line.compare(0,1,"#") == 0) {  /* "grep" lines beginning with a '#' */
      linesubstr = line.substr(1,3);
      percent = atoi(linesubstr.c_str());
      if (percent >= 0 && percent <= 100) {
        progress->value(percent);  /* update progress bar */
        sprintf(percent_label, "%d%%", percent);
        progress->label(percent_label);  /* update progress bar's label */
        Fl::check();  /* update the screen */
        if (percent == 100) {
          if (autoclose == 0) {
            prog_but->activate();
          } else {
            prog_win->remove(progress);
            delete(progress);
            ret = 0;
          }
        }
      }
    }
  }

  if (autoclose == 0) { ret = Fl::run(); }
  return ret;
}

