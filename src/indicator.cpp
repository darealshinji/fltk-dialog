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

// TODO: get tray area height

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/x.H>

#include <iostream>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "default_tray_icon_png.h"

class click_box : public Fl_Box
{
public:
  click_box(int X, int Y, int W, int H)
    : Fl_Box(X, Y, W, H) { }

  int handle(int event) {
    if (event == FL_RELEASE) {
      do_callback();
      return 1;
    }
    return Fl_Box::handle(event);
  }
};

static Fl_Double_Window *win;
static const char *command;

static void callback(Fl_Widget *)
{
  win->hide();

  /* right and middle mouse button only remove the indicator */
  if (command && strlen(command) > 0 && Fl::event_button() == FL_LEFT_MOUSE) {
    execl("/bin/sh", "sh", "-c", command, NULL);
    _exit(127);
  }
}

static int create_tray_entry(const char *icon, bool force_nanosvg)
{
  click_box *box;
  Fl_RGB_Image *rgb = NULL;
  Fl_Color flcol = 0;
  Window dock;
  XColor xcol;
  XImage *image;
  XEvent ev;

  std::string s = "run command: ";
  const char *tt;
  char atom_tray_name[128];

  if (msg) {
    tt = msg;
  } else {
    s.append(command);
    tt = s.c_str();
  }

  snprintf(atom_tray_name, sizeof(atom_tray_name), "_NET_SYSTEM_TRAY_S%i", fl_screen);
  dock = XGetSelectionOwner(fl_display, XInternAtom(fl_display, atom_tray_name, False));
  if (!dock) {
    return 1;
  }

  if (icon && strlen(icon) > 0) {
    rgb = img_to_rgb(icon, force_nanosvg);
  }

  win = new Fl_Double_Window(32, 32);
  box = new click_box(0, 0, win->w(), win->h());
  if (rgb) {
    box->image(rgb->copy(22, 22));
    delete rgb;
  } else {
    box->image(new Fl_PNG_Image(NULL, src_default_tray_icon_png, src_default_tray_icon_png_len));
  }
  box->tooltip(tt);
  box->callback(callback);
  win->clear_border();
  win->end();
  win->show();

  memset(&ev, 0, sizeof(ev));
  ev.xclient.type = ClientMessage;
  ev.xclient.window = dock;
  ev.xclient.message_type = XInternAtom(fl_display, "_NET_SYSTEM_TRAY_OPCODE", False);
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = fl_event_time;
  ev.xclient.data.l[1] = 0;  // SYSTEM_TRAY_REQUEST_DOCK
  ev.xclient.data.l[2] = fl_xid(win);
  ev.xclient.data.l[3] = 0;
  ev.xclient.data.l[4] = 0;
  XSendEvent(fl_display, dock, False, NoEventMask, &ev);
  XSync(fl_display, False);

  image = XGetImage(fl_display, RootWindow(fl_display, DefaultScreen(fl_display)),
                    win->x() + 1, win->y() + 1, 1, 1, AllPlanes, XYPixmap);
  xcol.pixel = XGetPixel(image, 0, 0);
  XFree(image);

  XQueryColor(fl_display, DefaultColormap(fl_display, DefaultScreen(fl_display)), &xcol);
  Fl::set_color(flcol, xcol.red >> 8, xcol.green >> 8, xcol.blue >> 8);
  win->color(flcol);
  win->redraw();

  return Fl::run();
}

int dialog_indicator(const char *command_, const char *icon, bool force_nanosvg)
{
  command = command_;

  if (create_tray_entry(icon, force_nanosvg) != 0) {
    std::cerr << "error: cannot create tray/indicator entry" << std::endl;
    return 1;
  }
  return 0;
}

