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
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Single_Window.H>
#include <FL/x.H>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "icon_png.h"

#define INIT_SIZE 1

class nobox_Fl_Menu_Button : public Fl_Menu_Button
{
public:
  nobox_Fl_Menu_Button(int X, int Y, int W, int H)
    : Fl_Menu_Button(X, Y, W, H) { }

protected:
  void draw() {
    draw_box(FL_NO_BOX, color());
    draw_label();
  }
};

static Fl_Single_Window *win;
static nobox_Fl_Menu_Button *but;
static const char *command, *icon;
static bool force_nanosvg;

static void set_size(void *)
{
  Fl_RGB_Image *rgb = NULL;

  int w = win->w();
  int h = win->h();

  if (w == INIT_SIZE || h == INIT_SIZE) {
    Fl::repeat_timeout(0.001, set_size);
    return;
  }

  if (w > h) {
    h = w;
  }

  win->size(h, h);
  but->size(h, h);

  if (icon && strlen(icon) > 0) {
    rgb = img_to_rgb(icon, force_nanosvg);
  }

  if (!rgb) {
    rgb = new Fl_PNG_Image(NULL, src_icon_png, src_icon_png_len);
  }

  /* make icon a bit smaller than the area */
  if ((h *= 0.72) > 0) {
    but->image(rgb->copy(h, h));
  }

  win->redraw();

  if (rgb) {
    delete rgb;
  }
}

static void callback(Fl_Widget *, void *)
{
  win->hide();

  if (command && strlen(command) > 0) {
    execl("/bin/sh", "sh", "-c", command, NULL);
    _exit(127);
  }
}

static void close_cb(Fl_Widget *, void *) {
  win->hide();
}

static int create_tray_entry(void)
{
  Fl_Color flcol = 0;
  Window dock;
  XColor xcol;
  XImage *image;
  XEvent ev;
  char atom_tray_name[128];

  Fl_Menu_Item menu_items[] = {
    { "Run command", 0, callback, 0,0, FL_NORMAL_LABEL, 0, 14, 0 },
    { "Close",       0, close_cb, 0,0, FL_NORMAL_LABEL, 0, 14, 0 },
    { 0,0,0,0,0,0,0,0,0 }
  };

  win = new Fl_Single_Window(INIT_SIZE, INIT_SIZE);
  but = new nobox_Fl_Menu_Button(0, 0, INIT_SIZE, INIT_SIZE);
  but->menu(menu_items);
  if (msg) {
    but->tooltip(msg);
  }
  win->end();
  win->clear_border();
  win->show();
  if (!win->shown()) {
    return 1;
  }

  snprintf(atom_tray_name, sizeof(atom_tray_name), "_NET_SYSTEM_TRAY_S%i", fl_screen);
  dock = XGetSelectionOwner(fl_display, XInternAtom(fl_display, atom_tray_name, False));
  if (!dock) {
    return 1;
  }

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

  /* get tray color and set as background */
  image = XGetImage(fl_display, RootWindow(fl_display, DefaultScreen(fl_display)),
                    win->x() + 1, win->y() + 1, 1, 1, AllPlanes, XYPixmap);
  xcol.pixel = XGetPixel(image, 0, 0);
  XFree(image);

  XQueryColor(fl_display, DefaultColormap(fl_display, DefaultScreen(fl_display)), &xcol);
  Fl::set_color(flcol, xcol.red >> 8, xcol.green >> 8, xcol.blue >> 8);
  win->color(flcol);

  Fl::add_timeout(0.005, set_size);

  return Fl::run();
}

int dialog_indicator(const char *command_, const char *icon_, bool force_nanosvg_)
{
  command = command_;
  icon = icon_;
  force_nanosvg = force_nanosvg_;

  if (create_tray_entry() != 0) {
    std::cerr << "error: cannot create tray/indicator entry" << std::endl;
    return 1;
  }

  return 0;
}

