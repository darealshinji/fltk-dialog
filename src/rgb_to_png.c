/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018, djcj <djcj@gmx.de>
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

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

static FILE *fp = NULL;
static png_struct *png = NULL;
static png_info *info = NULL;
static png_byte *row = NULL;

static void cleanup(void)
{
  if (fp) {
    fclose(fp);
  }

  if (png && info) {
    png_free_data(png, info, PNG_FREE_ALL, -1);
  }

  if (png) {
    png_destroy_write_struct(&png, NULL);
  }

  if (row) {
    free(row);
  }
}

int rgb_to_png(const char *file, const unsigned char *rgba, int w, int h)
{
  png_byte *px;
  int x, y, i;

  if ((fp = fopen(file, "wb")) == NULL) {
    return 1;
  }

  if ((png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL ||
      (info = png_create_info_struct(png)) == NULL ||
      setjmp(png_jmpbuf(png)) != 0)
  {
    cleanup();
    return 1;
  }

  png_init_io(png, fp);
  png_set_compression_level(png, 0);

  png_set_IHDR(png, info, w, h, 8,
               PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png, info);

  row = malloc(4 * w * sizeof(png_byte));

  for (y = 0; y < h; ++y) {
    for (x = 0; x < w; ++x) {
      px = &row[x * 4];
      i = (y*w + x) * 4;
      px[0] = rgba[i];
      px[1] = rgba[i + 1];
      px[2] = rgba[i + 2];
      px[3] = rgba[i + 3];
    }
    png_write_row(png, row);
  }
  png_write_end(png, NULL);

  cleanup();

  return 0;
}

