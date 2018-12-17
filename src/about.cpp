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

#include "fltk-dialog.hpp"
#include "about_license.hpp"
#include "fltk_png.h"

static Fl_Double_Window *win;
static Fl_Text_Buffer *buffer;
static bool license_displayed = false;
static const char *label_license = "Licenses";

static const char *text = "\n"
  "FLTK dialog: run dialog boxes from shell scripts\n"
  "\n"
  "Using FLTK version " FLTK_VERSION_STRING "\n"
  "http://www.fltk.org\n"
  "\n"
  /* http://www.utf8-chartable.de/ */
  "Copyright \xc2\xa9 2016-2018 djcj <djcj@gmx.de>\n"
  PROJECT_URL "\n"
  "\n"
  "The FLTK library and the font widget are\n"
  "copyright \xc2\xa9 1998-2016 by Bill Spitzak and others.\n"
  "\n"
  #ifdef WITH_FRIBIDI
  "The FriBidi parsing code is copyright \xc2\xa9 2003-2011\n"
  "by the FFmpeg developers and contributors.\n"
  "\n"
  #endif
  "The application icon is copyright \xc2\xa9 2016 by Haiku, Inc.";

static void callback(Fl_Widget *o)
{
  if (license_displayed) {
    o->label(label_license);
    buffer->text(text);
    license_displayed = false;
  } else {
    o->label("About");
    buffer->text(license);
    license_displayed = true;
  }
}

static void url_cb(Fl_Widget *)
{
  char errmsg[512] = {0};
  std::string warnmsg;
  if (!fl_open_uri(PROJECT_URL, errmsg, sizeof(errmsg))) {
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
  const int w = 500, h = 500;
  int range_w = 40;

  Fl_PNG_Image *logo = new Fl_PNG_Image(NULL, src_fltk_png, src_fltk_png_len);
  int logo_h = logo->h();

  win = new Fl_Double_Window(w, h, "FLTK dialog");
  win->callback(close_cb);
  {
    box = new Fl_Box(0, 20, w, logo_h);
    box->box(FL_NO_BOX);
    box->image(logo);

    g1 = new Fl_Group(10, logo_h + 40, w - 20, h - logo_h - 90);
    {
      buffer = new Fl_Text_Buffer();
      display = new Fl_Text_Display(10, logo_h + 40, w - 20, h - logo_h - 90);
      display->buffer(buffer);
      buffer->text(text);
    }
    g1->end();

    g2 = new Fl_Group(0, h - 40, w, 40);
    {
      int but_w = measure_button_width(label_license, 20);
      but_lic = new Fl_Button(10, h - 40, but_w, 28, label_license);
      but_lic->callback(callback);
      range_w += but_w;

      const char *label_homepage = "Homepage";
      but_w = measure_button_width(label_homepage, 20);
      but_hp = new Fl_Button(but_lic->w() + 20, h - 40, but_w, 28, label_homepage);
      but_hp->callback(url_cb);
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
  run_window(win, g1, range_w, logo_h + 130);

  return 0;
}

