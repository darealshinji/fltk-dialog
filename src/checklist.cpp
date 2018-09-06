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
#include <FL/Fl_Check_Browser.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>

#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>

#include "fltk-dialog.hpp"

static Fl_Double_Window *win;
static int ret = 1;

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = p;
}

int dialog_checklist(std::string checklist_options, bool return_value, bool check_all, char separator)
{
  Fl_Group         *g, *g_inside, *buttongroup;
  Fl_Box           *dummy1, *dummy2;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;
  Fl_Check_Browser *browser;

  std::vector<std::string> vec;
  size_t vec_size;

  split(checklist_options, separator, vec);
  vec_size = vec.size();

  if (vec_size < 2) {
    title = "error: checklist";
    msg = "Two or more options required!";
    dialog_message(MESSAGE_TYPE_INFO);
    return 1;
  }

  if (!title) {
    title = "Select your option(s)";
  }

  win = new Fl_Double_Window(420, 356, title);
  win->callback(close_cb, 1);
  {
    g = new Fl_Group(0, 0, 420, 310);
    {
      g_inside = new Fl_Group(0, 0, 420, 310);
      {
        browser = new Fl_Check_Browser(10, 10, 400, 299);
        browser->box(FL_THIN_DOWN_BOX);
        browser->color(fl_lighter(fl_lighter(FL_BACKGROUND_COLOR)));
        browser->clear_visible_focus();
        for (auto it = vec.begin(); it != vec.end(); ++it) {
          std::string &s = *it;
#ifdef WITH_FRIBIDI
          char *tmp = NULL;
          if (use_fribidi) {
            tmp = fribidi_parse_line(s.c_str());
          }
          if (tmp) {
            browser->add(tmp);
            delete tmp;
          } else
#endif
          {
            browser->add(s.c_str());
          }
        }
        if (check_all) {
          browser->check_all();
        }
        dummy1 = new Fl_Box(10, 308, 400, 1);
        dummy1->box(FL_NO_BOX);
        vec.clear();
      }
      g_inside->resizable(dummy1);
      g_inside->end();
    }
    g->resizable(g_inside);
    g->end();

    buttongroup = new Fl_Group(0, 310, 420, 42);
    {
      int but_w = measure_button_width(fl_cancel, 20);
      but_cancel = new Fl_Button(win->w() - 10 - but_w, 320, but_w, 26, fl_cancel);
      but_cancel->callback(close_cb, 1);
      but_w = measure_button_width(fl_ok, 40);
      but_ok = new Fl_Return_Button(but_cancel->x() - 10 - but_w, 320, but_w, 26, fl_ok);
      but_ok->callback(close_cb, 0);
      dummy2 = new Fl_Box(but_ok->x() - 1, 310, 1, 1);
      dummy2->box(FL_NO_BOX);
    }
    buttongroup->resizable(dummy2);
    buttongroup->end();
  }
  run_window(win, g);

  if (ret == 0) {
    std::string list;
    for (size_t i = 1; i <= vec_size; ++i) {
      if (return_value) {
        if (browser->checked(i)) {
          list.append(quote);
          list.append(browser->text(i));
          list.append(quote);
          list.push_back(separator);
        }
      } else {
        list.append(quote);
        list += (browser->checked(i)) ? "TRUE" : "FALSE";
        list.append(quote);
        list.push_back(separator);
      }
    }
    /* strip trailing separator */
    list.pop_back();
    std::cout << list << std::endl;
  }

  return ret;
}

