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
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Double_Window.H>
#include <FL/filename.H>

#include <string>

#include "fltk.xpm"
#include "fltk-dialog.hpp"

//#define PATCHES_TAB
#include "about.hpp"


static Fl_Double_Window *about_win;

static std::string about_text = "\n"
    "- FLTK dialog -\n"
    "run dialog boxes from shell scripts\n"
    "\n"
    "Using FLTK version " + get_fltk_version() + "\n"
    "\n" //http://www.fltk.org\n"
    "\n"
    /* http://www.utf8-chartable.de/ */
    "Copyright \xc2\xa9 2016 djcj <djcj@gmx.de>\n"
    "\n" //https://github.com/darealshinji/fltk-dialog\n"
    "\n"
#ifdef WITH_FONT
    "The FLTK library and the font widget are\n"
    "copyright \xc2\xa9 1998-2016 by Bill Spitzak and others.\n"
#else
    "The FLTK library is copyright \xc2\xa9 1998-2016 by\n"
    "Bill Spitzak and others.\n"
#endif
    "\n"
    "The calendar widget is copyright \xc2\xa9 1999-2000\n"
    "by the Flek development team and\n"
    "copyright \xc2\xa9 2016 by djcj <djcj@gmx.de>\n"
    "\n"
#ifdef WITH_DEFAULT_ICON
    "The application icon is copyright \xc2\xa9 2016 by Haiku, Inc."
#else
    "\n"
#endif
/* about_text end */;

class uri_box : public Fl_Box
{
  public:

    uri_box(int X, int Y, int W, int H, const char *L=0)
      : Fl_Box(X, Y, W, H, L) { }

    virtual ~uri_box() { }

    int handle(int event);
};

int uri_box::handle(int event)
{
  int ret = Fl_Box::handle(event);
  switch (event)
  {
    case FL_PUSH:
      do_callback();
      break;
    case FL_MOVE:
      fl_cursor(FL_CURSOR_HAND);
      break;
    case FL_LEAVE:
      fl_cursor(FL_CURSOR_DEFAULT);
      break;
  }
  return ret;
}

static void open_uri_cb(Fl_Widget *, void *p)
{
  const char *uri = (char *)p;
  char errmsg[512];
  char warnmsg[768];

  if (!fl_open_uri(uri, errmsg, sizeof(errmsg)))
  {
    sprintf(warnmsg, "Error: %s", errmsg);
    title = "Error";
    msg = warnmsg;
    dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO);
  }
}

static void about_close_cb(Fl_Widget *)
{
  about_win->hide();
}

int about()
{
  Fl_Tabs          *about_tab;
  Fl_Group         *g_about, *g_license;
  Fl_Pixmap        *about_pixmap;
  Fl_Box           *box;
  Fl_Text_Buffer   *license_buffer;
  Fl_Text_Display  *license_display;
  uri_box          *uri_button1, *uri_button2;
  Fl_Return_Button *but_close;
#ifdef PATCHES_TAB
  Fl_Group *g_patches;
  Fl_Text_Buffer *patches_buffer;
  Fl_Text_Display *patches_display;
#endif

  const char *uri1 = "http://www.fltk.org";
  const char *uri2 = "https://github.com/darealshinji/fltk-dialog";
  const char *about_text_c = about_text.c_str();

  about_pixmap = new Fl_Pixmap(fltk_xpm);

  about_win = new Fl_Double_Window(450, 490, "About FLTK dialog");
  about_win->callback(about_close_cb);
  {
    about_tab = new Fl_Tabs(10, 10, 430, 434);
    {
      g_about = new Fl_Group(10, 40, 430, 404, "About");
      {
        box = new Fl_Box(10, 40, 430, 404, about_text_c);
        box->box(FL_NO_BOX);
        box->image(about_pixmap);

        /* x = (winw/2) - (boxw/2) */
        uri_button1 = new uri_box(157, 210, 136, 22, uri1);
        uri_button1->box(FL_NO_BOX);
        uri_button1->labelcolor(FL_BLUE);
        uri_button1->clear_visible_focus();
        uri_button1->callback(open_uri_cb, (void *)uri1);

        uri_button2 = new uri_box(75, 260, 300, 22, uri2);
        uri_button2->box(FL_NO_BOX);
        uri_button2->labelcolor(FL_BLUE);
        uri_button2->clear_visible_focus();
        uri_button2->callback(open_uri_cb, (void *)uri2);
      }
      g_about->end();

      g_license = new Fl_Group(10, 40, 430, 404, "License");
      {
        license_buffer = new Fl_Text_Buffer();
        license_display = new Fl_Text_Display(10, 40, 430, 404);
        license_display->buffer(license_buffer);
        license_buffer->text(license_buffer_text);
      }
      g_license->end();

#ifdef PATCHES_TAB
      g_patches = new Fl_Group(10, 40, 430, 404, "Patches");
      {
        patches_buffer = new Fl_Text_Buffer();
        patches_display = new Fl_Text_Display(10, 40, 430, 404);
        patches_display->textfont(FL_COURIER);
        patches_display->buffer(patches_buffer);
        patches_buffer->text(patches_buffer_text);
      }
      g_patches->end();
#endif

    }
    about_tab->end();

    but_close = new Fl_Return_Button(0,0,0,0, fl_close);
    int but_w;
    measure_button_width(but_close, but_w, 40);
    but_close = new Fl_Return_Button(440 - but_w, 454, but_w, 28, fl_close);
    but_close->callback(about_close_cb);
  }
  set_position(about_win);
  about_win->end();
  about_win->show();
  set_undecorated(about_win);
  Fl::run();

  return 0;
}

