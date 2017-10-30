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

#include "gunzip_n.hpp"

char *gunzip_n(const char *file, size_t max)
{
  FILE *fp;
  char *buf = NULL;
  int status;
  size_t n;
  const int CHUNK_SIZE = 0x4000;
  Bytef in[CHUNK_SIZE];
  Bytef out[CHUNK_SIZE];
  z_stream strm;

  strm.next_in = in;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;

  if (inflateInit2(&strm, 15|32) != Z_OK)
  {
    return NULL;
  }

  if ((fp = fopen(file, "rb")) == NULL)
  {
    return NULL;
  }

  while (1)
  {
    strm.avail_in = (uInt)fread(in, sizeof(Bytef), sizeof(in), fp);

    if (ferror(fp))
    {
      goto close;
    }

    do
    {
      strm.avail_out = CHUNK_SIZE;
      strm.next_out = out;
      status = inflate(&strm, Z_NO_FLUSH);

      if (status != Z_OK && status != Z_STREAM_END)
      {
        goto close;
      }
      else if (max > 0 && (size_t)strm.total_out >= max)
      {
        buf = new char[max + 1];
        strncpy(buf, (const char *)out, max);
        buf[max+1] = '\0';
        goto close;
      }
    }
    while (strm.avail_out == 0);

    if (feof(fp))
    {
      n = (max > 0) ? max : (size_t)strm.total_out;
      buf = new char[n + 1];
      strncpy(buf, (const char *)out, n);
      buf[n+1] = '\0';
      goto close;
    }
  }

close:

  inflateEnd(&strm);

  if (fclose(fp) && buf)
  {
    delete buf;
  }

  if (buf)
  {
    return buf;
  }
  return NULL;
}

