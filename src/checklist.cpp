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
#include <FL/Fl_Check_Browser.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>

#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>

#include "fltk-dialog.hpp"
#include "misc/split.hpp"


static Fl_Double_Window *check_button_win;

static void check_button_close_cb(Fl_Widget *, long p)
{
  check_button_win->hide();
  ret = (int) p;
}

int dialog_checklist(std::string checklist_options,
                            bool return_value,
                            bool check_all)
{
  Fl_Group         *g, *g_inside, *buttongroup;
  Fl_Box           *dummy1, *dummy2;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;
  Fl_Check_Browser *browser;

  std::vector<std::string> checklist_v;
  std::vector<long> counter_v;
  int count = 0;

  split(checklist_options, separator, checklist_v);

  for (size_t i = 0; i < checklist_v.size(); ++i)
  {
    counter_v.push_back((long) i);
    ++count;
  }

  if (count < 1)
  {
    title = "error: checklist";
    msg = "Two or more options required!";
    dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO);
    return 1;
  }

  if (title == NULL)
  {
    title = "Select your option(s)";
  }

  check_button_win = new Fl_Double_Window(420, 356, title);
  check_button_win->callback(check_button_close_cb, 1);
  {
    g = new Fl_Group(0, 0, 420, 310);
    {
      g_inside = new Fl_Group(0, 0, 420, 310);
      {
        browser = new Fl_Check_Browser(10, 10, 400, 299);
        browser->box(FL_THIN_DOWN_BOX);
        browser->color(fl_lighter(fl_lighter(FL_BACKGROUND_COLOR)));
        browser->clear_visible_focus();
        for (int i = 0; i < count; ++i)
        {
          browser->add(checklist_v[i].c_str());
        }
        if (check_all)
        {
          browser->check_all();
        }
        dummy1 = new Fl_Box(10, 308, 400, 1);
        dummy1->box(FL_NO_BOX);
      }
      g_inside->resizable(dummy1);
      g_inside->end();
    }
    g->resizable(g_inside);
    g->end();

    buttongroup = new Fl_Group(0, 310, 420, 42);
    {
      dummy2 = new Fl_Box(199, 310, 1, 1);
      dummy2->box(FL_NO_BOX);
      but_ok = new Fl_Return_Button(200, 320, 100, 26, fl_ok);
      but_ok->callback(check_button_close_cb, 0);
      but_cancel = new Fl_Button(310, 320, 100, 26, fl_cancel);
      but_cancel->callback(check_button_close_cb, 1);
    }
    buttongroup->resizable(dummy2);
    buttongroup->end();
  }
  set_size(check_button_win, g);
  set_position(check_button_win);
  check_button_win->end();
  check_button_win->show();
  set_undecorated(check_button_win);
  Fl::run();

  if (ret == 0)
  {
    std::string list;

    for (int i = 1; i <= count; ++i)
    {
      if (return_value)
      {
        if (browser->checked(i))
        {
          list += std::string(browser->text(i)) + separator_s;
        }
      }
      else
      {
        if (browser->checked(i))
        {
          list += "TRUE";
        }
        else
        {
          list += "FALSE";
        }
        list += separator_s;
      }
    }

    /* strip trailing separator */
    std::cout << list.substr(0, list.length() - 1) << std::endl;
  }

  return ret;
}

