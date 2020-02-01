/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2020, djcj <djcj@gmx.de>
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

/*
pipe=$(mktemp -u)
mkfifo $pipe
exec 3<> $pipe
fltk-dialog --indicator="echo Hello" --listen <&3 &
echo "ICON:newIcon" >&3
*/

#include <iostream>
#include <dlfcn.h>
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
#include "image_missing_svg.h"

#define ICON_SCALE  0.86  //0.72
#define ICON_MIN    4


class corner_box : public Fl_Box
{
public:
  corner_box(int X, int Y, int W, int H, const char *L=NULL)
    : Fl_Box(X, Y, W, H, L)
  { }

  static int event_x_pos, event_y_pos;

  int handle(int event);
};

int corner_box::handle(int ev)
{
  switch (ev) {
    case FL_PUSH:
      if (Fl::event_button() == FL_RIGHT_MOUSE) {
        fl_cursor(FL_CURSOR_MOVE);
        event_x_pos = Fl::event_x();
        event_y_pos = Fl::event_y();
      }
      break;
    case FL_DRAG:
      if (Fl::event_button() == FL_RIGHT_MOUSE) {
        window()->position(Fl::event_x_root() - event_x_pos, Fl::event_y_root() - event_y_pos);
      }
      break;
    case FL_RELEASE:
      fl_cursor(FL_CURSOR_DEFAULT);

      if (Fl::event_inside(this)) {
        int eb = Fl::event_button();
        if (eb == FL_LEFT_MOUSE) {
          do_callback();  /* run command */
        } else if (eb == FL_MIDDLE_MOUSE) {
          window()->do_callback();  /* quit */
        }
      }
      break;
  }
  return ev;
}

int corner_box::event_x_pos = 1;
int corner_box::event_y_pos = 1;

static Fl_Double_Window *win = NULL;
static Fl_RGB_Image *rgb = NULL;
static corner_box *box = NULL;

static const char *command = NULL;
static bool listen, auto_close;
static pthread_t t1;

static void callback(Fl_Widget *, long)
{
  if (command && strlen(command) > 0) {
    int i = system(command);
    static_cast<void>(i);
  }
}

static void close_cb(Fl_Widget *, long exec_command)
{
  if (listen) {
    pthread_cancel(t1);
  }

  win->hide();

  if (rgb) {
    delete rgb;
  }

  if (exec_command != 0 && command && strlen(command) > 0) {
    execl("/bin/sh", "sh", "-c", command, NULL);
    _exit(127);
  }
}

static void check_icons(const char *icon)
{
  const std::string sizes[] = {
    "scalable",
    "512x512", "256x256", "128x128", "64x64",
    "48x48", "32x32", "24x24", "22x22", "16x16"
  };
  const std::string exthic[] = { "", ".svg", ".svgz", ".png" };  /* hicolor */
  const std::string extpix[] = { "", ".svg", ".png", ".xpm" };   /* pixmaps */
  std::string path, path2;

  if ((rgb = img_to_rgb(icon)) != NULL) {
    return;
  }

  for (size_t i = 0; i < sizeof(sizes)/sizeof(*sizes); ++i) {
    path = "/usr/share/icons/hicolor/" + sizes[i] + "/apps/";
    path.append(icon);

    for (int j = 0; j < 4; ++j) {
      path2 = path + exthic[j];
      if ((rgb = img_to_rgb(path2.c_str())) != NULL) {
        return;
      }
    }
  }

  path = "/usr/share/pixmaps/";
  path.append(icon);

  for (int i = 0; i < 4; ++i) {
    path2 = path + extpix[i];
    if ((rgb = img_to_rgb(path2.c_str())) != NULL) {
      return;
    }
  }

  if (!rgb) {
    rgb = new Fl_SVG_Image(NULL, image_missing_svg);
  }
}

