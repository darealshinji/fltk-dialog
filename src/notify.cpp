/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018, djcj <djcj@gmx.de>
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
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/x.H>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>

#ifdef DYNAMIC_NOTIFY
# include <dlfcn.h>
# include "notify.hpp"
#else
# include <libnotify/notify.h>
#endif

#include "fltk-dialog.hpp"

static Fl_Double_Window *win;

class close_box : public Fl_Box
{
public:
  close_box(int X, int Y, int W, int H, const char *L=0)
    : Fl_Box(X, Y, W, H, L) { }

  virtual ~close_box() { }

  int handle(int event) {
    int ret = Fl_Box::handle(event);
    if (event == FL_RELEASE) {
      do_callback();
    }
    return ret;
  }
};

static void close_cb(Fl_Widget *, void *) {
  win->hide();
}

static void callback(void *o)
{
  const int step = 10; /* pixels */
  int ms = *(int *)o;

  if (ms > 0) {
    int bord = Fl::w() - win->x() - win->w();
    int usec = (ms*1000) / ((win->w() + bord)/step);

    /* move to right screen boundary until the remaining
     * space is smaller than "step" */
    while ((Fl::w() - win->x() - win->w()) > step) {
      win->position(win->x() + step, win->y());
      Fl::flush();
      usleep(usec);
    }

    /* make window "step - [remaining space]" pixels smaller and
     * move to right screen boundary */
    bord = Fl::w() - win->x() - win->w();
    if (bord > 0) {
      win->size(win->w() - step + bord, win->h());
      win->position(win->x() + bord, win->y());
      Fl::flush();
      usleep(usec);
    }

    /* make window smaller until its width is less than "step" */
    while (win->w() > step) {
      win->size(win->w() - step, win->h());
      win->position(win->x() + step, win->y());
      Fl::flush();
      usleep(usec);
    }
  }

  win->hide();
}

int notification_box(double time_s, int fadeout_ms, const char *notify_icon)
{
  int n, h = 160, title_h = 0, message_h = 0;
  const int w = 400, icon_wh = 64, bord = 10, fs_title = 21, fs_message = 14;
  Fl_Font font = FL_HELVETICA_BOLD;
  Fl_Image *rgb = NULL;
  Fl_PNG_Image *png = NULL;

  n = w - icon_wh - bord*3;
  std::string title_wrapped = word_wrap(title, n, font, fs_title);
  std::string message_wrapped = word_wrap(msg, n, font, fs_message);

  fl_font(font, fs_title);
  fl_measure(title_wrapped.c_str(), n = 0, title_h);
  fl_font(font, fs_message);
  fl_measure(message_wrapped.c_str(), n = 0, message_h);

  n = title_h + message_h + bord*3;
  if (n > h) {
    h = n;
  }

  win = new Fl_Double_Window(Fl::w() - bord - w, bord, w, h);
  win->color(FL_WHITE);  /* outline color */
  {
    { /* background color */
      Fl_Box *o = new Fl_Box(1, 1, w - 2, h - 2);
      o->box(FL_FLAT_BOX);
      o->color(fl_gray_ramp(4)); }

    { /* trigger area to close window */
      close_box *o = new close_box(0, 0, w, h);
      o->callback(close_cb); }

    if (notify_icon) {
      png = new Fl_PNG_Image(notify_icon);
      if (!png->fail()) {
        int png_w = png->w();
        int png_h = png->h();
        if (png_w > icon_wh || png_h > icon_wh) {
          aspect_ratio_scale(png_w, png_h, icon_wh);
          rgb = png->copy(png_w, png_h);
        } else {
          rgb = png->copy();
        }
        Fl_Box *o = new Fl_Box(bord, bord, icon_wh, icon_wh);
        o->image(rgb);
      }
    }

    n = bord*2 + icon_wh;

    { /* title */
      Fl_Box *o = new Fl_Box(n, bord, 0, 0, title_wrapped.c_str());
      o->align(FL_ALIGN_INSIDE|FL_ALIGN_TOP_LEFT);
      o->labelfont(font);
      o->labelcolor(FL_WHITE);
      o->labelsize(fs_title); }

    { /* message */
      Fl_Box *o = new Fl_Box(n, title_h + bord*2, 0, 0, message_wrapped.c_str());
      o->align(FL_ALIGN_INSIDE|FL_ALIGN_TOP_LEFT);
      o->labelfont(font);
      o->labelcolor(FL_WHITE);
      o->labelsize(fs_message); }
  }
  //win->set_override();
  win->border(0);
  win->show();
  always_on_top = true;
  set_always_on_top(win);

  if (time_s > 0) {
    Fl::add_timeout(time_s, callback, &fadeout_ms);
  }
  ret = Fl::run();

  if (rgb) {
    delete rgb;
  }
  if (png) {
    delete png;
  }
  return ret;
}

