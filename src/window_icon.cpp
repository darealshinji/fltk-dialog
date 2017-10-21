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
#include <string.h>
#include <unistd.h>

#include "fltk-dialog.hpp"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

#define SVG_BUF_MAX  1048576 /* maximum accepted SVG data size, 1 MiB */
#define SVG_UNITS    "px"    /* units passed to NanoSVG */
#define SVG_DPI      96.0f   /* DPI (dots-per-inch) used for unit conversion */
#define SVG_DEPTH    4       /* image depth */
#define BYTES_BUF    16      /* magic bytes length */

static char *icon_svg_data;

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

/* piping seems to be easier and safer than directly using zlib */
static char *gunzip_svg(const char *filename)
{
  FILE *fd;
  size_t size;
  std::string command;
  icon_svg_data = new char[SVG_BUF_MAX]();

  command = "gzip -cd \"" + std::string(filename) + "\"";
  fd = popen(command.c_str(), "re");
  size = fread(icon_svg_data, 1, SVG_BUF_MAX - 1, fd);
  pclose(fd);

  if (size == 0 || feof(fd) == 0)
  {
    delete icon_svg_data;
    return NULL;
  }
  return icon_svg_data;
}

static void default_icon_svg(const char *filename, bool gunzip)
{
  NSVGimage *nsvg = NULL;
  NSVGrasterizer *r = NULL;
  char *data = NULL;
  unsigned char *img = NULL;
  Fl_RGB_Image *rgb = NULL;
  int w, h;

  if (access(filename, R_OK) != 0)
  {
    return;
  }

  if (gunzip)
  {
    data = gunzip_svg(filename);

    if (!data)
    {
      return;
    }
    nsvg = nsvgParse(data, SVG_UNITS, SVG_DPI);
    delete data;
  }
  else
  {
    nsvg = nsvgParseFromFile(filename, SVG_UNITS, SVG_DPI);
  }

  w = (int)nsvg->width;
  h = (int)nsvg->height;

  if (!nsvg->shapes || w < 1 || h < 1)
  {
    nsvgDelete(nsvg);
    return;
  }

  img = new unsigned char[w*h*SVG_DEPTH];

  if (!img)
  {
    nsvgDelete(nsvg);
    return;
  }

  r = nsvgCreateRasterizer();
  nsvgRasterize(r, nsvg, 0, 0, 1, img, w, h, w*SVG_DEPTH);
  rgb = new Fl_RGB_Image(img, w, h, SVG_DEPTH, 0);

  if (rgb)
  {
    Fl_Window::default_icon(rgb);
    delete rgb;
  }

  delete img;
  nsvgDeleteRasterizer(r);
  nsvgDelete(nsvg);
}

void set_window_icon(const char *file)
{
  char bytes[BYTES_BUF] = {0};

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
  else if (get_ext_lower(file, 4) == ".svg")
  {
    default_icon_svg(file, false);
  }
  else if (get_ext_lower(file, 5) == ".svgz" ||
           get_ext_lower(file, 7) == ".svg.gz")
  {
    default_icon_svg(file, true);
  }
  else if (get_ext_lower(file, 4) == ".xpm")
  {
    Fl_XPM_Image in(file);
    Fl_RGB_Image rgb(&in, Fl_Color(0));
    Fl_Window::default_icon(&rgb);
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
    Fl_RGB_Image *rgb = surf.image();
    Fl_Window::default_icon(rgb);
  }
}

