/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018, djcj <djcj@gmx.de>
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
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Double_Window.H>
#include <FL/filename.H>

#include "fltk-dialog.hpp"
#include "about_license.hpp"

#include "fltk_png.h"
#define FLTK_PNG_WIDTH  229
#define FLTK_PNG_HEIGHT 70

static Fl_Double_Window *win;
static Fl_Text_Buffer *buffer;
static bool license_displayed = false;

static std::string text = "\n"
    "FLTK dialog: run dialog boxes from shell scripts\n"
    "\n"
    "Using FLTK version " + get_fltk_version() + "\n"
    "http://www.fltk.org\n"
    "\n"
    /* http://www.utf8-chartable.de/ */
    "Copyright \xc2\xa9 2016-2018 djcj <djcj@gmx.de>\n"
    "https://github.com/darealshinji/fltk-dialog\n"
    "\n"
    "The FLTK library and the font widget are\n"
    "copyright \xc2\xa9 1998-2016 by Bill Spitzak and others.\n"
    "\n"
    "The calendar widget is copyright \xc2\xa9 1999-2000\n"
    "by the Flek development team and\n"
    "copyright \xc2\xa9 2016-2018 by djcj <djcj@gmx.de>\n"
    "\n"
#if defined(WITH_FRIBIDI) && !defined(SYSTEM_FRIBIDI)
    "This build is using FriBidi, released under GNU LGPL 2.1\n"
    "https://github.com/fribidi/fribidi\n"
    "\n"
#endif
    "The application icon is copyright \xc2\xa9 2016 by Haiku, Inc.";

static void callback(Fl_Widget *o)
{
  if (license_displayed) {
    o->label("License");
    buffer->text(text.c_str());
    license_displayed = false;
  } else {
    o->label("About");
    buffer->text(license);
    license_displayed = true;
  }
}

static void url_cb(Fl_Widget *, void *p)
{
  const char *uri = (char *)p;
  char errmsg[512] = {0};
  std::string warnmsg;
  if (!fl_open_uri(uri, errmsg, sizeof(errmsg))) {
    title = "Error";
    warnmsg = "Error: " + std::string(errmsg);
    msg = warnmsg.c_str();
    dialog_message(MESSAGE_TYPE_INFO);
  }
}

static void close_cb(Fl_Widget *) {
  win->hide();
}

int about()
{
  Fl_Group *g1, *g2;
  Fl_Box *box, *dummy;
  Fl_Text_Display *display;
  Fl_Return_Button *but_close;
  Fl_Button *but_lic, *but_hp;
  int w = 500, h = 500, range_w = 40;

  win = new Fl_Double_Window(w, h, "FLTK dialog");
  win->callback(close_cb);
  {
    box = new Fl_Box(0, 20, w, FLTK_PNG_HEIGHT);
    box->box(FL_NO_BOX);
    box->image(new Fl_PNG_Image(NULL, src_fltk_png, (int)src_fltk_png_len));

    g1 = new Fl_Group(10, FLTK_PNG_HEIGHT + 40, w - 20, h - FLTK_PNG_HEIGHT - 90);
    {
      buffer = new Fl_Text_Buffer();
      display = new Fl_Text_Display(10, FLTK_PNG_HEIGHT + 40, w - 20, h - FLTK_PNG_HEIGHT - 90);
      display->buffer(buffer);
      buffer->text(text.c_str());
    }
    g1->end();

    g2 = new Fl_Group(0, h - 40, w, 40);
    {
      int but_w = measure_button_width("License", 20);
      but_lic = new Fl_Button(10, h - 40, but_w, 28, "License");
      but_lic->callback(callback);
      range_w += but_w;

      but_w = measure_button_width("Homepage", 20);
      but_hp = new Fl_Button(but_lic->w() + 20, h - 40, but_w, 28, "Homepage");
      but_hp->callback(url_cb, (void *)"https://github.com/darealshinji/fltk-dialog");
      range_w += but_w;

      but_w = measure_button_width(fl_close, 40);
      but_close = new Fl_Return_Button(w - but_w - 10, h - 40, but_w, 28, fl_close);
      but_close->callback(close_cb);
      range_w += but_w;

      dummy = new Fl_Box(but_close->x() - 1, h - 40, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    g2->resizable(dummy);
    g2->end();
  }
  win->size_range(range_w, FLTK_PNG_HEIGHT + 130, max_w, max_h);
  run_window(win, g1);

  return 0;
}

