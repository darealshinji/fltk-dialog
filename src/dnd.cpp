/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018, djcj <djcj@gmx.de>
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

#include <iostream>
#include <string>
#include <string.h>

#include "fltk-dialog.hpp"

static Fl_Double_Window *win;
static Fl_Box *count;
static unsigned int val = 0;

class dnd_box : public Fl_Box
{
public:
  dnd_box(int X, int Y, int W, int H, const char *L=NULL)
    : Fl_Box(X, Y, W, H, L) { }

  int handle(int event) {
    switch (event) {
      case FL_DND_ENTER:
      case FL_DND_DRAG:
      case FL_DND_RELEASE:
        return 1;
      case FL_PASTE:
        do_callback();
        return 1;
    }
    return Fl_Box::handle(event);
  }
};

static void close_cb(Fl_Widget *) {
  win->hide();
}

static void callback(Fl_Widget *)
{
  char buf[16];
  const char *items = Fl::event_text();

  val++;
  snprintf(buf, sizeof(buf) - 1, "%d", val);
  count->copy_label(buf);
  win->redraw();

  if (strncmp(items, "file:///", 8) == 0) {
    std::string s(items + 7);
    std::string q(quote);
    repstr("\nfile://", q + "\n" + q, s);
    s.pop_back();  /* remove trailing newline */
    char *ch = strdup(s.c_str());
    fl_decode_uri(ch);
    std::cout << quote << ch << quote << std::endl;
    free(ch);
  } else {
    std::cout << quote << items << quote << std::endl;
  }
}

int dialog_dnd()
{
  dnd_box  *box;
  Fl_Group *g;
  Fl_Box   *text, *dummy;
  Fl_Return_Button *but_close;
  std::string s;
  int range;

  if (msg) {
#ifdef WITH_FRIBIDI
    char *tmp = NULL;
    if (use_fribidi) {
      tmp = fribidi_parse_line(msg);
    }
    if (tmp) {
      s = translate(tmp);
      msg = s.c_str();
      delete tmp;
    } else
#endif
    {
      s = translate(msg);
      msg = s.c_str();
    }
  }

  if (!msg) {
    msg = "drop stuff here";
  }

  if (!title) {
    title = "FLTK Drag & Drop";
  }

  win = new Fl_Double_Window(400, 244 + 56, title);
  win->callback(close_cb);
  {
    box = new dnd_box(10, 10, 380, 244, msg);
    box->box(FL_ENGRAVED_FRAME);
    box->callback(callback);

    g = new Fl_Group(0, 244, 400, 56);
    {
      const char *l = "Drop count:";
      int but_w = measure_button_width(l);
      range = but_w;
      text = new Fl_Box(10, 244 + 20, but_w, 26, l);
      text->box(FL_NO_BOX);
      text->align(FL_ALIGN_CENTER);

      count = new Fl_Box(10, 244 + 20, but_w, 26, "0");
      count->box(FL_NO_BOX);
      count->align(FL_ALIGN_RIGHT);

      but_w = measure_button_width(fl_close, 40);
      range += but_w + 60;
      dummy = new Fl_Box(389 - but_w, 260, 1, 1);
      dummy->box(FL_NO_BOX);
      but_close = new Fl_Return_Button(390 - but_w, 244 + 20, but_w, 26, fl_close);
      but_close->callback(close_cb);
    }
    g->resizable(dummy);
    g->end();
  }
  run_window(win, box, range, 100);

  return 0;
}

