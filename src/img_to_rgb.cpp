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
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

#include "fltk-dialog.hpp"
#include "nanosvg.h"
#include "nanosvgrast.h"

#define HASEXT(str,ext)  (strlastcasecmp(str,ext) == strlen(ext))

#define IMGFILE_MAX  10*1024*1024
#define SVGZ_MAX        1024*1024
#define SVG_MAX       3*1024*1024

static char *gzip_uncompress(const char *file)
{
  std::streampos size;
  std::string output;
  char *data;
  unsigned char out[16*1024];
  z_stream strm;

  if (!file) {
    return NULL;
  }

  std::ifstream ifs(file, std::ios::in|std::ios::binary|std::ios::ate);

  if (!ifs.is_open()) {
    return NULL;
  }

  size = ifs.tellg();

  data = new char[size];
  ifs.seekg(0, std::ios::beg);
  ifs.read(data, size);
  ifs.close();

  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;

  if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
    delete data;
    return NULL;
  }

  strm.avail_in = size;
  strm.next_in = (unsigned char *)data;
  do {
    strm.avail_out = sizeof(out);
    strm.next_out = out;
    switch (inflate(&strm, Z_NO_FLUSH)) {
      case Z_NEED_DICT:
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
        inflateEnd(&strm);
        delete data;
        return NULL;
    }
    output.append((char *)out, sizeof(out) - strm.avail_out);
    if (output.size() > SVG_MAX) {
      inflateEnd(&strm);
      delete data;
      return NULL;
    }
  } while (strm.avail_out == 0);

  delete data;

  if (inflateEnd(&strm) != Z_OK) {
    return NULL;
  }
  return strdup(output.c_str());
}

static Fl_RGB_Image *nsvg_to_rgb(const char *file, bool compressed)
{
  NSVGimage *nsvg;
  NSVGrasterizer *r;
  char *data;
  unsigned char *img;
  Fl_RGB_Image *rgb, *rgb_copy;
  int w, h;
  float scalex, scaley;

  if (!file) {
    return NULL;
  }

  if (compressed) {
    if (!(data = gzip_uncompress(file))) {
      return NULL;
    }
    nsvg = nsvgParse(data, "px", 90.0);
    free(data);
  } else {
    nsvg = nsvgParseFromFile(file, "px", 90.0);
  }

  if (!nsvg->shapes || nsvg->width < 5.0 || nsvg->height < 5.0) {
    nsvgDelete(nsvg);
    return NULL;
  }

  scalex = 64.0 / nsvg->width;
  scaley = 64.0 / nsvg->height;
  w = h = 64;

  img = new unsigned char[w*h*4];
  r = nsvgCreateRasterizer();
  nsvgRasterizeFull(r, nsvg, 0, 0, scalex, scaley, img, w, h, w*4);
  rgb = new Fl_RGB_Image(img, w, h, 4, 0);
  rgb_copy = (Fl_RGB_Image *)rgb->copy();

  delete rgb;
  delete img;
  nsvgDeleteRasterizer(r);
  nsvgDelete(nsvg);

  return rgb_copy;
}

#ifdef WITH_RSVG
static Fl_RGB_Image *svg_to_rgb(const char *file, bool compressed, bool force_nanosvg)
{
  Fl_RGB_Image *rgb = NULL;

  if (!force_nanosvg) {
    rgb = rsvg_to_rgb(file);
  }

  if (!rgb) {
    if (!force_nanosvg) {
      std::cerr << "Warning: falling back to NanoSVG" << std::endl;
    }
    rgb = nsvg_to_rgb(file, compressed);
  }
  return rgb;
}
#else
# define svg_to_rgb(a,b,c) nsvg_to_rgb(a,b)
#endif

Fl_RGB_Image *img_to_rgb(const char *file, bool force_nanosvg)
{
  FILE *fp;
  size_t len;
  unsigned char bytes[16] = {0};
  Fl_RGB_Image *rgb = NULL;
  struct stat st;

#ifndef WITH_RSVG
  (void) force_nanosvg;
#endif

  if (!file || stat(file, &st) == -1 || st.st_size > IMGFILE_MAX) {
    return NULL;
  }

  /* get filetype from extension */
  if (HASEXT(file, ".svg") && st.st_size <= SVG_MAX) {
    rgb = svg_to_rgb(file, false, force_nanosvg);
  } else if ((HASEXT(file, ".svgz") || HASEXT(file, ".svg.gz")) && st.st_size < SVGZ_MAX) {
    rgb = svg_to_rgb(file, true, force_nanosvg);
  } else if (HASEXT(file, ".xpm")) {
    Fl_XPM_Image *xpm = new Fl_XPM_Image(file);
    if (xpm) {
      rgb = new Fl_RGB_Image(xpm, Fl_Color(0));
    }
  } else if (HASEXT(file, ".xbm")) {
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
    len = fread(bytes, 1, sizeof(bytes), fp);
    if (ferror(fp) || len < sizeof(bytes)) {
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

