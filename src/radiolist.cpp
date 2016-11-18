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
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Window.H>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "fltk-dialog.hpp"
#include "misc/split.hpp"
#include "misc/Fl_Select_Browser2.H"


static Fl_Window        *radio_round_button_win;
static Fl_Return_Button *radiolist_but_ok;
static int radiolist_return = 0;
static bool radiolist_but_ok_activated = false;

static void radio_round_button_cb(Fl_Widget *, long p)
{
  radio_round_button_win->hide();
  ret = (int) p;
}

static void radiolist_callback(Fl_Widget *v)
{
  if (!radiolist_but_ok_activated)
  {
    radiolist_but_ok_activated = true;
    radiolist_but_ok->activate();
  }

  Fl_Select_Browser2 *b = (Fl_Select_Browser2 *)v;
  radiolist_return = b->value();
}

int dialog_radiolist(std::string radiolist_options,
                            bool return_number)
{
  Fl_Group           *g, *g_inside, *buttongroup;
  Fl_Box             *dummy1, *dummy2;
  Fl_Button          *but_cancel;
  Fl_Select_Browser2 *browser;

  std::vector<std::string> radiolist_v;
  split(radiolist_options, separator, radiolist_v);

  int count = 0;
  for (/**/; (size_t) count < radiolist_v.size(); ++count) {}

  if (count < 1)
  {
    title = "error: radiolist";
    msg = "Two or more options required!";
    dialog_message(fl_ok, fl_cancel, NULL, MESSAGE_TYPE_WARNING);
    return 1;
  }

  if (title == NULL)
  {
    title = "Select an option";
  }

  radio_round_button_win = new Fl_Window(420, 356, title);
  radio_round_button_win->callback(radio_round_button_cb, 1);
  {
    g = new Fl_Group(0, 0, 420, 290);
    {
      g_inside = new Fl_Group(0, 0, 420, 290);
      {
        for (int i = 0; i < count; ++i)
        {
          browser = new Fl_Select_Browser2(10, 10, 400, 289);
          browser->when(FL_WHEN_CHANGED);
          browser->box(FL_THIN_DOWN_BOX);
          browser->color(fl_lighter(fl_lighter(FL_BACKGROUND_COLOR)));
          browser->clear_visible_focus();
          for (int i = 0; i < count; ++i)
          {
            browser->add(radiolist_v[i].c_str());
          }
          browser->callback(radiolist_callback);
        }
        dummy1 = new Fl_Box(10, 288, 400, 1);
        dummy1->box(FL_NO_BOX);
      }
      g_inside->resizable(dummy1);
      g_inside->end();
    }
    g->resizable(g_inside);
    g->end();

    buttongroup = new Fl_Group(0, 310, 420, 36);
    {
      dummy2 = new Fl_Box(199, 310, 1, 1);
      dummy2->box(FL_NO_BOX);
      radiolist_but_ok = new Fl_Return_Button(200, 314, 100, 26, fl_ok);
      radiolist_but_ok->deactivate();
      radiolist_but_ok->callback(radio_round_button_cb, 0);
      but_cancel = new Fl_Button(310, 314, 100, 26, fl_cancel);
      but_cancel->callback(radio_round_button_cb, 1);
    }
    buttongroup->resizable(dummy2);
    buttongroup->end();
  }
  set_size(radio_round_button_win, g);
  set_position(radio_round_button_win);
  radio_round_button_win->end();
  radio_round_button_win->show();
  Fl::run();

  if (ret == 0)
  {
    if (return_number)
    {
      std::cout << radiolist_return << std::endl;
    }
    else
    {
      std::cout << browser->text(radiolist_return) << std::endl;
    }
  }
  return ret;
}

