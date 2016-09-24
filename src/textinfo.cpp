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
#include <FL/fl_ask.H>  /* fl_ok, fl_cancel, fl_close */
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Check_Button.H>

#include <string>    /* std::string, std::getline, c_str */
#include <iostream>  /* std::cin */
#include <stdlib.h>  /* exit */

#include "fltk-dialog.hpp"
#include "misc/readstdio.hpp"


Fl_Multi_Browser *ti_browser;
Fl_Return_Button *ti_but_ok = NULL;
Fl_Button        *ti_but_cancel = NULL;
bool ti_checkbutton_set = false;

static void textinfo_exit0_cb(Fl_Widget*)
{
  delete ti_browser;
  exit(0);
}

static void textinfo_exit1_cb(Fl_Widget*)
{
  delete ti_browser;
  exit(1);
}

static void ti_checkbutton_cb(Fl_Widget*)
{
  if (ti_checkbutton_set == true)
  {
    ti_checkbutton_set = false;
    ti_but_ok->deactivate();
  }
  else
  {
    ti_checkbutton_set = true;
    ti_but_ok->activate();
  }
}

int dialog_textinfo(       bool autoscroll,
                    std::string checkbox)
{
  Fl_Window *win;
  Fl_Check_Button *checkbutton;

  std::string line;
  int linecount = 0;
  int bord = 10;
  int winw = 400;
  int winh = 500;
  int butw = 100;
  int buth = 26;
  int browser_w = winw-bord*2;
  int browser_h = (checkbox == "") ? winh-buth-bord*3 : winh-buth*2-bord*3;

  if (title == NULL)
  {
    title = (char *)"FLTK text info window";
  }

  win = new Fl_Window(winw, winh, title);
  {
    ti_browser = new Fl_Multi_Browser(bord, bord, browser_w, browser_h);
    win->resizable(ti_browser);

    if (checkbox == "")
    {
      win->callback(textinfo_exit0_cb);  /* exit(0) */
      ti_but_ok = new Fl_Return_Button(winw-butw-bord, browser_h+bord*2,
                                       butw, buth, fl_close);
      ti_but_ok->callback(textinfo_exit0_cb);
    }
    else
    {
      win->callback(textinfo_exit1_cb);  /* exit(1) */
      checkbox = " " + checkbox;
      checkbutton = new Fl_Check_Button(bord, browser_h+bord, ti_browser->w(),
                                        buth, checkbox.c_str());
      checkbutton->callback(ti_checkbutton_cb);
      ti_but_cancel = new Fl_Button(winw-butw-bord, browser_h+buth+bord*2,
                                    butw, buth, fl_cancel);
      ti_but_cancel->callback(textinfo_exit1_cb);
      ti_but_ok = new Fl_Return_Button(winw-butw*2-bord*2, browser_h+buth+bord*2,
                                       butw, buth, fl_ok);
      ti_but_ok->deactivate();
      ti_but_ok->callback(textinfo_exit0_cb);
    }
  }
  win->show();

  int stdin;
  READSTDIO(stdin);

  if (stdin)
  {
    win->wait_for_expose();
    Fl::flush();
    for (/**/; std::getline(std::cin, line); /**/)
    {
      ti_browser->add(line.c_str());
      if (autoscroll)
      {
        linecount++;
        ti_browser->bottomline(linecount);
      }
      Fl::check();
    }
  }
  else if (stdin == -1)
  {
    line = "error: select()";
    ti_browser->add(line.c_str());
  }
  else
  {
    line = "error: no input";
    ti_browser->add(line.c_str());
  }

  int ret = Fl::run();
  delete ti_browser;
  return ret;
}

