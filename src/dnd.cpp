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
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <stdio.h>

#include "fltk-dialog.hpp"


static Fl_Double_Window *dnd_win;
static Fl_Box *dnd_count;
static void dnd_callback(const char *items);
static int dnd_count_val = 0;

class dnd_box : public Fl_Box
{
  public:

    dnd_box(int X, int Y, int W, int H, const char *L=0)
      : Fl_Box(X, Y, W, H, L) { }

    virtual ~dnd_box() { }

    int handle(int event);
};

int dnd_box::handle(int event)
{
  int handle_ret = Fl_Box::handle(event);
  switch (event) {
    case FL_DND_ENTER:
    case FL_DND_DRAG:
    case FL_DND_RELEASE:
      handle_ret = 1;
      break;
    case FL_PASTE:
      dnd_callback(Fl::event_text());
      handle_ret = 1;
      break;
  }
  return handle_ret;
}

static void dnd_close_cb(Fl_Widget *)
{
  dnd_win->hide();
}

static void dnd_callback(const char *items)
{
  const char *ch;
  static char newtext[128];

  for (ch = items; *ch != '\0'; ++ch)
  {
    if (*ch == '\n')
    {
      ++dnd_count_val;
    }
  }

  sprintf(newtext, "%d", dnd_count_val);
  dnd_count->label(newtext);
  dnd_win->redraw();

  std::cout << items;
}

int dialog_dnd()
{
  dnd_box          *box;
  Fl_Group         *g;
  Fl_Box           *text, *dummy;
  Fl_Return_Button *but_close;

  int but_w;

  if (msg == NULL)
  {
    msg = "drop stuff here";
  }

  if (title == NULL)
  {
    title = "FLTK Drag & Drop";
  }

  dnd_win = new Fl_Double_Window(400, 300, title);
  dnd_win->size_range(400, 300, max_w, max_h);
  dnd_win->callback(dnd_close_cb);
  {
    box = new dnd_box(10, 10, 380, 244, msg);
    box->box(FL_ENGRAVED_FRAME);

    g = new Fl_Group(0, 244, 400, 56);
    {
      text = new Fl_Box(10, 264, 120, 26);
      text->label("Items dropped:");
      text->box(FL_NO_BOX);
      text->align(FL_ALIGN_CENTER);

      /* the dnd_count widget has the same size and position as the
       * text widget, but its label text is placed right to it */
      dnd_count = new Fl_Box(10, 264, 120, 26);
      dnd_count->label("0");
      dnd_count->box(FL_NO_BOX);
      dnd_count->align(FL_ALIGN_RIGHT);

      but_close = new Fl_Return_Button(0,0,0,0, fl_close);
      measure_button_width(but_close, but_w, 40);

      dummy = new Fl_Box(389 - but_w, 260, 1, 1);
      dummy->box(FL_NO_BOX);
      but_close = new Fl_Return_Button(390 - but_w, 264, but_w, 26, fl_close);
      but_close->callback(dnd_close_cb);
    }
    g->resizable(dummy);
    g->end();
  }
  set_size(dnd_win, box);
  set_position(dnd_win);
  dnd_win->end();
  set_taskbar(dnd_win);
  dnd_win->show();
  set_undecorated(dnd_win);
  Fl::run();

  return ret;
}