extern "C" void *getline_xlib(void *)
{
  std::string line;

  while (true) {
    if (std::getline(std::cin, line)) {
      if (strcasecmp(line.c_str(), "quit") == 0) {
        win->hide();

        if (rgb) {
          delete rgb;
        }

        Fl::awake();

        return nullptr;
      } else if (line.length() > 5 && strcasecmp(line.substr(0,5).c_str(), "icon:") == 0) {
        Fl::lock();

        check_icons(line.substr(5).c_str());

        if (rgb) {
          /* make icon a bit smaller than the area */
          int n = box->w() * ICON_SCALE;
          box->image(rgb->copy(n, n));
          delete rgb;
        }

        win->redraw();

        Fl::unlock();
        Fl::awake(win);

        rgb = NULL;
      } else if (strcasecmp(line.c_str(), "run") == 0) {
        box->do_callback();
      }
      usleep(300000);  /* 300ms */
    }
  }
  return nullptr;
}

static int create_corner_window(const char *icon)
{
  int n = 40;

  if (!title) {
    title = "FLTK indicator";
  }

  win = new Fl_Double_Window(n, n);
  win->callback(close_cb, 0);

  box = new corner_box(0, 0, win->w(), win->h());
  box->callback(auto_close ? close_cb : callback, 1);

  win->label(title);
  win->tooltip(msg);
  win->when(FL_WHEN_NEVER);
  win->end();
  set_taskbar(win);
  win->position(Fl::w() - win->w(), Fl::h() - win->h());
  win->show();
  if (!win->shown()) {
    return 1;
  }

  window_decoration = false;
  set_undecorated(win);
  set_always_on_top(win);

  if (icon && strlen(icon) > 0) {
    check_icons(icon);
  }

  if (!rgb) {
    rgb = new Fl_PNG_Image(NULL, icon_png, icon_png_len);
  }

  n *= ICON_SCALE;
  box->image(rgb->copy(n, n));

  delete rgb;
  rgb = NULL;

  if (listen) {
    Fl::lock();
    pthread_create(&t1, 0, &getline_xlib, nullptr);
  }

  Fl::run();

  return 0;
}

#if defined(HAVE_QT) && defined(USE_DLOPEN)
static int dlopen_start_indicator_qt(const char *icon)
{
  std::string plugin, tmp;
  void *handle = NULL, *p;

  PROTO(int, start_indicator_qt, (int, const char*, const char*, bool, bool))
  p = dlopen_qtplugin(plugin, handle, "start_indicator_qt");

  if (!handle) {
    return -1;
  }

  if ((!icon || strlen(icon) == 0) && save_to_temp(icon_png, icon_png_len, ".png", tmp)) {
    icon = tmp.c_str();
  }

  start_indicator_qt = reinterpret_cast<start_indicator_qt_t>(p);
  int rv = start_indicator_qt(0, command, icon, listen, auto_close);
  dlclose(handle);
#ifndef USE_EXTERNAL_PLUGINS
  unlink(plugin.c_str());
#endif

  if (!tmp.empty()) {
    unlink(tmp.c_str());
  }

  return rv;
}
#endif  /* HAVE_QT && USE_DLOPEN */

int dialog_indicator(const char *command_, const char *icon, int native, bool listen_, bool auto_close_)
{
  command = command_;
  listen = listen_;
  auto_close = auto_close_;

#ifdef USE_DLOPEN
  switch (native) {
    case NATIVE_NONE:
      break;
    case NATIVE_ANY:
#ifdef HAVE_QT
      if (getenv("KDE_FULL_SESSION")) {
        return dlopen_start_indicator_qt(icon);
      }
#endif
    case NATIVE_GTK:
      return start_indicator_gtk(command, icon, listen, auto_close);
#ifdef HAVE_QT
    case NATIVE_QT:
      return dlopen_start_indicator_qt(icon);
#endif
  }
#else
  static_cast<void>(native);
#endif  /* USE_DLOPEN */

  return create_corner_window(icon);
}

