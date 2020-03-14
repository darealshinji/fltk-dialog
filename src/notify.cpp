/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020, djcj <djcj@gmx.de>
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

#include <iostream>
#include <string>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#ifdef USE_DLOPEN
# include "notify.h"
#endif

static Fl_Double_Window *win;
static int win_x, win_y;

class close_box : public Fl_Box
{
public:
  close_box(int X, int Y, int W, int H, const char *L=NULL)
    : Fl_Box(X, Y, W, H, L) { }

  int handle(int event) {
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
  /* Make sure the XY positions remain the same inside the callback function,
   * see https://www.fltk.org/str.php?L3515+P0+S-2+C0+I0+E0+V1.3%25+Q
   */
  win->position(win_x, win_y);

  const int step = 10; /* pixels */
  const int fadeout_ms = 600;
  const int max_w = Fl::w();
  int bord = max_w - win->x() - win->w();
  const int usec = (fadeout_ms*1000) / ((win->w() + bord)/step);

  /* move to right screen boundary until the remaining
   * space is smaller than "step" */
  while ((max_w - win->x() - win->w()) > step) {
    win->position(win->x() + step, win->y());
    Fl::flush();
    usleep(usec);
  }

  /* make window "step - [remaining space]" pixels smaller and
   * move to right screen boundary */
  bord = max_w - win->x() - win->w();
  if (bord > 0) {
    win->size(win->w() - step + bord, win->h());
    win->position(max_w - win->w() + 2, win->y());
    Fl::flush();
    usleep(usec);
  }

  /* make window smaller until its width is less than "step" */
  while (win->w() > step*2) {
    win->size(win->w() - step, win->h());
    win->position(max_w - win->w() + 2, win->y());
    Fl::flush();
    usleep(usec);
  }

  win->hide();
}

static void notification_box(double time_s, Fl_RGB_Image *rgb)
{
  int n, h = 160
  ,   title_h = 0
  ,   message_h = 0;

  const int w = 400
  ,         limit = 64
  ,         fs_title = 18
  ,         fs_message = 14;

  const Fl_Font font = FL_HELVETICA;
  const Fl_Font font_t = FL_HELVETICA_BOLD;
  Fl_Image *img = NULL;
  std::string title_wrapped, message_wrapped;

  n = w - limit - 30;
  title_wrapped = text_wrap(title, n, font_t, fs_title);
  message_wrapped = text_wrap(msg, n, font, fs_message);

  fl_font(font, fs_title);
  fl_measure(title_wrapped.c_str(), n = 0, title_h);
  fl_font(font, fs_message);
  fl_measure(message_wrapped.c_str(), n = 0, message_h);

  n = title_h + message_h + 30;
  if (n > h) {
    h = n;
  }

  win_x = Fl::w() - 10 - w;
  win_y = 10;

  /* only positions 9, 6 and 3 are truely supported */
  switch (override_pos) {
    case 0:
      break;
    case 1:
    case 2:
    case 3:
      win_y = Fl::h() - h - 10;
      break;
    case 4:
    case 5:
    case 6:
      win_y = (Fl::h() - h)/2;
      break;
  }

  win = new Fl_Double_Window(win_x, win_y, w, h);
  win->color(FL_WHITE);  /* outline color */
  {
    { /* background color */
      Fl_Box *o = new Fl_Box(2, 2, w - 3, h - 3);
      o->box(FL_FLAT_BOX);
      o->color(fl_gray_ramp(4)); }

    { /* trigger area to close window */
      close_box *o = new close_box(0, 0, w, h);
      o->callback(close_cb); }

    if (rgb) {
      int img_w = rgb->w();
      int img_h = rgb->h();
      if (img_w > limit || img_h > limit) {
        aspect_ratio_scale(img_w, img_h, limit);
        img = rgb->copy(img_w, img_h);
        delete rgb;
      } else {
        img = rgb;
      }
      Fl_Box *o = new Fl_Box(10, 10, limit, limit);
      o->image(img);
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
  set_always_on_top(win);

  if (time_s > 0) {
    Fl::add_timeout(time_s, callback);
  }

  Fl::run();

  if (img) {
    delete img;
  }
}

#ifdef USE_DLOPEN
static bool run_libnotify(const char *appname, int timeout, const char *notify_icon)
{
  /* dlopen() libnotify */

  void *handle = dlopen("libnotify.so.4", RTLD_LAZY);
  char *error = dlerror();

  if (!handle) {
    std::cerr << error << std::endl;
    return false;
  }

  dlerror();

# define LOAD_SYMBOL(type,func,param) \
  GETPROCADDRESS(handle,type,func,param) \
  error = dlerror(); \
  if (error) { \
    std::cerr << "error: " << error << std::endl; \
    dlclose(handle); \
    return false; \
  }

  LOAD_SYMBOL( gboolean,             notify_init,                      (const char*) )
  LOAD_SYMBOL( gboolean,             notify_is_initted,                (void) )
  LOAD_SYMBOL( NotifyNotification*,  notify_notification_new,          (const char*, const char*, const char*) )
  LOAD_SYMBOL( void,                 notify_notification_set_timeout,  (NotifyNotification*, gint) )
  LOAD_SYMBOL( gboolean,             notify_notification_show,         (NotifyNotification*, GError**) )
  LOAD_SYMBOL( void,                 notify_uninit,                    (void) )

  char *resolved_path = NULL;
  NotifyNotification *n;
  bool rv = true;

  notify_init(appname);

  if (!notify_is_initted()) {
    std::cerr << "error: notify_init()" << std::endl;
    dlclose(handle);
    return false;
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
    rv = false;
  }

  notify_uninit();
  dlclose(handle);

  if (resolved_path) {
    free(resolved_path);
  }
  return rv;
}
#endif  /* USE_DLOPEN */

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

#ifdef USE_DLOPEN
  if (libnotify && run_libnotify(appname, timeout, notify_icon)) {
    return 0;
  }
#else
  (void)libnotify;
#endif

  notification_box(timeout, img_to_rgb(notify_icon));

  return 0;
}

