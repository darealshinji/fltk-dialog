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

// pipe=$(mktemp -u)
// mkfifo $pipe
// exec 3<> $pipe
// echo "ICON:newIcon" >&3

#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "icon_png.h"
#include "indicator_gtk.h"

/* From the Tango icon theme, released into the Public Domain.
 * https://commons.wikimedia.org/wiki/File:Image-missing.svg
 */
#include "image_missing_png.h"

#define ICON_SCALE  0.72
#define ICON_MIN    4

class simple_button : public Fl_Box
{
public:
  simple_button(int X, int Y, int W, int H, const char *L=NULL)
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
  menu_window(int X, int Y, int W, int H, const char *L=NULL)
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
  menu_entry(int X, int Y, int W, int H, const char *L=NULL);

  int handle(int event) {
    switch (event) {
      case FL_ENTER:
        color(selection_color());
        labelcolor(FL_WHITE);
        parent()->redraw();
        break;
      case FL_LEAVE:
      case FL_PUSH:
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

static Fl_Single_Window *win = NULL;
static Fl_RGB_Image *rgb = NULL;
static simple_button *but = NULL;
static menu_window *menu = NULL;

static const char *command = NULL;
static int it = 0;
static bool listen, auto_close;
static pthread_t t1;

static void callback(Fl_Widget *);
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

    if (auto_close) {
      m1->callback(close_cb, 1);
    } else {
      m1->callback(callback);
    }
    m2->callback(close_cb, 0);

    if (command) {
      if (strlen(command) > 36) {
        std::string s = command;
        s.erase(31);
        s += "[...]";
        m1->copy_tooltip(s.c_str());
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

static void callback(Fl_Widget *) {
  if (command && strlen(command) > 0) {
    popdown_cb(NULL);
    int i = system(command);
    static_cast<void>(i);
  }
}

static void close_windows(void)
{
  if (menu) {
    menu->hide();
  }
  win->hide();

  if (rgb) {
    delete rgb;
  }
}

static void close_cb(Fl_Widget *, long exec_command)
{
  if (listen) {
    pthread_cancel(t1);
  }

  close_windows();

  if (exec_command && command && strlen(command) > 0) {
    execl("/bin/sh", "sh", "-c", command, NULL);
    _exit(127);
  }
}

static void set_size(void *)
{
  int w = win->w();
  int h = win->h();

  if (w == 0 || h == 0) {
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

  if (rgb) {
    /* make icon a bit smaller than the area */
    if ((n *= ICON_SCALE) > ICON_MIN) {
      but->image(rgb->copy(n, n));
    }

    delete rgb;
    rgb = NULL;
  }
}

static void check_icons(const char *icon)
{
  const std::string sizes[] = {
    "scalable",
    "512x512", "256x256", "128x128", "64x64",
    "48x48", "32x32", "24x24", "22x22", "16x16"
  };
  const std::string ext[] = {
    "", ".svg", ".svgz", ".png",  /* hicolor */
    "", ".svg", ".png", ".xpm"    /* pixmaps */
  };
  std::string path, path2;

  if ((rgb = img_to_rgb(icon)) != NULL) {
    return;
  }

  for (size_t i = 0; i < sizeof(sizes)/sizeof(*sizes); ++i) {
    path = "/usr/share/icons/hicolor/" + sizes[i] + "/apps/";
    path.append(icon);

    for (int j = 0; j < 4; ++j) {
      path2 = path + ext[j];
      if ((rgb = img_to_rgb(path2.c_str())) != NULL) {
        return;
      }
    }
  }

  path = "/usr/share/pixmaps/";
  path.append(icon);

  for (int i = 4; i < 8; ++i) {
    path2 = path + ext[i];
    if ((rgb = img_to_rgb(path2.c_str())) != NULL) {
      return;
    }
  }

  if (!rgb) {
    rgb = new Fl_PNG_Image(NULL, src_image_missing_png, src_image_missing_png_len);
  }
}

extern "C" void *getline_xlib(void *)
{
  std::string line;

  while (true) {
    if (std::getline(std::cin, line)) {
      if (strcasecmp(line.c_str(), "quit") == 0) {
        close_windows();
        Fl::awake();
        return nullptr;
      } else if (line.length() > 5 && strcasecmp(line.substr(0,5).c_str(), "icon:") == 0) {
        std::string icon = line.substr(5).c_str();

        Fl::lock();

        check_icons(icon.c_str());

        /* make icon a bit smaller than the area */
        int n = but->w();
        if ((n *= ICON_SCALE) > ICON_MIN) {
          but->image(rgb->copy(n, n));
        }

        win->redraw();

        Fl::unlock();
        Fl::awake(win);

        delete rgb;
        rgb = NULL;
      }
      usleep(300000);  /* 300ms */
    }
  }
  return nullptr;
}

static bool create_tray_entry_xlib(const char *icon)
{
  Fl_Color flcol = 0;
  Window dock;
  XEvent ev;
  XColor xcol;
  XImage *image;
  char atom_tray_name[128];

  snprintf(atom_tray_name, sizeof(atom_tray_name) - 1, "_NET_SYSTEM_TRAY_S%i", fl_screen);
  dock = XGetSelectionOwner(fl_display, XInternAtom(fl_display, atom_tray_name, False));
  if (!dock) {
    return false;
  }

  win = new Fl_Single_Window(0, 0);
  {
    but = new simple_button(0, 0, 0, 0);
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
    return false;
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

  if (icon && strlen(icon) > 0) {
    check_icons(icon);
  }

  if (!rgb) {
    rgb = new Fl_PNG_Image(NULL, src_icon_png, src_icon_png_len);
  }

  Fl::add_timeout(0.005, set_size);  /* 5 ms */

  if (listen) {
    Fl::lock();
    pthread_create(&t1, 0, &getline_xlib, nullptr);
  }

  Fl::run();

  return true;
}

int dialog_indicator(const char *command_, const char *icon, int flags, bool listen_, bool auto_close_)
{
  command = command_;
  listen = listen_;
  auto_close = auto_close_;

  if (flags & INDICATOR_GTK) {
    if (start_indicator_gtk(command, icon, listen, auto_close)) {
      return 0;
    }
    if (flags & INDICATOR_X11) {
      std::cerr << "warning: falling back to legacy X11 indicator" << std::endl;
    }
  }

  if ((flags & INDICATOR_X11) && create_tray_entry_xlib(icon)) {
    return 0;
  }

  std::cerr << "error: cannot create tray/indicator entry" << std::endl;
  return 1;
}

