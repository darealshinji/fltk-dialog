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
#include <FL/Fl_Choice.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "fltk-dialog.hpp"

static Fl_Double_Window *dropdown_win;
static Fl_Menu_Item *menu_items;

static void dropdown_close_cb(Fl_Widget *, long p)
{
  dropdown_win->hide();
  ret = (int) p;
  delete[] menu_items;
}

int dialog_dropdown(std::string dropdown_list, bool return_number)
{
  Fl_Group         *g;
  Fl_Choice        *entries;
  Fl_Box           *dummy;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  std::vector<std::string> itemlist_v;

  if (msg == NULL)
  {
    msg = "Select an option";
  }

  if (title == NULL)
  {
    title = "FLTK dropdown menu dialog";
  }

  split(dropdown_list, separator, itemlist_v);

  size_t count = itemlist_v.size();
  if (count <= 1)
  {
    msg = "ERROR: need at least 2 entries";
    dialog_message(fl_ok, fl_cancel, NULL, MESSAGE_TYPE_WARNING);
    return 1;
  }

  menu_items = new Fl_Menu_Item[count]();

  for (size_t i = 0; i < count; ++i)
  {
    menu_items[i].text = (itemlist_v[i] == "") ? "<EMPTY>" : itemlist_v[i].c_str();
    menu_items[i].labeltype_ = FL_NORMAL_LABEL;
    menu_items[i].labelsize_ = 14;
  }

  dropdown_win = new Fl_Double_Window(320, 110, title);
  dropdown_win->size_range(320, 110, max_w, max_h);
  dropdown_win->callback(dropdown_close_cb, 1);
  {
    g = new Fl_Group(0, 0, 320, 110);
    {
      entries = new Fl_Choice(10, 30, 300, 30, msg);
      entries->down_box(FL_BORDER_BOX);
      entries->align(FL_ALIGN_TOP_LEFT);
      entries->menu(menu_items);

      dummy = new Fl_Box(99, 73, 1, 1);
      dummy->box(FL_NO_BOX);

      but_ok = new Fl_Return_Button(100, 74, 100, 26, fl_ok);
      but_ok->callback(dropdown_close_cb, 0);
      but_cancel = new Fl_Button(210, 74, 100, 26, fl_cancel);
      but_cancel->callback(dropdown_close_cb, 1);
    }
    g->resizable(dummy);
    g->end();
  }
  run_window(dropdown_win, g);

  if (ret == 0)
  {
    int item = entries->value();
    if (return_number)
    {
      std::cout << (item + 1) << std::endl;
    }
    else
    {
      std::cout << itemlist_v[item] << std::endl;
    }
  }
  return ret;
}

