/**
 *  Copyright (c) 2013-2014 Victor Laskin <victor.laskin@gmail.com>
 *  Copyright (c) 2019 djcj <djcj@gmx.de>
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 *  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Original source:
 *  http://vitiy.info/manual-decoding-of-ico-file-format-small-c-cross-platform-decoder-lib/
 *
 * ICO format resources:
 *  http://www.daubnet.com/en/file-format-ico
 *  https://en.wikipedia.org/wiki/ICO_(file_format)#Icon_resource_structure
 */

#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_RGB_Image.H>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "ico_to_rgb.hpp"


static unsigned int load_ico
(
  unsigned char *buffer,    /* input - buffer data */
  unsigned int &width,      /* output - width */
  unsigned int &height,     /* output - height */
  bool &png,                /* output - whether image data is png or rgba */
  unsigned char * &image    /* output - image data */
)
{
  ICONDIR *icoDir = reinterpret_cast<ICONDIR *>(buffer);
  unsigned int iconsCount = icoDir->idCount;

  width = height = 0;
  png = false;

  if (icoDir->idReserved != 0 || icoDir->idType != 1 || iconsCount == 0 || iconsCount > 20) {
    return 0;
  }

  unsigned char *cursor = buffer + 6;
  ICONDIRENTRY *dirEntry = reinterpret_cast<ICONDIRENTRY *>(cursor);

  unsigned int maxSize = 0;
  unsigned int offset = 0;
  unsigned int numBytes = 0;

  for (unsigned int i = 0; i < iconsCount; i++) {
    unsigned int w = dirEntry->bWidth;
    unsigned int h = dirEntry->bHeight;

    if (w == 0) {
      w = 256;
    }
    if (h == 0) {
      h = 256;
    }

    /* pick icon with highest resolution */
    if (w * h > maxSize) {
      width = w;
      height = h;
      offset = dirEntry->dwImageOffset;
      numBytes = dirEntry->dwBytesInRes;
      maxSize = w * h;
    }

    dirEntry++;
  }

  if (offset == 0) {
    return 0;
  }

  cursor = buffer + offset;

  /* PNG compressed resource */
  if (memcmp(cursor, "\211PNG\r\n\032\n", 8) == 0) {
    if (numBytes > 0) {
      png = true;
      image = new unsigned char[numBytes];
      memcpy(image, cursor, numBytes);
    }
    return numBytes;
  }

  ICONIMAGE *icon = reinterpret_cast<ICONIMAGE *>(cursor);
  unsigned int realBitsCount = icon->icHeader.biBitCount;
  bool hasAndMask = (realBitsCount < 32) && (height != icon->icHeader.biHeight);

  cursor += 40;
  numBytes = width * height * 4;
  image = new unsigned char[numBytes];

  /* rgba + vertical swap */
  if (realBitsCount == 32) {
    unsigned int x, y, shift, shift2;
    for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
        shift = 4 * (x + y * width);
        shift2 = 4 * (x + (height - y - 1) * width);
        image[shift] = cursor[shift2 + 2];
        image[shift + 1] = cursor[shift2 + 1];
        image[shift + 2] = cursor[shift2];
        image[shift + 3] = cursor[shift2 + 3];
      }
    }
  }

  if (realBitsCount == 24) {
    unsigned int x, y, shift, shift2;
    for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
        shift = 4 * (x + y * width);
        shift2 = 3 * (x + (height - y - 1) * width);
        image[shift] = cursor[shift2 + 2];
        image[shift + 1] = cursor[shift2 + 1];
        image[shift + 2] = cursor[shift2];
        image[shift + 3] = 255;
      }
    }
  }

  /* 256 colors */
  if (realBitsCount == 8) {
    unsigned int x, y, shift, shift2, index;
    unsigned char *colors = cursor;
    cursor += 256 * 4;

    for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
        shift = 4 * (x + y * width);
        shift2 = x + (height - y - 1) * width;
        index = 4 * cursor[shift2];
        image[shift] = colors[index + 2];
        image[shift + 1] = colors[index + 1];
        image[shift + 2] = colors[index];
        image[shift + 3] = 255;
      }
    }
  }

  /* 16 colors */
  if (realBitsCount == 4) {
    unsigned int x, y, shift, shift2;
    unsigned char index;
    unsigned char *colors = cursor;
    cursor += 16 * 4;

    for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
        shift = 4 * (x + y * width);
        shift2 = x + (height - y - 1) * width;
        index = cursor[shift2 / 2];
        if (shift2 % 2 == 0) {
          index = (index >> 4) & 0xF;
        } else {
          index = index & 0xF;
        }
        index *= 4;
        image[shift] = colors[index + 2];
        image[shift + 1] = colors[index + 1];
        image[shift + 2] = colors[index];
        image[shift + 3] = 255;
      }
    }
  }

  /* 2 colors */
  if (realBitsCount == 1) {
    unsigned int x, y, shift, shift2, boundary;
    unsigned char index, bit;
    unsigned char *colors = cursor;
    cursor += 2 * 4;

    /* 32 bit boundary */
    boundary = width;
    while (boundary % 32 != 0) {
      boundary++;
    }

    for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
        shift = 4 * (x + y * width);
        shift2 = x + (height - y - 1) * boundary;
        index = cursor[shift2 / 8];
        /* select 1 bit only */
        bit = 7 - (x % 8);
        index = (index >> bit) & 0x01;
        index *= 4;
        image[shift] = colors[index + 2];
        image[shift + 1] = colors[index + 1];
        image[shift + 2] = colors[index];
        image[shift + 3] = 255;
      }
    }
  }

  /* Read AND mask after base color data - 1 BIT MASK */
  if (hasAndMask) {
    unsigned int x, y, shift, shift2, boundary;
    unsigned char bit;
    int mask;

    /* 32 bit boundary */
    boundary = width * realBitsCount;
    while (boundary % 32 != 0) {
      boundary++;
    }
    cursor += boundary * height / 8;

    boundary = width;
    while (boundary % 32 != 0) {
      boundary++;
    }

    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
        shift = 4 * (x + y * width) + 3;
        bit = 7 - (x % 8);
        shift2 = (x + (height - y - 1) * boundary) / 8;
        mask = 0x01 & (cursor[shift2] >> bit);
        image[shift] *= 1 - mask;
      }
    }
  }

  return numBytes;
}

Fl_RGB_Image *ico_to_rgb(const char *file)
{
  unsigned char buffer[ICO_MAX];
  size_t size;
  unsigned char *data = NULL;
  unsigned int w = 0, h = 0, data_len;
  bool is_png = false;
  Fl_RGB_Image *rgb, *copy;

  FILE *fp = fopen(file, "rb");

  if (!fp) {
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);

  if (size > ICO_MAX) {
    fclose(fp);
    return NULL;
  }

  rewind(fp);
  if (fread(&buffer, 1, size, fp) != size) {
    fclose(fp);
    return NULL;
  }
  fclose(fp);

  data_len = load_ico(buffer, w, h, is_png, data);

  if (!data) {
    return NULL;
  }

  if (data_len == 0) {
    delete data;
    return NULL;
  }

  if (is_png) {
    rgb = new Fl_PNG_Image(NULL, data, data_len);
    delete data;
    return rgb;
  }

  rgb = new Fl_RGB_Image(data, w, h, 4);
  copy = dynamic_cast<Fl_RGB_Image *>(rgb->copy());

  delete data;
  delete rgb;
  return copy;
}

