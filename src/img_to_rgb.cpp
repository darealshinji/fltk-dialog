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

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "ico_image.hpp"
#ifdef USE_DLOPEN
# include "icns_image.hpp"
#endif

#define HASEXT(str,ext)  (strlastcasecmp(str,ext) == strlen(ext))

#define CHECK_IF_LOADED(x) \
  if (!x) { return NULL; } \
  else if (x->fail() < 0) { delete x; return NULL; }

#define IMGFILE_MAX  (10*1024*1024)
#define SVGZ_MAX        (1024*1024)
#define SVG_MAX       (3*1024*1024)

Fl_RGB_Image *img_to_rgb(const char *file)
{
  FILE *fp;
  size_t len;
  unsigned char bytes[8];
  struct stat st;

  if (!file || stat(file, &st) == -1 || st.st_size > IMGFILE_MAX) {
    return NULL;
  }

  /* get filetype from extension */
  if (HASEXT(file, ".svg")) {
    if (st.st_size > SVG_MAX) {
      return NULL;
    }
    Fl_SVG_Image *svg = new Fl_SVG_Image(file, NULL);
    CHECK_IF_LOADED(svg)
    return svg;
  }

  if (HASEXT(file, ".svgz") || HASEXT(file, ".svg.gz")) {
    if (st.st_size > SVGZ_MAX) {
      return NULL;
    }
    Fl_SVG_Image *svg = new Fl_SVG_Image(file, NULL);
    CHECK_IF_LOADED(svg)
    return svg;
  }

  if (HASEXT(file, ".xpm")) {
    Fl_XPM_Image *xpm = new Fl_XPM_Image(file);
    CHECK_IF_LOADED(xpm)
    Fl_RGB_Image *rgb = new Fl_RGB_Image(xpm, Fl_Color(0));
    delete xpm;
    return rgb;
  }

  if (HASEXT(file, ".xbm")) {
    Fl_XBM_Image *in = new Fl_XBM_Image(file);
    CHECK_IF_LOADED(in)
    Fl_Image_Surface *surf = new Fl_Image_Surface(in->w(), in->h());
    if (!surf) {
      delete in;
      return NULL;
    }
    surf->set_current();
    fl_color(FL_WHITE);
    fl_rectf(0, 0, in->w(), in->h());
    fl_color(FL_BLACK);
    in->draw(0, 0);
    Fl_RGB_Image *rgb = surf->image();
    delete in;
    delete surf;
    return rgb;
  }

  /* get filetype from magic bytes */
  if (!(fp = fopen(file, "r"))) {
    return NULL;
  }

  len = fread(bytes, 1, sizeof(bytes), fp);
  if (ferror(fp) || len < sizeof(bytes)) {
    fclose(fp);
    return NULL;
  }

  fclose(fp);

  if (memcmp(bytes, "\211PNG\r\n\032\n", 8) == 0) {
    Fl_PNG_Image *png = new Fl_PNG_Image(file);
    CHECK_IF_LOADED(png)
    return png;
  }

  if (memcmp(bytes, "\377\330\377", 3) == 0 && bytes[3] >= 0xc0 && bytes[3] <= 0xef) {
    Fl_JPEG_Image *jpeg = new Fl_JPEG_Image(file);
    CHECK_IF_LOADED(jpeg)
    return jpeg;
  }

  if (bytes[0] == 'B' && bytes[1] == 'M') {
    Fl_BMP_Image *bmp = new Fl_BMP_Image(file);
    CHECK_IF_LOADED(bmp)
    return bmp;
  }

  if (memcmp(bytes, "GIF87a", 6) == 0 || memcmp(bytes, "GIF89a", 6) == 0) {
    Fl_GIF_Image *gif = new Fl_GIF_Image(file);
    CHECK_IF_LOADED(gif)
    Fl_RGB_Image *rgb = new Fl_RGB_Image(gif, Fl_Color(0));
    delete gif;
    return rgb;
  }

  if (memcmp(bytes, "\0\0\1\0", 4) == 0) {
    ico_image *ico = new ico_image(file);
    CHECK_IF_LOADED(ico)
    return ico;
  }

#ifdef USE_DLOPEN
  if (memcmp(bytes, "icns", 4) == 0 || memcmp(bytes, "mBIN", 4) == 0) {
    icns_image *icns = new icns_image(file);
    CHECK_IF_LOADED(icns)
    return icns;
  }
#endif

  return NULL;
}

