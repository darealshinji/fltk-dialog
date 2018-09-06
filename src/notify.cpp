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
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
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
# include "notify.h"
#else
# include <libnotify/notify.h>
#endif

#include "fltk-dialog.hpp"

static Fl_Double_Window *win;

class close_box : public Fl_Box
{
public:
  close_box(int X, int Y, int W, int H, const char *L=NULL)
    : Fl_Box(X, Y, W, H, L) { }

  inline int handle(int event) {
    if (event == FL_RELEASE) {
      do_callback();
    }
    return Fl_Box::handle(event);
  }
};

static void close_cb(Fl_Widget *, void *) {
  win->hide();
}

static void callback(void *)
{
  const int step = 10; /* pixels */
  const int fadeout_ms = 500;

  int bord = Fl::w() - win->x() - win->w();
  int usec = (fadeout_ms*1000) / ((win->w() + bord)/step);

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

  win->hide();
}

static int notification_box(double time_s, const char *notify_icon, bool force_nanosvg)
{
  int n, h = 160
  ,   title_h = 0
  ,   message_h = 0;

  const int w = 400
  ,         limit = 64
  ,         fs_title = 18
  ,         fs_message = 14;

  Fl_Font font = FL_HELVETICA;
  Fl_Font font_t = FL_HELVETICA_BOLD;
  Fl_Image *rgb = NULL;
  std::string title_wrapped, message_wrapped;

#ifdef WITH_FRIBIDI
  bool title_alloc = false, msg_alloc = false;

  if (use_fribidi) {
    if (title && (title = fribidi_parse_line(title)) != NULL) {
      title_alloc = true;
    }
    if (msg && (msg = fribidi_parse_line(msg)) != NULL) {
      msg_alloc = true;
    }
  }
#endif

  n = w - limit - 30;
  title_wrapped = text_wrap(title, n, font_t, fs_title);
  message_wrapped = text_wrap(msg, n, font, fs_message);

#ifdef WITH_FRIBIDI
  if (title_alloc && title) {
    delete title;
  }
  if (msg_alloc && msg) {
    delete msg;
  }
#endif

  fl_font(font, fs_title);
  fl_measure(title_wrapped.c_str(), n = 0, title_h);
  fl_font(font, fs_message);
  fl_measure(message_wrapped.c_str(), n = 0, message_h);

  n = title_h + message_h + 30;
  if (n > h) {
    h = n;
  }

  win = new Fl_Double_Window(Fl::w() - 10 - w, 10, w, h);
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
      Fl_RGB_Image *img = img_to_rgb(notify_icon, force_nanosvg);
      if (img) {
        int img_w = img->w();
        int img_h = img->h();
        if (img_w > limit || img_h > limit) {
          aspect_ratio_scale(img_w, img_h, limit);
          rgb = img->copy(img_w, img_h);
          delete img;
        } else {
          rgb = img;
        }
        Fl_Box *o = new Fl_Box(10, 10, limit, limit);
        o->image(rgb);
      }
    }

    n = 20 + limit;

    { /* title */
      Fl_Box *o = new Fl_Box(n, 10, 0, 0, title_wrapped.c_str());
      o->align(FL_ALIGN_INSIDE|FL_ALIGN_TOP_LEFT);
      o->labelfont(font_t);
      o->labelcolor(FL_WHITE);
      o->labelsize(fs_title); }

    { /* message */
      Fl_Box *o = new Fl_Box(n, title_h + 20, 0, 0, message_wrapped.c_str());
      o->align(FL_ALIGN_INSIDE|FL_ALIGN_TOP_LEFT);
      o->labelfont(font);
      o->labelcolor(FL_WHITE);
      o->labelsize(fs_message); }
  }
  win->border(0);
  win->show();
  fl_always_on_top(win);

  if (time_s > 0) {
    Fl::add_timeout(time_s, callback);
  }

  int rv = Fl::run();

  if (rgb) {
    delete rgb;
  }
  return rv;
}

static int run_libnotify(const char *appname, int timeout, const char *notify_icon)
{
#ifdef DYNAMIC_NOTIFY
  /* dlopen() libnotify */

  void *handle = dlopen("libnotify.so.4", RTLD_LAZY);
  char *error = dlerror();

  if (!handle) {
    std::cerr << error << std::endl;
    return 1;
  }

  dlerror();

# define LOAD_SYMBOL(type,func,param) \
  GETPROCADDRESS(handle,type,func,param) \
  error = dlerror(); \
  if (error) { \
    std::cerr << "error: " << error << std::endl; \
    dlclose(handle); \
    return 1; \
  }

  LOAD_SYMBOL( gboolean,             notify_init,                      (const char*) )
  LOAD_SYMBOL( gboolean,             notify_is_initted,                (void) )
  LOAD_SYMBOL( NotifyNotification*,  notify_notification_new,          (const char*, const char*, const char*) )
  LOAD_SYMBOL( void,                 notify_notification_set_timeout,  (NotifyNotification*, gint) )
  LOAD_SYMBOL( gboolean,             notify_notification_show,         (NotifyNotification*, GError**) )
  LOAD_SYMBOL( void,                 notify_uninit,                    (void) )

# define DLCLOSE_NOTIFY dlclose(handle)

#endif  /* DYNAMIC_NOTIFY */

#ifndef DLCLOSE_NOTIFY
# define DLCLOSE_NOTIFY
#endif

  char *resolved_path = NULL;
  NotifyNotification *n;
  int rv = 0;

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
    rv = 1;
  }

  notify_uninit();
  DLCLOSE_NOTIFY;

  if (resolved_path) {
    free(resolved_path);
  }
  return rv;
}

int dialog_notify(const char *appname, int timeout, const char *notify_icon, bool libnotify, bool force_nanosvg)
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

  int rv = libnotify ? run_libnotify(appname, timeout, notify_icon) : 1;

  if (rv != 0) {
    rv = notification_box(timeout, notify_icon, force_nanosvg);
  }
  return rv;
}

