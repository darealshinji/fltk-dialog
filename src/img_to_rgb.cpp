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
#include <iostream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fltk-dialog.hpp"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

#define MAGIC_LEN              16
#define IMGFILE_MAX  10*1024*1024  /* 10 MB */
#define SVGZ_MAX        1024*1024  /*  1 MB */
#define SVG_MAX       3*1024*1024  /*  3 MB */

struct to_lower {
  int operator() (int c) {
    return tolower(c);
  }
};

static std::string get_ext_lower(const char *input, size_t length)
{
  if (!input) {
    return "";
  }
  std::string s(input);
  if (s.size() <= length) {
    return "";
  }
  s = s.substr(s.size() - length);
  std::transform(s.begin(), s.end(), s.begin(), to_lower());
  return s;
}

static Fl_RGB_Image *nsvg_to_rgb(const char *file, bool compressed)
{
  NSVGimage *nsvg;
  NSVGrasterizer *r;
  char *data;
  unsigned char *img;
  Fl_RGB_Image *rgb, *rgb_copy;
  int w, h;

  if (compressed) {
    data = gunzip(file, SVG_MAX);
    if (!data) {
      return NULL;
    }
    nsvg = nsvgParse(data, "px", 96.0f);
    delete data;
  } else {
    nsvg = nsvgParseFromFile(file, "px", 96.0f);
  }

  w = (int)nsvg->width;
  h = (int)nsvg->height;

  if (!nsvg->shapes || w < 1 || h < 1) {
    nsvgDelete(nsvg);
    return NULL;
  }

  img = new unsigned char[w*h*4];
  r = nsvgCreateRasterizer();
  nsvgRasterize(r, nsvg, 0, 0, 1, img, w, h, w*4);
  rgb = new Fl_RGB_Image(img, w, h, 4, 0);
  rgb_copy = (Fl_RGB_Image *)rgb->copy();

  delete rgb;
  delete img;
  nsvgDeleteRasterizer(r);
  nsvgDelete(nsvg);

  return rgb_copy;
}

#ifdef WITH_RSVG
static Fl_RGB_Image *svg_to_rgb(const char *file, bool compressed)
{
  Fl_RGB_Image *rgb = rsvg_to_rgb(file);

  if (!rgb) {
    //std::cout << "fall back to NanoSVG" << std::endl;
    rgb = nsvg_to_rgb(file, compressed);
  }
  return rgb;
}
#else
# define svg_to_rgb(a,b) nsvg_to_rgb(a,b)
#endif

Fl_RGB_Image *img_to_rgb(const char *file)
{
  FILE *fp;
  size_t len;
  unsigned char bytes[MAGIC_LEN] = {0};
  Fl_RGB_Image *rgb = NULL;
  struct stat st;

  if (!file || stat(file, &st) == -1 || st.st_size > IMGFILE_MAX) {
    return NULL;
  }

  /* get filetype from extension */

  if (get_ext_lower(file, 4) == ".svg" && st.st_size <= SVG_MAX) {
    rgb = svg_to_rgb(file, false);
  }
  else if ((get_ext_lower(file, 5) == ".svgz" || get_ext_lower(file, 7) == ".svg.gz") &&
           st.st_size < SVGZ_MAX) {
    rgb = svg_to_rgb(file, true);
  }
  else if (get_ext_lower(file, 4) == ".xpm") {
    Fl_XPM_Image *xpm = new Fl_XPM_Image(file);
    if (xpm) {
      rgb = new Fl_RGB_Image(xpm, Fl_Color(0));
    }
  }
  else if (get_ext_lower(file, 4) == ".xbm") {
    Fl_XBM_Image in(file);
    Fl_Image_Surface surf(in.w(), in.h());
    surf.set_current();
    fl_color(FL_WHITE);
    fl_rectf(0, 0, in.w(), in.h());
    fl_color(FL_BLACK);
    in.draw(0, 0);
    rgb = surf.image();
  }

  /* get filetype from magic bytes */

  if (!rgb) {
    if (!(fp = fopen(file, "r"))) {
      return NULL;
    }
    len = fread(bytes, 1, MAGIC_LEN, fp);
    if (ferror(fp) || len < MAGIC_LEN) {
      fclose(fp);
      return NULL;
    }

    if (memcmp(bytes, "\x89PNG\x0D\x0A\x1A\x0A", 8) == 0) {
      rgb = new Fl_PNG_Image(file);
    }
    else if (memcmp(bytes, "\xFF\xD8\xFF\xDB", 4) == 0 ||
            (memcmp(bytes, "\xFF\xD8\xFF\xE0", 4) == 0 && memcmp(bytes + 6, "JFIF\x00\x01", 6) == 0) ||
            (memcmp(bytes, "\xFF\xD8\xFF\xE1", 4) == 0 && memcmp(bytes + 6, "Exif\x00\x00", 6) == 0)) {
      rgb = new Fl_JPEG_Image(file);
    }
    else if (memcmp(bytes, "BM", 2) == 0) {
      rgb = new Fl_BMP_Image(file);
    }
    else if (memcmp(bytes, "GIF87a", 6) == 0 || memcmp(bytes, "GIF89a", 6) == 0) {
      Fl_GIF_Image *gif = new Fl_GIF_Image(file);
      if (gif) {
        rgb = new Fl_RGB_Image(gif, Fl_Color(0));
      }
    }
  }

  return rgb;
}

