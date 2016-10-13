/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, djcj <djcj@gmx.de>
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
#include <FL/fl_draw.H>
#include <FL/Fl_Image_Surface.H>

#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_PNM_Image.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_XBM_Image.H>
#include <FL/Fl_XPM_Image.H>

#include <algorithm>
#include <locale>
#include <string>

#include "fltk-dialog.hpp"


struct to_lower {
  int operator() (int ch)
  {
    return std::tolower(ch);
  }
};

static std::string get_ext(const char *input, unsigned int n=4)
{
  std::string s = std::string(input);

  if (s.size() <= n)
  {
    return "";
  }
  s = s.substr(s.size() - n);
  std::transform(s.begin(), s.end(), s.begin(), to_lower());
  return s;
}

void set_window_icon(const char *file)
{
  if (get_ext(file) == ".png")
  {
    Fl_PNG_Image in(file);
    Fl_Window::default_icon(&in);
  }
  else if (get_ext(file) == ".xpm")
  {
    Fl_XPM_Image in(file);
    Fl_RGB_Image rgb(&in, Fl_Color(0));
    Fl_Window::default_icon(&rgb);
  }
  else if (get_ext(file) == ".xbm")
  {
    Fl_XBM_Image in(file);
    Fl_Image_Surface surf(in.w(), in.h());
    surf.set_current();
    fl_color(FL_WHITE);
    fl_rectf(0, 0, in.w(), in.h());
    fl_color(FL_BLACK);
    in.draw(0, 0);
    Fl_RGB_Image *rgb = surf.image();
    Fl_Window::default_icon(rgb);
  }
  else if (get_ext(file) == ".jpg" || get_ext(file, 5) == ".jpeg")
  {
    Fl_JPEG_Image in(file);
    Fl_Window::default_icon(&in);
  }
  else if (get_ext(file) == ".bmp")
  {
    Fl_BMP_Image in(file);
    Fl_Window::default_icon(&in);
  }
  else if (get_ext(file) == ".gif")
  {
    Fl_GIF_Image in(file);
    Fl_RGB_Image rgb(&in, Fl_Color(0));
    Fl_Window::default_icon(&rgb);
  }
  else if (get_ext(file) == ".pnm")
  {
    Fl_PNM_Image in(file);
    Fl_Window::default_icon(&in);
  }
}

