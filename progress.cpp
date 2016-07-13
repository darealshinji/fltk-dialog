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
/* isatty */
#include <unistd.h>

#include "fltk-dialog.h"


Fl_Window *prog_win;
Fl_Progress *prog_bar;

static void prog_but_cb(Fl_Widget*)
{
  prog_win->remove(prog_bar);
  delete(prog_bar);
  exit(0);
}

/* run a test:
 * (for n in `seq 1 100`; do echo "#$n" && sleep 0.0$n; done) | ./fltk-dialog --progress; echo "exit: $?"
 */
int dialog_fl_progress(const char *progress_msg,
                       char *progress_title,
                       int autoclose)
{
  Fl_Box *box;
  Fl_Button *but = NULL;

  int winw = 320;
  int winh = 0;
  int barh = 30;
  int bord = 10;
  int textheight = 18;
  int textlines = 1;
  int butw = 100;
  int buth = 26;

  std::string s, line, linesubstr;
  int percent = 0;
  char percent_label[5];

  if (progress_msg == NULL) {
    s = "Simple FLTK progress bar";
  } else {
    s = translate(progress_msg);
    for (size_t i = 0; i < strlen(progress_msg); i++) {
      if (progress_msg[i] == '\n')
        textlines++;
    }
  }

  if (progress_title == NULL) {
    progress_title = (char *)"FLTK progress window";
  }

  /* check for input data */
  if (isatty(STDIN_FILENO)) {
    s = "Error: no input data receiving";
    dialog_fl_message(s.c_str(), progress_title, 1);
    return 1;
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
  box = new Fl_Box(0, 0, bord, boxh, s.c_str());
  box->box(FL_NO_BOX);
  box->align(FL_ALIGN_RIGHT);

  /* create the progress bar */
  prog_bar = new Fl_Progress(bord, boxh, winw-bord*2, barh);
  prog_bar->minimum(0);
  prog_bar->maximum(100);
  prog_bar->color(0x88888800);  /* background color */
  prog_bar->selection_color(0x4444ff00);  /* progress bar color */
  prog_bar->labelcolor(FL_WHITE);  /* percent text color */
  prog_bar->value(0);
  prog_bar->label("0%");

  if (autoclose == 0) {
    /* close button */
    but = new Fl_Button(winw-butw-bord,
                        boxh+textheight+buth,
                        butw,
                        buth,
                        fl_close);
    but->deactivate();
    but->callback(prog_but_cb);
  }

  prog_win->end();
  prog_win->show();

  /* initialize with 0% bar */
  prog_win->wait_for_expose();
  Fl::flush();

  /* get stdin line by line */
  for (/**/; std::getline(std::cin, line); /**/) {
    if (line.compare(0,1,"#") == 0) {  /* "grep" lines beginning with a '#' */
      linesubstr = line.substr(1,3);
      percent = atoi(linesubstr.c_str());
      if (percent >= 0 && percent <= 100) {
        prog_bar->value(percent);  /* update progress bar */
        sprintf(percent_label, "%d%%", percent);
        prog_bar->label(percent_label);  /* update progress bar's label */
        Fl::check();  /* update the screen */
        if (percent == 100) {
          if (autoclose == 0) {
            but->activate();
          } else {
            prog_win->remove(prog_bar);
            delete(prog_bar);
            exit(0);
          }
        }
      }
    }
  }

  return Fl::run();
}

