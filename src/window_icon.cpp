/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017, djcj <djcj@gmx.de>
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
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_XBM_Image.H>
#include <FL/Fl_XPM_Image.H>

#include <algorithm>
#include <fstream>
#include <locale>
#include <string>
#include <string.h>

#include "fltk-dialog.hpp"

#define BYTES_BUF 16


struct to_lower {
  int operator() (int ch)
  {
    return std::tolower(ch);
  }
};

static std::string get_ext(const char *input)
{
  std::string s(input);

  if (s.size() <= 4)
  {
    return "";
  }
  s = s.substr(s.size() - 4);
  std::transform(s.begin(), s.end(), s.begin(), to_lower());
  return s;
}

void set_window_icon(const char *file)
{
  char bytes[BYTES_BUF];

  std::ifstream ifs(file, std::ifstream::binary);
  std::streambuf *pbuf = ifs.rdbuf();
  pbuf->pubseekoff(0, ifs.beg);
  pbuf->sgetn(bytes, BYTES_BUF);
  ifs.close();

  if (strncmp(bytes, "\x89PNG\x0D\x0A\x1A\x0A", 8) == 0)
  {
    Fl_PNG_Image in(file);
    Fl_Window::default_icon(&in);
  }
  else if (strncmp(bytes, "\xFF\xD8\xFF\xDB", 4) == 0 ||
          (strncmp(bytes, "\xFF\xD8\xFF\xE0", 4) == 0 && strncmp(bytes + 6, "JFIF\x00\x01", 6) == 0) ||
          (strncmp(bytes, "\xFF\xD8\xFF\xE1", 4) == 0 && strncmp(bytes + 6, "Exif\x00\x00", 6) == 0))
  {
    Fl_JPEG_Image in(file);
    Fl_Window::default_icon(&in);
  }
  else if (strncmp(bytes, "BM", 2) == 0)
  {
    Fl_BMP_Image in(file);
    Fl_Window::default_icon(&in);
  }
  else if (strncmp(bytes, "GIF87a", 6) == 0 ||
           strncmp(bytes, "GIF89a", 6) == 0)
  {
    Fl_GIF_Image in(file);
    Fl_RGB_Image rgb(&in, Fl_Color(0));
    Fl_Window::default_icon(&rgb);
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
}

