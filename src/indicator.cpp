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
./build/fltk_dialog/fltk-dialog --indicator="echo Hello" --listen=$pipe &
echo "ICON:newIcon" > $pipe
*/

#include <iostream>
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "icon_png.h"
#include "icon_ind_png.h"
#include "indicator_gtk.h"

/* From the Tango icon theme, released into the Public Domain.
 * https://commons.wikimedia.org/wiki/File:Image-missing.svg
 */
#include "image_missing_png.h"

#define DEFAULT_SIZE  48
#define ICON_SCALE    0.86  //0.72
#define ICON_MIN      4


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
static const char *named_pipe = NULL;
static bool auto_close;
static pthread_t th;

static void callback(Fl_Widget *, long)
{
  if (!command || strlen(command) == 0) {
    return;
  }

  pid_t pid = fork();

  if (pid == -1) {
    perror("fork()");
  } else if (pid == 0) {
    execl("/bin/sh", "sh", "-c", command, NULL);
    _exit(127);
  }
}

static void close_cb(Fl_Widget *, long exec_command)
{
  if (named_pipe) {
    pthread_cancel(th);
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
    rgb = new Fl_PNG_Image(NULL, image_missing_png, image_missing_png_len);
  }
}

static FILE *open_named_pipe(void)
{
  FILE *fp = NULL;

  if (access(named_pipe, F_OK) == -1) {
    /* path doesn't exist -> make FIFO */
    if (mkfifo(named_pipe, 0644) == -1) {
      /* cannot make FIFO */
      perror("mkfifo()");
      return NULL;
    }
  } else {
    /* path exists -> check if it's a FIFO */
    struct stat st;
    stat(named_pipe, &st);

    if ((st.st_mode & S_IFMT) != S_IFIFO) {
      std::cerr << "error: file is not a FIFO: " << named_pipe << std::endl;
      return NULL;
    }
  }

  if ((fp = fopen(named_pipe, "r+")) == NULL) {
    perror("fopen()");
  }

  return fp;
}

extern "C" void *getline_xlib(void *)
{
  FILE *fp;
  char *line = NULL;
  size_t n = 0;
  ssize_t len;

  if ((fp = open_named_pipe()) == NULL) {
    return nullptr;
  }

  while ((len = getline(&line, &n, fp)) != -1) {
    if (strcasecmp(line, "QUIT\n") == 0) {
      win->hide();

      if (rgb) {
        delete rgb;
      }

      Fl::awake();

      return nullptr;
    } else if (len > 5 && strncasecmp(line, "ICON:", 5) == 0) {
      Fl::lock();

      line[len - 1] = '\0';  /* remove trailing newline */
      check_icons(line + 5);

      if (rgb) {
        /* make icon a bit smaller than the area */
        int n = box->w() * ICON_SCALE;
        box->image(rgb->copy(n, n));
        delete rgb;
      }

      win->redraw();

      Fl::unlock();
      Fl::awake();

      rgb = NULL;
    } else if (strcasecmp(line, "RUN\n") == 0) {
      box->do_callback();
    }
  }

  fclose(fp);

  return nullptr;
}

static int create_corner_window(const char *icon)
{
  int n = DEFAULT_SIZE;

  if (!title) {
    title = "FLTK indicator";
  }

  win = new Fl_Double_Window(n, n, title);
  {
    box = new corner_box(0, 0, win->w(), win->h());
    box->align(FL_ALIGN_CENTER);
    box->callback(auto_close ? close_cb : callback, 1);

    if (icon && strlen(icon) > 0) {
      check_icons(icon);
    }

    if (rgb) {
      n *= ICON_SCALE;
      //std::cout << "n == " << n << std::endl;
      box->image(rgb->copy(n, n));
    } else {
      rgb = new Fl_PNG_Image(NULL, icon_ind_png, icon_ind_png_len);
      box->image(rgb->copy());
    }

    delete rgb;
    rgb = NULL;
  }
  win->callback(close_cb, 0);
  win->tooltip(msg);
  win->when(FL_WHEN_NEVER);
  win->end();

  if (named_pipe) {
    Fl::lock();
    pthread_create(&th, 0, &getline_xlib, NULL);
  }

  set_taskbar(win);
  win->position(Fl::w() - win->w(), Fl::h() - win->h());
  win->show();
  window_decoration = false;
  set_undecorated(win);
  set_always_on_top(win);

  Fl::run();

  return 0;
}

#if defined(HAVE_QT) && defined(USE_DLOPEN)
static int dlopen_start_indicator_qt(const char *icon)
{
  std::string plugin, tmp;
  void *handle = NULL, *p;

  PROTO(int, start_indicator_qt, (int, const char*, const char*, const char*, bool))
  p = dlopen_qtplugin(plugin, handle, "start_indicator_qt");

  if (!handle) {
    return -1;
  }

  if ((!icon || strlen(icon) == 0) && save_to_temp(icon_png, icon_png_len, ".png", tmp)) {
    icon = tmp.c_str();
  }

  start_indicator_qt = reinterpret_cast<start_indicator_qt_t>(p);
  int rv = start_indicator_qt(0, command, icon, named_pipe, auto_close);
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

int dialog_indicator(const char *command_, const char *icon, int native, const char *named_pipe_, bool auto_close_)
{
  command = command_;
  named_pipe = named_pipe_;
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
      return start_indicator_gtk(command, icon, named_pipe, auto_close);
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

