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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if defined(HAVE_QT) && defined(USE_DLOPEN)
# include <dlfcn.h>
# ifndef USE_EXTERNAL_PLUGINS
#  include "qtplugin_so.h"
# endif
#endif

#include "fltk-dialog.hpp"
#include "../random/include/effolkronium/random.hpp"

bool always_on_top = false;
static char date[512] = {0};

const int days_in_month[2][13] = {
  { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
  { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

const int ordinal_day[2][13] = {
  { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
  { 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
};

void run_window(Fl_Double_Window *o, Fl_Widget *w, int min_w, int min_h)
{
  set_size(o, w);
  set_size_range(o, min_w, min_h);
  set_position(o);
  o->end();
  set_taskbar(o);
  o->show();
  set_undecorated(o);
  set_always_on_top(o);
  Fl::run();
}

void set_size(Fl_Double_Window *o, Fl_Widget *w)
{
  if (resizable && w) {
    o->resizable(w);
  }
  o->size(override_w > 0 ? override_w : o->w(), override_h > 0 ? override_h : o->h());
}

void set_size_range(Fl_Double_Window *o, int min_w, int min_h) {
  if (resizable) {
    o->size_range(min_w, min_h, Fl::w(), Fl::h());
  } else {
    o->size_range(o->w(), o->h(), o->w(), o->h());
  }
}

#define LEFT        1
#define UP          1
#define RIGHT       (Fl::w() - o->w())
#define DOWN        (Fl::h() - o->h())
#define HALF_RIGHT  RIGHT/2
#define HALF_DOWN   DOWN/2

void set_position(Fl_Double_Window *o)
{
  switch (override_pos) {
    case 0:  /* not set */
      break;
    case 5:  /* center */
      override_x = HALF_RIGHT;
      override_y = HALF_DOWN;
      break;
    case 1:
      override_x = LEFT;
      override_y = DOWN;
      break;
    case 2:
      override_x = HALF_RIGHT;
      override_y = DOWN;
      break;
    case 3:
      override_x = RIGHT;
      override_y = DOWN;
      break;
    case 4:
      override_x = LEFT;
      override_y = HALF_DOWN;
      break;
    case 6:
      override_x = RIGHT;
      override_y = HALF_DOWN;
      break;
    case 7:
      override_x = LEFT;
      override_y = UP;
      break;
    case 8:
      override_x = HALF_RIGHT;
      override_y = UP;
      break;
    case 9:
      override_x = RIGHT;
      override_y = UP;
      break;
    default:
      break;
  }

  o->position(override_x > 0 ? override_x : o->x(), override_y > 0 ? override_y : o->y());
}

void set_taskbar(Fl_Double_Window *o) {
  if (!window_taskbar) {
    o->border(0);
  }
}

void set_undecorated(Fl_Double_Window *o) {
  o->border(window_decoration ? 1 : 0);
}

void set_always_on_top(Fl_Double_Window *o)
{
  if (always_on_top) {
    XEvent e;
    e.xclient.type = ClientMessage;
    e.xclient.window = fl_xid(o);
    e.xclient.message_type = XInternAtom(fl_display, "_NET_WM_STATE", False);
    e.xclient.format = 32;
    e.xclient.data.l[0] = 1;
    e.xclient.data.l[1] = static_cast<long>(XInternAtom(fl_display, "_NET_WM_STATE_ABOVE", False));
    e.xclient.data.l[2] = 0;
    e.xclient.data.l[3] = 0;
    e.xclient.data.l[4] = 0;
    XSendEvent(fl_display, RootWindow(fl_display, fl_screen), False,
               SubstructureRedirectMask|SubstructureNotifyMask, &e);
    XFlush(fl_display);
  }
}

int measure_button_width(const char *label, int extra_width)
{
  int w = 0;
  if (!label) {
    return 90;
  }
  Fl_Button *o = new Fl_Button(0,0,0,0, label);
  fl_font(o->labelfont(), o->labelsize());
  w = fl_width(o->label()) + extra_width;
  if (w < 90) {
    w = 90;
  }
  delete o;
  return w;
}

void aspect_ratio_scale(int &w, int &h, const int limit)
{
  if (w == h) {
    w = h = limit;
  } else {
    double dw = w, dh = h;
    if (w > h) {
      w = limit;
      h = dh/(dw/limit);
    } else {
      h = limit;
      w = dw/(dh/limit);
    }
  }
}

void split(const std::string &s, char const c, std::vector<std::string> &v)
{
  size_t i = 0;
  size_t j = s.find(c);

  while (j != std::string::npos) {
    v.push_back(s.substr(i, j - i));
    i = ++j;
    j = s.find(c, j);
    if (j == std::string::npos) {
      v.push_back(s.substr(i, s.length()));
    }
  }
}

void repstr(const std::string &from, const std::string &to, std::string &s)
{
  if (!from.empty()) {
    for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size()) {
      s.replace(pos, from.size(), to);
    }
  }
}

std::string translate(const char *text) {
  std::string s = text;
  repstr("\\n", "\n", s);
  repstr("\\t", "\t", s);
  return s;
}

std::string text_wrap(const char *text, int linewidth, Fl_Font font, int font_size)
{
  std::istringstream iss(text);
  std::ostringstream oss;
  std::string word;
  int w, ws, remain = linewidth;

  if (!text) {
    return "";
  }

  fl_font(font, font_size);
  ws = fl_width(' ');

  while (iss >> word) {
    w = fl_width(word.c_str());
    if (oss.tellp() > 0) {
      oss << ' ';
      remain -= ws;
    }
    if (remain >= w) {
      /* enough space to append the word */
      oss << word;
      remain -= w;
    } else {
      /* not enough space to append the word */
      if (w > linewidth) {
        /* word is longer than line -> split the word */
        w = 0;
        for (size_t i = 0; i < word.size(); ++i) {
          int wi = fl_width(word[i]);
          if (w + wi > remain) {
            oss << '\n' << word[i];
            w = wi;
            remain = linewidth - w;
          } else {
            oss << word[i];
            w += wi;
          }
        }
      } else {
        /* new line */
        if (oss.tellp() > 0) {
          oss << '\n';
        }
        oss << word;
      }
      remain = linewidth - w;
    }
  }

  return oss.str();
}

char *format_date(const char *format, int y, int m, int d)
{
  struct tm time;

  if (!format || strlen(format) < 2 || strchr(format, '%') == NULL) {
    format = "%Y-%m-%d";
  }
  snprintf(date, sizeof(date) - 1, "%d-%d-%d", y, m, d);
  memset(&time, 0, sizeof(struct tm));
  strptime(date, "%Y-%m-%d", &time);
  strftime(date, sizeof(date) - 1, format, &time);

  return date;
}

/* Compares the last bytes of s1 and s2 and returns the number
 * of equal bytes, ignoring the case.
 * The terminating null byte ('\0') is ignored.
 * It returns -1 if the length of s1 and/or s2 is 0.
 */
size_t strlastcasecmp(const char *s1, const char *s2)
{
  size_t len1 = strlen(s1);
  size_t len2 = strlen(s2);
  size_t n = 0;

  if (len1 == 0 || len2 == 0) {
    return -1;
  }

  for ( ; len1 > 0 && len2 > 0; ++n, --len1, --len2) {
    if (tolower(s1[len1 - 1]) != tolower(s2[len2 - 1])) {
      break;
    }
  }

  return n;
}

std::string get_random(void)
{
  const char *abc = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  std::string s;

  for (int i=0; i < 26; ++i) {
    /* strlen(abc) - 1 == 61 */
    s.push_back(abc[effolkronium::random_static::get(0, 61)]);
  }

  return s;
}

bool save_to_temp(const unsigned char *data, const unsigned int data_len, const char *postfix, std::string &path)
{
  path = "/tmp/temp-" + get_random();

  if (postfix) {
    path.append(postfix);
  }

  std::ofstream out(path, std::ios::out|std::ios::binary);
  if (!out) {
    std::cerr << "error: cannot create temporary file: " << path << std::endl;
    return false;
  }

  if (data && data_len > 0) {
    out.write(reinterpret_cast<const char *>(data), data_len);
  }
  out.close();

  return true;
}

/* Optimized algorithm by Kevin P. Rice:
 * https://stackoverflow.com/a/11595914/5687704
 *
 * Returns 1 on a leap year, otherwise 0.
 */
int leap_year(int year) {
  return ((year & 3) == 0 && ((year % 25) != 0 || (year & 15) == 0));
}

#if defined(HAVE_QT) && defined(USE_DLOPEN)
void *dlopen_qtplugin(std::string &plugin, void * &handle, const char *func)
{
#ifdef USE_EXTERNAL_PLUGINS

  int len = wai_getExecutablePath(NULL, 0, NULL);
  char *path = new char[len + 1];
  wai_getExecutablePath(path, len, NULL);
  path[len] = '\0';

  char *dir = dirname(path);
  if (!dir) {
    delete path;
    return NULL;
  }

  plugin = std::string(dir) + "/qtplugin.so";

  /* RTLD_NODELETE is needed to prevent memory access violation on dlclose() */
  handle = dlopen(plugin.c_str(), RTLD_LAZY|RTLD_NODELETE);

  if (!handle) {
    plugin = std::string(dir) + "/../lib/fltk-dialog/qtplugin.so";
    handle = dlopen(plugin.c_str(), RTLD_LAZY|RTLD_NODELETE);
  }

  dir = NULL;
  delete path;

#else

  if (!save_to_temp(qtplugin_so, qtplugin_so_len, ".so", plugin)) {
    return NULL;
  }
  handle = dlopen(plugin.c_str(), RTLD_LAZY|RTLD_NODELETE);

#endif  /* USE_EXTERNAL_PLUGINS */

  char *error = dlerror();

  if (!handle) {
    std::cerr << error << std::endl;
#ifndef USE_EXTERNAL_PLUGINS
    unlink(plugin.c_str());
#endif
    return NULL;
  }

  dlerror();

  void *func_ptr = dlsym(handle, func);

  error = dlerror();

  if (error || !func_ptr) {
    std::cerr << "error: " << error << std::endl;
    dlclose(handle);
#ifndef USE_EXTERNAL_PLUGINS
    unlink(plugin.c_str());
#endif
    return NULL;
  }

  return func_ptr;
}
#endif  /* HAVE_QT && USE_DLOPEN */

