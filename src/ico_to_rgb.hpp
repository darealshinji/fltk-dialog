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
#include <stdint.h>

#define ICO_MAX  (1024*1024)

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

Fl_RGB_Image *ico_to_rgb(const char *file);

