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

#include <stdio.h>
#include <unistd.h>

static FILE *popen_gzip(const char *file)
{
  enum { r = 0, w = 1 };
  int fd[2];

  if (pipe(fd) == -1)
  {
    perror("pipe()");
    return NULL;
  }

  if (fork() == 0)
  {
    close(fd[r]);
    dup2(fd[w], 1);
    close(fd[w]);
    execl("/bin/gzip", "gzip", "-cd", file, NULL);
    _exit(127);
  }
  else
  {
    close(fd[w]);
    return fdopen(fd[r], "r");
  }

  return NULL;
}

char *gunzip(const char *file, size_t limit)
{
  FILE *fd;
  size_t size;
  char *data = NULL;

  fd = popen_gzip(file);

  if (fd == NULL)
  {
    return NULL;
  }

  data = new char[limit + 1]();
  size = fread(data, 1, limit, fd);
  pclose(fd);

  if (size == 0)
  {
    delete data;
    return NULL;
  }
  return data;
}

