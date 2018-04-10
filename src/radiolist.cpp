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
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <string>
#include <vector>

#include "fltk-dialog.hpp"
#include "radiolist_browser.hpp"

static Fl_Double_Window *win;
static Fl_Return_Button *but_ok;
static radiolist_browser *browser;
static int browser_rv = 0;
static bool but_ok_activated = false;
static int ret = 1;

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = (int) p;
}

static void callback(Fl_Widget *) {
  if (!but_ok_activated) {
    but_ok_activated = true;
    but_ok->activate();
  }
  browser_rv = browser->value();
}

int dialog_radiolist(std::string radiolist_options, bool return_number, char separator)
{
  Fl_Group   *g1, *g1a, *g2;
  Fl_Box     *dummy1, *dummy2;
  Fl_Button  *but_cancel;

  std::vector<std::string> radiolist_v;
  split(radiolist_options, separator, radiolist_v);

  if (radiolist_v.size() < 2) {
    title = "error: radiolist";
    msg = "Two or more options required!";
    dialog_message(MESSAGE_TYPE_WARNING);
    return 1;
  }

  if (!title) {
    title = "Select an option";
  }

  win = new Fl_Double_Window(420, 356, title);
  win->callback(close_cb, 1);
  {
    g1 = new Fl_Group(0, 0, 420, 290);
    {
      g1a = new Fl_Group(0, 0, 420, 290);
      {
        for (size_t i = 0; i < radiolist_v.size(); ++i)
        {
          browser = new radiolist_browser(10, 10, 400, 289);
          browser->when(FL_WHEN_CHANGED);
          browser->box(FL_THIN_DOWN_BOX);
          browser->color(fl_lighter(fl_lighter(FL_BACKGROUND_COLOR)));
          browser->clear_visible_focus();
          for (size_t j = 0; j < radiolist_v.size(); ++j) {
#ifdef WITH_FRIBIDI
            char *tmp = NULL;
            if (use_fribidi) {
              tmp = fribidi_parse_line(radiolist_v[j].c_str());
            }
            if (tmp) {
              browser->add(tmp);
              free(tmp);
            } else
#endif
            {
              browser->add(radiolist_v[j].c_str());
            }
          }
          browser->callback(callback);
        }
        dummy1 = new Fl_Box(10, 288, 400, 1);
        dummy1->box(FL_NO_BOX);
      }
      g1a->resizable(dummy1);
      g1a->end();
    }
    g1->resizable(g1a);
    g1->end();

    g2 = new Fl_Group(0, 290 + 20, 420, 36);
    {
      int but_w = measure_button_width(fl_cancel, 20);
      but_cancel = new Fl_Button(win->w() - 10 - but_w, 314, but_w, 26, fl_cancel);
      but_cancel->callback(close_cb, 1);
      but_w = measure_button_width(fl_ok, 40);
      but_ok = new Fl_Return_Button(but_cancel->x() - 10 - but_w, 314, but_w, 26, fl_ok);
      but_ok->callback(close_cb, 0);
      but_ok->deactivate();
      dummy2 = new Fl_Box(but_ok->x() - 1, 290 + 20, 1, 1);
      dummy2->box(FL_NO_BOX);
    }
    g2->resizable(dummy2);
    g2->end();
  }
  run_window(win, g1);

  if (ret == 0) {
    std::cout << quote;
    if (return_number) {
      std::cout << browser_rv;
    } else {
      std::cout << browser->text(browser_rv);
    }
    std::cout << quote << std::endl;
  }
  return ret;
}

