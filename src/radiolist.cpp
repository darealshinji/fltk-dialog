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
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Window.H>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "fltk-dialog.hpp"
#include "misc/split.hpp"


static Fl_Window        *radio_round_button_win;
static Fl_Return_Button *radiolist_but_ok;
static const char *radiolist_return = NULL;
static bool radiolist_but_ok_activated = false;

static void radio_round_button_cb(Fl_Widget *w, long p)
{
  (void) w;
  radio_round_button_win->hide();
  ret = (int) p;
}

static void rb_callback(Fl_Widget *w, void *p)
{
  (void) w;

  if (!radiolist_but_ok_activated)
  {
    radiolist_but_ok_activated = true;
    radiolist_but_ok->activate();
  }

  radiolist_return = (char *)p;
}

int dialog_fl_radio_round_button(std::string radiolist_options,
                                        bool return_number)
{
  Fl_Group  *g, *g_inside, *buttongroup;
  Fl_Box    *dummy1, *dummy2;
  Fl_Button *but_cancel;
  std::vector<std::string> counter_v, radiolist_v;
  int rbcount = 0;

  split(radiolist_options, DEFAULT_DELIMITER, radiolist_v);

  for (size_t i = 0; i < radiolist_v.size(); ++i)
  {
    std::stringstream ss;
    ss << (i + 1);
    counter_v.push_back(ss.str());
    rbcount++;
  }

  if (rbcount < 1)
  {
    title = (char *)"error: radiolist";
    msg = (char *)"Two or more options required!";
    dialog_fl_message(ALERT);
    exit(1);
  }

  Fl_Radio_Round_Button *rb[rbcount];

  if (title == NULL)
  {
    title = (char *)"Select an option";
  }

  int win_h = (30 * rbcount) + 56;
  int mod_h = win_h - 40;

  radio_round_button_win = new Fl_Window(420, win_h, title);
  radio_round_button_win->callback(radio_round_button_cb, 1);
  {
    g = new Fl_Group(0, 0, 420, mod_h);
    {
      g_inside = new Fl_Group(0, 0, 420, mod_h);
      {
        for (int i = 0; i < rbcount; ++i)
        {
          char *p = NULL;
          rb[i] = new Fl_Radio_Round_Button(10, 10 + (30 * i), 400, 30, radiolist_v[i].c_str());

          if (return_number)
          {
            p = (char *)counter_v[i].c_str();
          }
          else
          {
            p = (char *)rb[i]->label();
          }
          rb[i]->callback(rb_callback, (char *)p);
        }
        dummy1 = new Fl_Box(10, mod_h - 2, 400, 1);
        dummy1->box(FL_NO_BOX);
      }
      g_inside->resizable(dummy1);
      g_inside->end();
    }
    g->resizable(g_inside);
    g->end();

    buttongroup = new Fl_Group(0, mod_h, 420, 36);
    {
      dummy2 = new Fl_Box(219, mod_h, 1, 1);
      dummy2->box(FL_NO_BOX);
      radiolist_but_ok = new Fl_Return_Button(220, mod_h + 4, 90, 26, fl_ok);
      radiolist_but_ok->deactivate();
      radiolist_but_ok->callback(radio_round_button_cb, 0);
      but_cancel = new Fl_Button(320, mod_h + 4, 90, 26, fl_cancel);
      but_cancel->callback(radio_round_button_cb, 1);
    }
    buttongroup->resizable(dummy2);
    buttongroup->end();
  }
  if (resizable)
  {
    radio_round_button_win->resizable(g);
  }
  radio_round_button_win->end();
  radio_round_button_win->show();
  Fl::run();

  if (ret == 0)
  {
    std::cout << radiolist_return << std::endl;
  }
  return ret;
}