int run_libnotify(const char *appname, int timeout, const char *notify_icon)
{
#ifdef DYNAMIC_NOTIFY
  /* dlopen() libnotify */

  gboolean            (*notify_init)                     (const char*);
  gboolean            (*notify_is_initted)               (void);
  NotifyNotification* (*notify_notification_new)         (const char*, const char*, const char*);
  void                (*notify_notification_set_timeout) (NotifyNotification*, gint);
  gboolean            (*notify_notification_show)        (NotifyNotification*, GError**);
  void                (*notify_uninit)                   (void);

  void *handle = dlopen("libnotify.so.4", RTLD_LAZY);
  const char *dlsym_error = dlerror();

  if (!handle) {
    std::cerr << dlsym_error << std::endl;
    return 1;
  }

  dlerror();  /* clear/reset errors */

# define LOAD_SYMBOL(x) \
  *(void **) (&x) = dlsym(handle, STRINGIFY(x)); \
  dlsym_error = dlerror(); \
  if (dlsym_error) \
  { \
    std::cerr << "error: cannot load symbol\n" << dlsym_error << std::endl; \
    dlclose(handle); \
    return 1; \
  }

  LOAD_SYMBOL(notify_init);
  LOAD_SYMBOL(notify_is_initted);
  LOAD_SYMBOL(notify_notification_new);
  LOAD_SYMBOL(notify_notification_set_timeout);
  LOAD_SYMBOL(notify_notification_show);
  LOAD_SYMBOL(notify_uninit);

# define DLCLOSE_NOTIFY dlclose(handle)

#endif  /* DYNAMIC_NOTIFY */

#ifndef DLCLOSE_NOTIFY
# define DLCLOSE_NOTIFY
#endif

  char *resolved_path = NULL;
  NotifyNotification *n;

  notify_init(appname);

  if (!notify_is_initted()) {
    std::cerr << "error: notify_init()" << std::endl;
    DLCLOSE_NOTIFY;
    return 1;
  }

  if (notify_icon) {
    resolved_path = realpath(notify_icon, NULL);
    if (resolved_path) {
      notify_icon = resolved_path;
    }
  }

  n = notify_notification_new(title, msg, notify_icon);
  notify_notification_set_timeout(n, timeout * 1000);

  if (!notify_notification_show(n, NULL)) {
    std::cerr << "error: notify_notification_show()" << std::endl;
    ret = 1;
  }

  notify_uninit();
  DLCLOSE_NOTIFY;

  if (resolved_path) {
    free(resolved_path);
  }
  return ret;
}

int dialog_notify(const char *appname, int timeout, const char *notify_icon, bool libnotify)
{
  if (timeout < 1) {
    std::cerr << "error: timeout shorter than 1 second: " << timeout << std::endl;
    return 1;
  }

  if (timeout > 60) {
    std::cerr << "error: timeout longer than 60 seconds: " << timeout << std::endl;
    return 1;
  }

  if (!msg) {
    msg = "No message";
  }

  if (!title) {
    title = "No title";
  }

  ret = libnotify ? run_libnotify(appname, timeout, notify_icon) : 1;

  if (ret != 0) {
    ret = notification_box(timeout, 500, notify_icon);
  }
  return ret;
}

