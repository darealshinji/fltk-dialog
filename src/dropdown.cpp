/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020, djcj <djcj@gmx.de>
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
#include <vector>
#include <stdlib.h>

#include "fltk-dialog.hpp"

static Fl_Double_Window *win;
static int ret = 1;

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = p;
}

int dialog_dropdown(std::string dropdown_list, bool return_number, char separator)
{
  Fl_Group         *g;
  Fl_Choice        *entries;
  Fl_Box           *dummy;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  Fl_Menu_Item *menu_items;
  int range;

  std::vector<std::string> vec;
  size_t vec_size;

  if (!msg) {
    msg = "Select an option";
  }

  if (!title) {
    title = "FLTK dropdown menu dialog";
  }

  split(dropdown_list, separator, vec);
  vec_size = vec.size();

  if (vec_size < 2) {
    msg = "ERROR: need at least 2 entries";
    dialog_message(MESSAGE_TYPE_INFO);
    return 1;
  }

  menu_items = new Fl_Menu_Item[vec_size + 1];

  for (size_t i = 0; i < vec_size; ++i) {
    menu_items[i] = { 0,0,0,0,0, FL_NORMAL_LABEL, 0, 14, 0 };
    menu_items[i].text = (vec.at(i) == "") ? "<EMPTY>" : vec.at(i).c_str();
  }

  menu_items[vec_size] = { 0,0,0,0,0,0,0,0,0 };
  vec.clear();

  win = new Fl_Double_Window(320, 110, title);
  win->callback(close_cb, 1);
  {
    g = new Fl_Group(0, 0, 320, 110);
    {
      entries = new Fl_Choice(10, 30, 300, 30, msg);
      entries->down_box(FL_BORDER_BOX);
      entries->align(FL_ALIGN_TOP_LEFT);
      entries->menu(menu_items);

      int but_w = measure_button_width(fl_cancel, 20);
      range = but_w + 60;
      but_cancel = new Fl_Button(310 - but_w, 74, but_w, 26, fl_cancel);
      but_cancel->callback(close_cb, 1);

      but_w = measure_button_width(fl_ok, 40);
      but_ok = new Fl_Return_Button(but_cancel->x() - 10 - but_w, 74, but_w, 26, fl_ok);
      but_ok->callback(close_cb, 0);

      dummy = new Fl_Box(but_ok->x() - 1, 73, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    g->resizable(dummy);
    g->end();
  }
  run_window(win, g, range, win->h());

  if (ret == 0) {
    int n = entries->value();
    if (return_number) {
      std::cout << quote << n + 1 << quote << std::endl;
    } else {
      std::cout << quote << menu_items[n].text << quote << std::endl;
    }
  }

  delete[] menu_items;

  return ret;
}

