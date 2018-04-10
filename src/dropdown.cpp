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

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "fltk-dialog.hpp"

static Fl_Double_Window *win;
static int ret = 1;
static bool msg_alloc = false;

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = (int) p;
#ifdef WITH_FRIBIDI
  if (msg_alloc && msg) {
    free((void *)msg);
  }
#endif
}

int dialog_dropdown(std::string dropdown_list, bool return_number, char separator)
{
  Fl_Group         *g;
  Fl_Choice        *entries;
  Fl_Box           *dummy;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  std::vector<std::string> itemlist_v;

#ifdef WITH_FRIBIDI
  if (msg && use_fribidi && (msg = fribidi_parse_line(msg)) != NULL) {
    msg_alloc = true;
  }
#endif

  if (!msg) {
    msg = "Select an option";
  }

  if (!title) {
    title = "FLTK dropdown menu dialog";
  }

  split(dropdown_list, separator, itemlist_v);

  if (itemlist_v.size() < 2) {
    msg = "ERROR: need at least 2 entries";
    dialog_message(MESSAGE_TYPE_INFO);
    return 1;
  }

  struct Fl_Menu_Item menu_items[itemlist_v.size() + 1];

  for (size_t i = 0; i < itemlist_v.size(); i++) {
    menu_items[i] = { 0,0,0,0,0, FL_NORMAL_LABEL, 0, 14, 0 };

#ifdef WITH_FRIBIDI
    if (use_fribidi) {
      if (itemlist_v[i] == "") {
        menu_items[i].text = strdup("<EMPTY>");
      } else {
        char *tmp = fribidi_parse_line(itemlist_v[i].c_str());
        if (tmp) {
          menu_items[i].text = strdup(tmp);
          free(tmp);
        } else {
          menu_items[i].text = strdup(itemlist_v[i].c_str());
        }
      }
    } else
#endif
    {
      menu_items[i].text = (itemlist_v[i] == "") ? "<EMPTY>" : itemlist_v[i].c_str();
    }
  }

  menu_items[itemlist_v.size()] = { 0,0,0,0,0,0,0,0,0 };

  win = new Fl_Double_Window(320, 110, title);
  win->size_range(320, 110, max_w, max_h);
  win->callback(close_cb, 1);
  {
    g = new Fl_Group(0, 0, 320, 110);
    {
      entries = new Fl_Choice(10, 30, 300, 30, msg);
      entries->down_box(FL_BORDER_BOX);
      entries->align(FL_ALIGN_TOP_LEFT);
      entries->menu(menu_items);

      int but_w = measure_button_width(fl_cancel, 20);
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
  run_window(win, g);

  if (ret == 0) {
    int item = entries->value();
    if (return_number) {
      std::cout << quote << (item + 1) << quote << std::endl;
    } else {
      std::cout << quote << itemlist_v[item] << quote << std::endl;
    }
  }

#ifdef WITH_FRIBIDI
  if (use_fribidi) {
    for (size_t i = 0; i < itemlist_v.size(); i++) {
      if (menu_items[i].text) {
        free((void *)menu_items[i].text);
      }
    }
  }
#endif

  return ret;
}

