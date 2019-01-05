/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2014 Victor Laskin <victor.laskin@gmail.com>
 * Copyright (c) 2019, djcj <djcj@gmx.de>
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
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_RGB_Image.H>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "ico_image.hpp"

// Extensively modified from original code made by Victor Laskin:
// http://vitiy.info/manual-decoding-of-ico-file-format-small-c-cross-platform-decoder-lib/
//
// The original copyright notice follows:
/*
 *	
 *	code by Victor Laskin (victor.laskin@gmail.com)
 *  rev 2 - 1bit color was added, fixes for bit mask
 *
 *
 *	
 *	THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
 *	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *	ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 *	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* ICO file header, 6 bytes */
typedef struct {
  uint16_t  idReserved;   /* Reserved */
  uint16_t  idType;       /* Resource type (1 for icons) */
  uint16_t  idCount;      /* How many images? */
} ICONDIR;

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

/* 46 bytes */
typedef struct {
  BITMAPINFOHEADER icHeader;  /* DIB header */
  uint32_t  icColors;         /* Color table */
  uint8_t   icXOR;            /* DIB bits for XOR mask */
  uint8_t   icAND;            /* DIB bits for AND mask */
} ICONIMAGE;


ico_image::ico_image(const char *filename) : Fl_RGB_Image(0,0,0)
{
  FILE *fp;
  ICONDIR *icoDir;
  ICONDIRENTRY *dirEntry;
  ICONIMAGE *icon;
  uchar hdr[6];
  uchar *cursor = NULL, *data, *ptr;
  unsigned highestRes, offset, numBytes, iconsCount, biBitCount;
  unsigned shift, shift2;
  int biHeight, x, y;
  long readSize, bytesRead;

  w(0); h(0); d(0); ld(0);
  alloc_array = 0;
  array = NULL;

  /* Open file */
  if ((fp = fl_fopen(filename, "rb")) == NULL) {
    ld(ERR_FILE_ACCESS);
    return;
  }

  /* Check file header */

  readSize = sizeof(ICONDIR);
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

  readSize = iconsCount * sizeof(ICONDIRENTRY);
  data = new uchar[readSize];
  bytesRead = fread(data, 1, readSize, fp);

  if (bytesRead != readSize) {
    ld(ERR_FILE_ACCESS);
    fclose(fp);
    delete data;
    return;
  }

  dirEntry = reinterpret_cast<ICONDIRENTRY *>(data);
  highestRes = 0;
  offset = 0;
  numBytes = 0;

  for (unsigned i = 0; i < iconsCount; i++) {
    unsigned width = dirEntry->bWidth;
    unsigned height = dirEntry->bHeight;

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

  delete data;

  /* Seek to offset and read data */

  if (offset == 0 || w() == 0 || h() == 0 ||
      fseek(fp, offset, SEEK_SET) == -1)
  {
    w(0); h(0); d(0); ld(ERR_FORMAT);
    fclose(fp);
    return;
  }

  data = new uchar[numBytes];
  bytesRead = fread(data, 1, numBytes, fp);
  fclose(fp);

  if (bytesRead != numBytes) {
    w(0); h(0); d(0); ld(ERR_FILE_ACCESS);
    delete data;
    return;
  }

  /* PNG compressed resource */
  if (memcmp(data, "\211PNG\r\n\032\n", 8) == 0) {
    if (numBytes == 0) {
      w(0); h(0); d(0); ld(ERR_FORMAT);
      delete data;
      return;
    }

    Fl_PNG_Image *png = new Fl_PNG_Image(NULL, data, numBytes);
    delete data;

    int loaded = png ? png->fail() : ERR_FILE_ACCESS;
    if (loaded < 0) {
      w(0); h(0); d(0); ld(loaded);
      if (png) {
        delete png;
      }
      return;
    }

    w(png->w());
    h(png->h());
    d(png->d());

    /* take over pointer of Fl_PNG_Image's array */
    array = png->array;
    alloc_array = png->alloc_array;
    png->array = NULL;
    png->alloc_array = 0;

    delete png;
    return;
  }

  /* Bitmap resource */

  cursor = data;
  icon = reinterpret_cast<ICONIMAGE *>(cursor);
  biBitCount = icon->icHeader.biBitCount;
  biHeight = icon->icHeader.biHeight;

  cursor += sizeof(BITMAPINFOHEADER);
  d(4);
  numBytes = w() * h() * d();
  array = new uchar[numBytes];
  alloc_array = 1;
  ptr = const_cast<uchar *>(array);

  if (biBitCount == 32) {
    for (x = 0; x < w(); x++) {
      for (y = 0; y < h(); y++) {
        shift = 4 * (x + y * w());
        shift2 = 4 * (x + (h() - y - 1) * w());
        ptr[shift] = cursor[shift2 + 2];
        ptr[shift + 1] = cursor[shift2 + 1];
        ptr[shift + 2] = cursor[shift2];
        ptr[shift + 3] = cursor[shift2 + 3];
      }
    }
  } else if (biBitCount == 24) {
    for (x = 0; x < w(); x++) {
      for (y = 0; y < h(); y++) {
        shift = 4 * (x + y * w());
        shift2 = 3 * (x + (h() - y - 1) * w());
        ptr[shift] = cursor[shift2 + 2];
        ptr[shift + 1] = cursor[shift2 + 1];
        ptr[shift + 2] = cursor[shift2];
        ptr[shift + 3] = 255;
      }
    }
  } else if (biBitCount == 8) {  /* 256 colors */
    unsigned index;
    uchar *colors = cursor;
    cursor += 256 * 4;

    for (x = 0; x < w(); x++) {
      for (y = 0; y < h(); y++) {
        shift = 4 * (x + y * w());
        shift2 = x + (h() - y - 1) * w();
        index = 4 * cursor[shift2];
        ptr[shift] = colors[index + 2];
        ptr[shift + 1] = colors[index + 1];
        ptr[shift + 2] = colors[index];
        ptr[shift + 3] = 255;
      }
    }
  } else if (biBitCount == 4) {  /* 16 colors */
    uchar index;
    uchar *colors = cursor;
    cursor += 16 * 4;

    for (x = 0; x < w(); x++) {
      for (y = 0; y < h(); y++) {
        shift = 4 * (x + y * w());
        shift2 = x + (h() - y - 1) * w();
        index = cursor[shift2 / 2];
        if (shift2 % 2 == 0) {
          index = (index >> 4) & 0xF;
        } else {
          index = index & 0xF;
        }
        index *= 4;
        ptr[shift] = colors[index + 2];
        ptr[shift + 1] = colors[index + 1];
        ptr[shift + 2] = colors[index];
        ptr[shift + 3] = 255;
      }
    }
  } else if (biBitCount == 1) {  /* 2 colors */
    unsigned boundary;
    uchar index, bit;
    uchar *colors = cursor;
    cursor += 2 * 4;

    /* 32 bit boundary */
    boundary = w();
    while (boundary % 32 != 0) {
      boundary++;
    }

    for (x = 0; x < w(); x++) {
      for (y = 0; y < h(); y++) {
        shift = 4 * (x + y * w());
        shift2 = x + (h() - y - 1) * boundary;
        index = cursor[shift2 / 8];
        /* select 1 bit only */
        bit = 7 - (x % 8);
        index = (index >> bit) & 0x01;
        index *= 4;
        ptr[shift] = colors[index + 2];
        ptr[shift + 1] = colors[index + 1];
        ptr[shift + 2] = colors[index];
        ptr[shift + 3] = 255;
      }
    }
  } else {
    /* unsupported color format */
    w(0); h(0); d(0); ld(ERR_FORMAT);
    if (alloc_array == 1) {
      delete array;
    }
    delete data;
    return;
  }

  /* Read AND mask after base color data - 1 BIT MASK */
  if ((biBitCount < 32) && (h() != biHeight)) {
    unsigned boundary;
    int mask;
    uchar bit;

    /* 32 bit boundary */
    boundary = w() * biBitCount;
    while (boundary % 32 != 0) {
      boundary++;
    }
    cursor += boundary * h() / 8;

    boundary = w();
    while (boundary % 32 != 0) {
      boundary++;
    }

    for (y = 0; y < h(); y++) {
      for (x = 0; x < w(); x++) {
        shift = 4 * (x + y * w()) + 3;
        bit = 7 - (x % 8);
        shift2 = (x + (h() - y - 1) * boundary) / 8;
        mask = 0x01 & (cursor[shift2] >> bit);
        ptr[shift] *= 1 - mask;
      }
    }
  }

  delete data;
}

