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
#include <FL/fl_ask.H>  /* fl_ok, fl_cancel */
#include <FL/Fl_Button.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Window.H>

#include <string>    /* std::string */
#include <iostream>  /* std::cout, std::cerr, std::endl */
#include <stdlib.h>  /* exit */
#include <string.h>  /* strlen */
#include <vector>    /* std::vector */

#include "split.hpp"  /* split */


Fl_Button *radiolist_but_ok;
const char *radiolist_return = NULL;
bool radiolist_but_ok_activated = false;

static void rb_exit0_cb(Fl_Widget*)
{
  std::cout << radiolist_return << std::endl;
  exit(0);
}

static void rb_exit1_cb(Fl_Widget*)
{
  exit(1);
}

static void rb_callback(Fl_Widget *w)
{
  if (!radiolist_but_ok_activated)
  {
    radiolist_but_ok_activated = true;
    radiolist_but_ok->activate();
  }
  radiolist_return = w->label();
}

int dialog_fl_radio_round_button(std::string  radiolist_options,
                                  const char *radiolist_title)
{
  Fl_Window *w;
  Fl_Button *but_cancel;
  std::vector<std::string> v;
  int rbcount = 0;

  split(radiolist_options, '|', v);
  for (size_t i = 0; i < v.size(); ++i)
  {
    rbcount++;
  }

  if (rbcount < 1)
  {
    std::cerr << "error: need two or more options" << std::endl;
    exit(1);
  }

  Fl_Radio_Round_Button *rb[rbcount];

  if (radiolist_title == NULL)
  {
    radiolist_title = (char *)"Select an option";
  }

  int bord = 10;
  int butw = 100;
  int buth = 26;
  int radh = 30;
  int winw = 420;
  int winh = radh * rbcount + buth + bord*3;

  w = new Fl_Window(winw, winh, radiolist_title);
  w->begin();
  w->callback(rb_exit1_cb);
  {
    for (int i = 0; i < rbcount; i++)
    {
      rb[i] = new Fl_Radio_Round_Button(bord, bord+i*radh, winw-bord*2, radh, v[i].c_str());
      rb[i]->callback(rb_callback);
    }

    radiolist_but_ok = new Fl_Button(winw-butw*2-bord*2, winh-buth-bord, butw, buth, fl_ok);
    radiolist_but_ok->deactivate();
    radiolist_but_ok->callback(rb_exit0_cb);

    but_cancel = new Fl_Button(winw-butw-bord, winh-buth-bord, butw, buth, fl_cancel);
    but_cancel->callback(rb_exit1_cb);
  }
  w->end();
  w->show();

  return Fl::run();
}

