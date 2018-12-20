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

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <dlfcn.h>
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

#define IMGFILE_MAX  (10*1024*1024)
#define SVGZ_MAX        (1024*1024)
#define SVG_MAX       (3*1024*1024)

#define DEFAULT_DPI  90.0
#define DEFAULT_WH   64
#define DEFAULT_WHF  64.0

static std::string png_data;

static bool gzip_uncompress(const char *file, std::string &output)
{
  std::streampos size;
  char *data;
  unsigned char out[16*1024];
  z_stream strm;

  if (!file) {
    return false;
  }

  std::ifstream ifs(file, std::ios::in|std::ios::binary|std::ios::ate);

  if (!ifs.is_open()) {
    return false;
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
    return false;
  }

  strm.avail_in = size;
  strm.next_in = reinterpret_cast<unsigned char *>(data);
  do {
    strm.avail_out = sizeof(out);
    strm.next_out = out;
    switch (inflate(&strm, Z_NO_FLUSH)) {
      case Z_NEED_DICT:
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
        inflateEnd(&strm);
        delete data;
        return false;
    }
    output.append(reinterpret_cast<const char *>(out), sizeof(out) - strm.avail_out);
    if (output.size() >= SVG_MAX) {
      inflateEnd(&strm);
      delete data;
      return false;
    }
  } while (strm.avail_out == 0);

  delete data;

  if (inflateEnd(&strm) != Z_OK) {
    return false;
  }
  return true;
}

static Fl_RGB_Image *svg_to_rgb(const char *file, bool compressed)
{
  Fl_RGB_Image *rgb, *rgb_copy;
  NSVGimage *nsvg;
  NSVGrasterizer *r;
  std::string data;
  char *data_copy;
  unsigned char *img;
  int w, h;
  float scalex, scaley;

  if (!file) {
    return NULL;
  }

  if (compressed) {
    if (!gzip_uncompress(file, data)) {
      return NULL;
    }
    data_copy = strdup(data.c_str());
    data.clear();
    nsvg = nsvgParse(data_copy, "px", DEFAULT_DPI);
    free(data_copy);
  } else {
    nsvg = nsvgParseFromFile(file, "px", DEFAULT_DPI);
  }

  if (!nsvg->shapes || nsvg->width < 5.0 || nsvg->height < 5.0) {
    nsvgDelete(nsvg);
    return NULL;
  }

  scalex = DEFAULT_WHF / nsvg->width;
  scaley = DEFAULT_WHF / nsvg->height;
  w = h = DEFAULT_WH;

  img = new unsigned char[w*h*4];
  r = nsvgCreateRasterizer();
  nsvgRasterizeXY(r, nsvg, 0, 0, scalex, scaley, img, w, h, w*4);
  rgb = new Fl_RGB_Image(img, w, h, 4, 0);
  rgb_copy = dynamic_cast<Fl_RGB_Image *>(rgb->copy());

  nsvgDeleteRasterizer(r);
  nsvgDelete(nsvg);
  delete img;
  delete rgb;

  /* need to return a copy */
  return rgb_copy;
}

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
    if (st.st_size < SVG_MAX) {
      return svg_to_rgb(file, false);
    }
    return NULL;
  }

  if (HASEXT(file, ".svgz") || HASEXT(file, ".svg.gz")) {
    if (st.st_size < SVGZ_MAX) {
      return svg_to_rgb(file, true);
    }
    return NULL;
  }

  if (HASEXT(file, ".xpm")) {
    Fl_XPM_Image *xpm = new Fl_XPM_Image(file);
    if (!xpm) {
      return NULL;
    }
    return new Fl_RGB_Image(xpm, Fl_Color(0));
  }

  if (HASEXT(file, ".xbm")) {
    Fl_XBM_Image *in = new Fl_XBM_Image(file);
    if (!in) {
      return NULL;
    }
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
    return surf->image();
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

  if (memcmp(bytes, "\211PNG", 4) == 0) {
    return new Fl_PNG_Image(file);
  }

  if (memcmp(bytes, "\377\330\377", 3) == 0 && bytes[3] >= 0xc0 && bytes[3] <= 0xef) {
    return new Fl_JPEG_Image(file);
  }

  if (memcmp(bytes, "BM", 2) == 0) {
    return new Fl_BMP_Image(file);
  }

  if (memcmp(bytes, "GIF87a", 6) == 0 || memcmp(bytes, "GIF89a", 6) == 0) {
    Fl_GIF_Image *gif = new Fl_GIF_Image(file);
    if (gif) {
      return new Fl_RGB_Image(gif, Fl_Color(0));
    }
  }

  return NULL;
}

