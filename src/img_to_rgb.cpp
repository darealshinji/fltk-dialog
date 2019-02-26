/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2019, djcj <djcj@gmx.de>
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
#include "ico_image.hpp"
#include "icns_image.hpp"

#define HASEXT(str,ext)  (strlastcasecmp(str,ext) == strlen(ext))

#define IMGFILE_MAX  (10*1024*1024)
#define SVGZ_MAX        (1024*1024)
#define SVG_MAX       (3*1024*1024)


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
  Fl_SVG_Image *svg = NULL;
  std::string data;

  if (!file) {
    return NULL;
  }

  if (compressed) {
    if (!gzip_uncompress(file, data)) {
      return NULL;
    }
    svg = new Fl_SVG_Image(NULL, data.c_str());
    data.clear();
  } else {
    svg = new Fl_SVG_Image(file, NULL);
  }

  if (!svg) {
    return NULL;
  }

  if (svg->fail() < 0) {
    delete svg;
    return NULL;
  }

  if (svg->w() > 64 || svg->h() > 64) {
    int w = 0, h = 0;
    aspect_ratio_scale(w, h, 64);
    svg->resize(w, h);
  }

  return svg;
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
    if (xpm->fail() < 0) {
      delete xpm;
      return NULL;
    }
    Fl_RGB_Image *rgb = new Fl_RGB_Image(xpm, Fl_Color(0));
    delete xpm;
    return rgb;
  }

  if (HASEXT(file, ".xbm")) {
    Fl_XBM_Image *in = new Fl_XBM_Image(file);
    if (!in) {
      return NULL;
    }
    if (in->fail() < 0) {
      delete in;
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
    if (!png) {
      return NULL;
    }
    if (png->fail() < 0) {
      delete png;
      return NULL;
    }
    return png;
  }

  if (memcmp(bytes, "\377\330\377", 3) == 0 && bytes[3] >= 0xc0 && bytes[3] <= 0xef) {
    Fl_JPEG_Image *jpeg = new Fl_JPEG_Image(file);
    if (!jpeg) {
      return NULL;
    }
    if (jpeg->fail() < 0) {
      delete jpeg;
      return NULL;
    }
    return jpeg;
  }

  if (bytes[0] == 'B' && bytes[1] == 'M') {
    Fl_BMP_Image *bmp = new Fl_BMP_Image(file);
    if (!bmp) {
      return NULL;
    }
    if (bmp->fail() < 0) {
      delete bmp;
      return NULL;
    }
    return bmp;
  }

  if (memcmp(bytes, "GIF87a", 6) == 0 || memcmp(bytes, "GIF89a", 6) == 0) {
    Fl_GIF_Image *gif = new Fl_GIF_Image(file);
    if (!gif) {
      return NULL;
    }
    if (gif->fail() < 0) {
      delete gif;
      return NULL;
    }
    Fl_RGB_Image *rgb = new Fl_RGB_Image(gif, Fl_Color(0));
    delete gif;
    return rgb;
  }

  if (memcmp(bytes, "\0\0\1\0", 4) == 0) {
    ico_image *ico = new ico_image(file);
    if (!ico) {
      return NULL;
    }
    if (ico->fail() < 0) {
      delete ico;
      return NULL;
    }
    return ico;
  }

  if (memcmp(bytes, "icns", 4) == 0 || memcmp(bytes, "mBIN", 4) == 0) {
    icns_image *icns = new icns_image(file);
    if (!icns) {
      return NULL;
    }
    if (icns->fail() < 0) {
      delete icns;
      return NULL;
    }
    return icns;
  }

  return NULL;
}

