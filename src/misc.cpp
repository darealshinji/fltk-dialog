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
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Button.H>
#include <FL/x.H>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "fltk-dialog.hpp"

bool always_on_top = false;

void run_window(Fl_Double_Window *o, Fl_Widget *w);
void set_size(Fl_Double_Window *o, Fl_Widget *w);
void set_position(Fl_Double_Window *o);
void set_taskbar(Fl_Double_Window *o);  /* place before show() */
void set_undecorated(Fl_Double_Window *o);  /* place after show() */
void set_always_on_top(Fl_Double_Window *o);  /* place after show() */
int measure_button_width(const char *label, int extra_width);
void aspect_ratio_scale(int &w, int &h, const int limit);
void split(const std::string &s, char c, std::vector<std::string> &v);
void repstr(const std::string &from, const std::string &to, std::string &s);
std::string translate(const char *inputText);
std::string text_wrap(const char *text, int width, Fl_Font font, int font_size);
void print_date(std::string format, int y, int m, int d);
size_t strlastcasecmp(const char *s1, const char *s2);

void run_window(Fl_Double_Window *o, Fl_Widget *w)
{
  if (w) {
    set_size(o, w);
  }
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
  if (resizable) {
    o->resizable(w);
  }
  if (override_w > 0) {
    o->size(override_w, o->h());
  }
  if (override_h > 0) {
    o->size(o->w(), override_h);
  }
}

void set_position(Fl_Double_Window *o)
{
  if (position_center) {
    override_x = (max_w - o->w()) / 2;
    override_y = (max_h - o->h()) / 2;
  }
  if (override_x >= 0) {
    o->position(override_x, o->y());
  }
  if (override_y >= 0) {
    o->position(o->x(), override_y);
  }
}

void set_taskbar(Fl_Double_Window *o) {
  if (!window_taskbar) {
    o->border(0);
  }
}

void set_undecorated(Fl_Double_Window *o) {
  if (window_decoration) {
    o->border(1);
  } else {
    o->border(0);
  }
}

void set_always_on_top(Fl_Double_Window *o)
{
  if (always_on_top) {
    o->wait_for_expose();

    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(fl_display, "_NET_WM_STATE", False);
    event.xclient.window = fl_xid((Fl_Window *)o);
    event.xclient.format = 32;
    event.xclient.data.l[0] = 1;  /* _NET_WM_STATE_ADD */
    event.xclient.data.l[1] = XInternAtom(fl_display, "_NET_WM_STATE_ABOVE", False);
    event.xclient.data.l[2] = 0;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;

    XSendEvent(fl_display, DefaultRootWindow(fl_display), False,
               SubstructureRedirectMask|SubstructureNotifyMask, &event);
  }
}

int measure_button_width(const char *label, int extra_width)
{
  int h = 0, w = 0;
  if (!label) {
    return 90;
  }
  Fl_Button *o = new Fl_Button(0,0,0,0, label);
  fl_font(o->labelfont(), o->labelsize());
  fl_measure(o->label(), w, h);
  w += extra_width;
  if (w < 90) {
    w = 90;
  }
  delete o;
  return w;
}

void aspect_ratio_scale(int &w, int &h, const int limit)
{
  float fw = (float)w;
  float fh = (float)h;

  if (fw/fh == 1.0) {
    w = limit;
    h = limit;
  } else {
    if (w > h) {
      w = limit;
      h = (int)(fh/(fw/(float)limit));
    } else {
      h = limit;
      w = (int)(fw/(fh/(float)limit));
    }
  }
}

void split(const std::string &s, char c, std::vector<std::string> &v)
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

void repstr(const std::string &from, const std::string &to, std::string &s) {
  if (!from.empty()) {
    for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size()) {
      s.replace(pos, from.size(), to);
    }
  }
}

std::string translate(const char *text) {
  std::string s(text);
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
    if (remain >= w) {
      /* enough space to append the word */
      if (oss.tellp() > 0) {
        oss << ' ';
        remain -= ws;
      }
      remain -= w;
      oss << word;
    } else {
      /* new line */
      if (oss.tellp() > 0) {
        oss << '\n';
      }
      if (w > linewidth) {
        /* split the word */
        w = 0;
        for (size_t i = 0; i < word.size(); i++) {
          int wi = fl_width(word[i]);
          if (w + wi > linewidth) {
            oss << '\n' << word[i];
            w = wi;
          } else {
            oss << word[i];
            w += wi;
          }
        }
      } else {
        oss << word;
      }
      remain = linewidth - w;
    }
  }

  return oss.str();
}

void print_date(std::string format, int y, int m, int d)
{
  std::stringstream ss;

  char date[256] = {0};
  struct tm time;

  if (format == "") {
    format = "%Y-%m-%d";
  } else {
    /* glibc date formats
     * example date: 2006-01-08 */
    repstr("\\n", "\n",  format);  /* newline */
    repstr("\\t", "\t",  format);  /* tab */
    repstr("%",   "%%",  format);  /* literal % */
    repstr("d",   "%-d", format);  /* day (8) */
    repstr("D",   "%d",  format);  /* day (08) */
    repstr("m",   "%-m", format);  /* month (1) */
    repstr("M",   "%m",  format);  /* month (01) */
    repstr("y",   "%y",  format);  /* year (06) */
    repstr("Y",   "%Y",  format);  /* year (2006) */
    repstr("j",   "%-j", format);  /* day of the year (8) */
    repstr("J",   "%j",  format);  /* day of the year (008) */
    repstr("W",   "%A",  format);  /* weekday name (Sunday) */
    repstr("w",   "%a",  format);  /* weekday name (Sun) */
    repstr("n",   "%-V", format);  /* ISO 8601 week number (1) */
    repstr("N",   "%V",  format);  /* ISO 8601 week number (01) */
    repstr("B",   "%B",  format);  /* month name (January) */
    repstr("b",   "%b",  format);  /* month name (Jan) */
    repstr("u",   "%u",  format);  /* day of the week, Monday being 1 (7) */
  }

  ss << y << "-" << m << "-" << d;
  memset(&time, 0, sizeof(struct tm));
  strptime(ss.str().c_str(), "%Y-%m-%d", &time);
  strftime(date, sizeof(date), format.c_str(), &time);

  std::cout << date << std::endl;
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

  while (len1 > 0 && len2 > 0) {
    if (tolower(s1[len1 - 1]) != tolower(s2[len2 - 1])) {
      break;
    }
    ++n;
    --len1;
    --len2;
  }

  return n;
}

