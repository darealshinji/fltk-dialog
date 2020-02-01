/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020, djcj <djcj@gmx.de>
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "fltk-dialog.hpp"
#include "ico_image.hpp"

/* ICO file header, 6 bytes */
typedef struct {
  uint16_t  idReserved;   /* Reserved */
  uint16_t  idType;       /* Resource type (1 for icons) */
  uint16_t  idCount;      /* How many images? */
} ICONDIR;
#define SZICONDIR 6

/* Image directory, 16 bytes */
typedef struct {
  uint8_t   bWidth;          /* Width of the image (0 means 256px) */
  uint8_t   bHeight;         /* Height of the image (0 means 256px) */
  uint8_t   bColorCount;     /* Number of colors in image (0 if >=8bpp) */
  uint8_t   bReserved;       /* Reserved */
  uint16_t  wPlanes;         /* Color Planes */
  uint16_t  wBitCount;       /* Bits per pixel */
  uint32_t  dwBytesInRes;    /* How many bytes in this resource? */
  uint32_t  dwImageOffset;   /* Where in the file is this image */
} ICONDIRENTRY;
#define SZICONDIRENTRY 16

/* 40 bytes */
typedef struct {
  uint32_t  biSize;
  uint32_t  biWidth;
  uint32_t  biHeight;
  uint16_t  biPlanes;
  uint16_t  biBitCount;
  uint32_t  biCompression;
  uint32_t  biSizeImage;
  uint32_t  biXPelsPerMeter;
  uint32_t  biYPelsPerMeter;
  uint32_t  biClrUsed;
  uint32_t  biClrImportant;
} BITMAPINFOHEADER;
#define SZBITMAPINFOHEADER 40

/* 14 bytes */
/*
typedef struct {
  uint16_t  bfType;
  uint32_t  bfSize;
  uint32_t  bfReserved;
  uint32_t  bfOffBits;
} BITMAPFILEHEADER;
*/
#define SZBITMAPFILEHEADER 14


ico_image::ico_image(const char *filename) : Fl_RGB_Image(0,0,0)
{
  Fl_PNG_Image *png = NULL;
  Fl_BMP_Image *bmp = NULL;
  Fl_RGB_Image *rgb = NULL;
  FILE *fp;
  ICONDIR *icoDir;
  ICONDIRENTRY *dirEntry;
  uchar hdr[6];
  uchar *ico_data, *ptr;
  uint highestRes, offset, iconsCount;
  size_t readSize, bytesRead, numBytes;

  w(0); h(0); d(0); ld(0);
  alloc_array = 0;
  array = NULL;

  /* Open file */
  if ((fp = fopen(filename, "r")) == NULL) {
    ld(ERR_FILE_ACCESS);
    return;
  }

  /* Check file header */

  readSize = SZICONDIR;
  bytesRead = fread(hdr, 1, readSize, fp);

  if (bytesRead != readSize) {
    ld(ERR_FILE_ACCESS);
    fclose(fp);
    return;
  }

  icoDir = reinterpret_cast<ICONDIR *>(hdr);
  iconsCount = icoDir->idCount;

  if (icoDir->idReserved != 0 || icoDir->idType != 1 || iconsCount == 0) {
    fclose(fp);
    return;
  }

  /* Check directory entries */

  readSize = iconsCount * SZICONDIRENTRY;
  ico_data = new uchar[readSize];
  bytesRead = fread(ico_data, 1, readSize, fp);

  if (bytesRead != readSize) {
    ld(ERR_FILE_ACCESS);
    fclose(fp);
    delete ico_data;
    return;
  }

  dirEntry = reinterpret_cast<ICONDIRENTRY *>(ico_data);
  highestRes = 0;
  offset = 0;
  numBytes = 0;

  for (uint i = 0; i < iconsCount; i++) {
    uint width = dirEntry->bWidth;
    uint height = dirEntry->bHeight;

    if (width == 0) {
      width = 256;
    }
    if (height == 0) {
      height = 256;
    }

    /* pick icon with highest resolution */
    if (width * height > highestRes) {
      w(width);
      h(height);
      offset = dirEntry->dwImageOffset;
      numBytes = dirEntry->dwBytesInRes;
      highestRes = w() * h();
    }

    dirEntry++;
  }

  delete ico_data;

  /* Seek to offset and read data */

  if (offset == 0 || w() == 0 || h() == 0 ||
      fseek(fp, offset, SEEK_SET) == -1)
  {
    w(0); h(0); d(0); ld(ERR_FORMAT);
    fclose(fp);
    return;
  }

  ico_data = new uchar[numBytes + SZBITMAPFILEHEADER];
  ptr = ico_data + SZBITMAPFILEHEADER;
  bytesRead = fread(ptr, 1, numBytes, fp);
  fclose(fp);

  if (bytesRead != numBytes) {
    w(0); h(0); d(0); ld(ERR_FILE_ACCESS);
    delete ico_data;
    return;
  }

  if (memcmp(ptr, "\211PNG\r\n\032\n", 8) == 0) {
    png = new Fl_PNG_Image(NULL, ptr, numBytes);
    rgb = png;
  } else {
    memset(ico_data + 2, 0, SZBITMAPFILEHEADER - 2);

    /* bfType */
    ico_data[0] = 'B';
    ico_data[1] = 'M';

    /* bfOffBits */
    ico_data[10] = SZBITMAPFILEHEADER + SZBITMAPINFOHEADER;

    bmp = new Fl_BMP_Image(NULL, ico_data);
    rgb = bmp;
  }

  delete ico_data;

  int loaded = rgb ? rgb->fail() : ERR_FILE_ACCESS;

  if (loaded < 0) {
    w(0); h(0); d(0); ld(loaded);
    if (rgb) {
      delete rgb;
    }
    return;
  }

  w(rgb->w());
  h(rgb->h());
  d(rgb->d());
  array = rgb->array;
  alloc_array = 1;

  rgb->array = NULL;
  rgb->alloc_array = 0;
  delete rgb;
}

