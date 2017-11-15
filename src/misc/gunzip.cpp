/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, djcj <djcj@gmx.de>
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

#include <sstream>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

#define CHUNK 16384

/* must be free()d later */
char *gunzip(const char *file, size_t limit)
{
  FILE *fp;
  int ret;
  z_stream strm;
  Bytef in[CHUNK];
  Bytef out[CHUNK];
  Bytef tmp[CHUNK];
  uInt have;
  char *str = NULL;
  std::stringstream ss;

  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;

  fp = fopen(file, "r");
  if (fp == NULL)
  {
    return NULL;
  }

  ret = inflateInit2(&strm, 31);
  if (ret != Z_OK)
  {
    goto close;
  }

  do
  {
    strm.avail_in = fread(in, 1, CHUNK, fp);
    if (ferror(fp))
    {
      goto clean;
    }

    if (strm.avail_in == 0)
    {
      break;
    }
    strm.next_in = in;

    do
    {
      strm.avail_out = CHUNK;
      strm.next_out = out;

      ret = inflate(&strm, Z_NO_FLUSH);
      switch (ret)
      {
        case Z_NEED_DICT:
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
          goto clean;
      }

      have = CHUNK - strm.avail_out;
      memset(tmp, 0, CHUNK);
      if (memcpy(tmp, out, have))
      {
        ss << tmp;
      }

      if (ss.str().length() >= limit)
      {
        goto clean;
      }
    }
    while (strm.avail_out == 0);
  }
  while (ret != Z_STREAM_END);

  str = strdup(ss.str().c_str());

clean:
  inflateEnd(&strm);
close:
  fclose(fp);
  return str;
}

