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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "misc/gunzip.hpp"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

struct to_lower {
  int operator() (int ch)
  {
    return std::tolower(ch);
  }
};

static std::string get_ext_lower(const char *input, size_t length)
{
  std::string s(input);

  if (s.size() <= length)
  {
    return "";
  }
  s = s.substr(s.size() - length);
  std::transform(s.begin(), s.end(), s.begin(), to_lower());
  return s;
}

static Fl_RGB_Image *svg_to_rgb(const char *file, bool compressed)
{
  NSVGimage *nsvg = NULL;
  NSVGrasterizer *r = NULL;
  char *data = NULL;
  unsigned char *img = NULL;
  Fl_RGB_Image *rgb = NULL;
  int w, h;

  if (compressed)
  {
    data = gunzip(file, 1024*1024);
    if (!data)
    {
      return NULL;
    }
    nsvg = nsvgParse(data, "px", 96.0f);
    free(data);
  }
  else
  {
    nsvg = nsvgParseFromFile(file, "px", 96.0f);
  }

  w = (int)nsvg->width;
  h = (int)nsvg->height;

  if (!nsvg->shapes || w < 1 || h < 1)
  {
    nsvgDelete(nsvg);
    return NULL;
  }

  img = new unsigned char[w*h*4];

  if (!img)
  {
    nsvgDelete(nsvg);
    return NULL;
  }

  r = nsvgCreateRasterizer();
  nsvgRasterize(r, nsvg, 0, 0, 1, img, w, h, w*4);
  rgb = new Fl_RGB_Image(img, w, h, 4, 0);

  delete img;
  nsvgDeleteRasterizer(r);
  nsvgDelete(nsvg);

  return rgb;
}

#define MAGIC_LEN 16

void set_window_icon(const char *file)
{
  char bytes[MAGIC_LEN] = {0};
  Fl_RGB_Image *rgb = NULL;
  struct stat st;

  if (stat(file, &st) == -1 || st.st_size > 10*1024*1024)
  {
    return;
  }

  std::ifstream ifs(file, std::ifstream::binary);
  std::streambuf *pbuf = ifs.rdbuf();
  pbuf->pubseekoff(0, ifs.beg);
  pbuf->sgetn(bytes, MAGIC_LEN);
  ifs.close();

  if (SSTREQ(bytes, "\x89PNG\x0D\x0A\x1A\x0A", 8))
  {
    rgb = new Fl_PNG_Image(file);
  }
  else if (SSTREQ(bytes, "\xFF\xD8\xFF\xDB", 4) ||
          (SSTREQ(bytes, "\xFF\xD8\xFF\xE0", 4) && SSTREQ(bytes + 6, "JFIF\x00\x01", 6)) ||
          (SSTREQ(bytes, "\xFF\xD8\xFF\xE1", 4) && SSTREQ(bytes + 6, "Exif\x00\x00", 6)))
  {
    rgb = new Fl_JPEG_Image(file);
  }
  else if (SSTREQ(bytes, "BM", 2))
  {
    rgb = new Fl_BMP_Image(file);
  }
  else if (SSTREQ(bytes, "GIF87a", 6) ||
           SSTREQ(bytes, "GIF89a", 6))
  {
    rgb = new Fl_RGB_Image(new Fl_GIF_Image(file), Fl_Color(0));
  }
  else if (get_ext_lower(file, 4) == ".svg")
  {
    rgb = svg_to_rgb(file, false);
  }
  else if ((get_ext_lower(file, 5) == ".svgz" ||
            get_ext_lower(file, 7) == ".svg.gz") && \
           st.st_size < 1024*1024)
  {
    rgb = svg_to_rgb(file, true);
  }
  else if (get_ext_lower(file, 4) == ".xpm")
  {
    rgb = new Fl_RGB_Image(new Fl_XPM_Image(file), Fl_Color(0));
  }
  else if (get_ext_lower(file, 4) == ".xbm")
  {
    Fl_XBM_Image in(file);
    Fl_Image_Surface surf(in.w(), in.h());
    surf.set_current();
    fl_color(FL_WHITE);
    fl_rectf(0, 0, in.w(), in.h());
    fl_color(FL_BLACK);
    in.draw(0, 0);
    rgb = surf.image();
  }

  if (rgb)
  {
    Fl_Window::default_icon(rgb);
    delete rgb;
  }
}

