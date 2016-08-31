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
#include <FL/fl_ask.H>  /* fl_close */
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

#include "dnd.hpp"


static void dnd_exit_cb(Fl_Widget*)
{
  exit(0);
}

int dialog_dnd(char *dnd_label,
               char *dnd_title)
{
  Fl_Window *win;
  /* dnd_box *b; */
  Fl_Button *but_close;

  int bord = 10;
  int winw = 400;
  int winh = 300;
  int butw = 100;
  int buth = 26;

  if (dnd_label == NULL) {
    dnd_label = (char *)"drop here";
  }
  if (dnd_title == NULL) {
    dnd_title = (char *)"FLTK Drag & Drop";
  }

  win = new Fl_Window(winw, winh, dnd_title);
  /* b = */ new dnd_box(0, 0, winw, winh-buth-bord, dnd_label);
  but_close = new Fl_Button(winw-butw-bord, winh-buth-bord, butw, buth, fl_close);

  win->callback(dnd_exit_cb);  /* exit(0) */
  but_close->callback(dnd_exit_cb);

  win->end();
  win->show();

  return Fl::run();
}

