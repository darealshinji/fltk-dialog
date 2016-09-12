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

#include <iostream>  /* std::cout */
#include <stdio.h>   /* sprintf */

#include "dnd.hpp"


Fl_Window *dnd_window;
Fl_Box *dnd_count;
int dnd_count_val = 0;

static void dnd_callback(const char *items)
{
  const char *ch;
  static char newtext[128];

  for (ch = items; *ch != '\0'; ch++)
  {
    if (*ch == '\n')
    {
      dnd_count_val++;
    }
  }

  sprintf(newtext, "%d", dnd_count_val);
  dnd_count->label(newtext);
  dnd_window->redraw();

  std::cout << items;
}

static void dnd_exit_cb(Fl_Widget*)
{
  exit(0);
}

int dialog_dnd(char *dnd_label,
               char *dnd_title)
{
  dnd_box *box;
  Fl_Group *g;
  Fl_Box *text;
  Fl_Button *but_close;

  int bord = 10;
  int winw = 400;
  int winh = 300;
  int butw = 100;
  int buth = 26;
  int textw = 110 + bord;

  if (dnd_label == NULL)
  {
    dnd_label = (char *)"drop stuff here";
  }

  if (dnd_title == NULL)
  {
    dnd_title = (char *)"FLTK Drag & Drop";
  }

  dnd_window = new Fl_Window(winw, winh, dnd_title);
  dnd_window->begin();
  dnd_window->callback(dnd_exit_cb);
  {
    box = new dnd_box(bord, bord, winw-bord*2, winh-buth-bord*3, dnd_label);
    box->box(FL_ENGRAVED_FRAME);
    g = new Fl_Group(bord, winh-buth-bord*3, winw-bord*2, buth+bord*2);
    {
      text = new Fl_Box(bord, winh-buth-bord, textw, buth);
      text->label("Items dropped:");
      text->box(FL_NO_BOX);
      text->align(FL_ALIGN_CENTER);

      /* the dnd_count widget has the same size and position as the
       * text widget, but its label text is placed right to it */
      dnd_count = new Fl_Box(bord, winh-buth-bord, textw, buth);
      dnd_count->label("0");
      dnd_count->box(FL_NO_BOX);
      dnd_count->align(FL_ALIGN_RIGHT);

      but_close = new Fl_Button(winw-butw-bord, winh-buth-bord, butw, buth, fl_close);
      but_close->callback(dnd_exit_cb);
    }
    g->end();
  }
  dnd_window->end();
  dnd_window->show();

  return Fl::run();
}

