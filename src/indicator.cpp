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
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Single_Window.H>
#include <FL/x.H>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "icon_png.h"

/* Issue: starting too many instances makes the icons disappear or their size uneven */

class simple_button : public Fl_Box
{
public:
  simple_button(int X, int Y, int W, int H, const char *L=0)
    : Fl_Box(X, Y, W, H, L)
  { }

  int handle(int event) {
    if (event == FL_PUSH) {
      do_callback();
    }
    return Fl_Box::handle(event);
  }
};

class menu_window : public Fl_Single_Window
{
public:
  menu_window(int X, int Y, int W, int H, const char *L=0)
    : Fl_Single_Window(X, Y, W, H, L)
  { }

  int handle(int event) {
    if (event == FL_UNFOCUS) {
      hide();
      do_callback();
    }
    return Fl_Single_Window::handle(event);
  }
};

class menu_entry : public Fl_Button
{
public:
  menu_entry(int X, int Y, int W, int H, const char *L=0);

  int handle(int event) {
    switch (event) {
      case FL_ENTER:
        color(selection_color());
        labelcolor(FL_WHITE);
        parent()->redraw();
        break;
      case FL_LEAVE:
        color(FL_BACKGROUND_COLOR);
        labelcolor(FL_BLACK);
        parent()->redraw();
        break;
    }
    return Fl_Button::handle(event);
  }
};

menu_entry::menu_entry(int X, int Y, int W, int H, const char *L)
 : Fl_Button(X, Y, W, H, L)
{
  box(FL_FLAT_BOX);
  down_box(FL_FLAT_BOX);
  align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
  selection_color(FL_BLUE);
  clear_visible_focus();
}

#define INIT_SIZE 0

static Fl_Single_Window *win;
static simple_button *but;
static menu_window *menu = NULL;
static const char *command, *icon;
static bool force_nanosvg;
static int it = 0;

static void popup_cb(Fl_Widget *);
static void popdown_cb(Fl_Widget *);
static void menu_cb(Fl_Widget *);
static void close_cb(Fl_Widget *, long exec_command);

static void init_menu(void)
{
  menu = new menu_window(0, 0, 140, 52);
  {
    menu_entry *m1 = new menu_entry(0,  0, 140, 26, "  Run command");
    menu_entry *m2 = new menu_entry(0, 26, 140, 26, "  Quit");
    m1->callback(close_cb, 1);
    m2->callback(close_cb, 0);

    if (command) {
      char buf[36];
      const char *appendix = "[...]";
      const int len = 5;  /* strlen(appendix) */
      const int limit = sizeof(buf) - 1;

      if (strlen(command) > limit) {
        strncpy(buf, command, limit - len);
        strcat(buf, appendix);
        m1->copy_tooltip(buf);
      } else {
        m1->tooltip(command);
      }
    }
  }
  menu->callback(menu_cb);
  menu->end();
  menu->border(0);
}

static void menu_cb(Fl_Widget *) {
  //menu->hide();
  but->box(FL_NO_BOX);
  but->callback(popup_cb);
  win->redraw();
}

static void popup_cb(Fl_Widget *)
{
  if (win->w() != win->h()) {
    std::cerr << "error: unexpected uneven button size:"
      " w=" << win->w() << ", h=" << win->h() << std::endl;
  }
  if (!menu) {
    init_menu();
  }
  but->box(FL_FLAT_BOX);
  but->callback(popdown_cb);
  win->redraw();
  menu->position(Fl::event_x_root() - Fl::event_x(), Fl::event_y_root() - Fl::event_y());
  menu->show();
}

static void popdown_cb(Fl_Widget *) {
  menu->hide();
  but->box(FL_NO_BOX);
  but->callback(popup_cb);
  win->redraw();
}

static void close_cb(Fl_Widget *, long exec_command)
{
  menu->hide();
  win->hide();

  if (exec_command && command && strlen(command) > 0) {
    execl("/bin/sh", "sh", "-c", command, NULL);
    _exit(127);
  }
}

static void set_size(void *)
{
  Fl_RGB_Image *rgb = NULL;

  int w = win->w();
  int h = win->h();

  if (w == INIT_SIZE || h == INIT_SIZE) {
    it++;
    /* don't loop forever */
    if (it < 5000) {
      Fl::repeat_timeout(0.001, set_size);
    }
    return;
  }

  int n = (w > h) ? w : h;

  win->size(n, n);
  but->size(n, n);

  if (icon && strlen(icon) > 0) {
    rgb = img_to_rgb(icon, force_nanosvg);
  }

  if (!rgb) {
    rgb = new Fl_PNG_Image(NULL, src_icon_png, src_icon_png_len);
  }

  /* make icon a bit smaller than the area */
  if ((n *= 0.72) > 0) {
    but->image(rgb->copy(n, n));
  }

  if (rgb) {
    delete rgb;
  }
}

static int create_tray_entry(void)
{
  Fl_Color flcol = 0;
  Window dock;
  XEvent ev;
  XColor xcol;
  XImage *image;
  char atom_tray_name[128];

  snprintf(atom_tray_name, sizeof(atom_tray_name), "_NET_SYSTEM_TRAY_S%i", fl_screen);
  dock = XGetSelectionOwner(fl_display, XInternAtom(fl_display, atom_tray_name, False));
  if (!dock) {
    return 1;
  }

  win = new Fl_Single_Window(INIT_SIZE, INIT_SIZE);
  {
    but = new simple_button(0, 0, INIT_SIZE, INIT_SIZE);
    but->box(FL_NO_BOX);
    if (msg) {
      but->tooltip(msg);
    }
    but->callback(popup_cb);
  }
  win->when(FL_WHEN_NEVER);
  win->end();
  win->border(0);
  win->show();
  if (!win->shown()) {
    return 1;
  }

  /* dock window */
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
  but->color(fl_lighter(flcol));
  win->color(flcol);

  Fl::add_timeout(0.005, set_size);  /* 5 ms */

  return 0;
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

  Fl::run();

  return 0;
}

