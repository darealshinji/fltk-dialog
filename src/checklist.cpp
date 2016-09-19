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
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Window.H>

#include <string>    /* std::string, size, length, substr, c_str */
#include <iostream>  /* std::cout, std::endl */
#include <sstream>   /* std::stringstream, str */
#include <vector>    /* std::vector */
#include <stdlib.h>  /* exit, atoi */

#include "fltk-dialog.hpp"
#include "misc/split.hpp"


Fl_Button *checklist_but_ok;

/* vector returning a '|' separated list of all
 * options that have been checked by the user */
std::vector<std::string> checklist_v;

/* set entries true/false on selecting check button */
bool checklist_checked[255];

int check_button_count = 0;


/* int to std::string */
std::string itostr(int i)
{
  std::stringstream ss;
  ss << i;
  std::string str = ss.str();
  return str;
}

static void cb_callback(Fl_Widget *w, void *p)
{
  (void) w;
  /* int->char->int because I can't use int directly */
  int i = atoi((char *)p);

  if (checklist_checked[i])
  {
    checklist_checked[i] = false;
  }
  else
  {
    checklist_checked[i] = true;
  }
}

static void cb_exit0_cb(Fl_Widget *w)
{
  (void) w;
  std::string list;

  for (int i = 0; i < check_button_count; i++)
  {
    if (checklist_checked[i])
    {
      list += checklist_v[i] + "|";
    }
  }

  /* strip trailing "|" */
  int len = list.length() - 1;
  std::cout << list.substr(0, len) << std::endl;

  exit(0);
}

static void cb_exit1_cb(Fl_Widget*)
{
  exit(1);
}

int dialog_fl_check_button(std::string checklist_options)
{
  Fl_Window *w;
  Fl_Button *but_cancel;
  std::vector<std::string> v;
  int empty_lines = 0;

  split(checklist_options, '|', checklist_v);

  for (size_t i = 0; i < checklist_v.size(); ++i)
  {
    if (checklist_v[i] == "")
    {
      empty_lines++;
    }

    v.push_back(itostr(i));
    checklist_checked[i] = false;
    check_button_count++;
  }

  Fl_Check_Button *rb[check_button_count];

  if (title == NULL)
  {
    title = (char *)"Select your option(s)";
  }

  int bord = 10;
  int butw = 100;
  int buth = 26;
  int chkh = 30;
  int winw = 420;
  int winh = chkh * (check_button_count - empty_lines) + buth + bord*3;

  w = new Fl_Window(winw, winh, title);
  w->begin();
  w->callback(cb_exit1_cb);
  {
    int j = 0;
    for (int i = 0; i < check_button_count; ++i)
    {
      if (checklist_v[i] == "")
      {
        --j;
      }
      else
      {
        rb[i] = new Fl_Check_Button(bord, bord+j*chkh, winw-bord*2, chkh, checklist_v[i].c_str());
        rb[i]->callback(cb_callback, (char *)v[i].c_str());
      }
      ++j;
    }

    checklist_but_ok = new Fl_Button(winw-butw*2-bord*2, winh-buth-bord, butw, buth, fl_ok);
    checklist_but_ok->callback(cb_exit0_cb);

    but_cancel = new Fl_Button(winw-butw-bord, winh-buth-bord, butw, buth, fl_cancel);
    but_cancel->callback(cb_exit1_cb);
  }
  w->end();
  w->show();

  return Fl::run();
}

