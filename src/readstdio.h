/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, djcj <djcj@gmx.de>
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

/* convenience macro to check for stdin using select();
 * READSTDIO(x) initialises int x and sets the return value of select()
 */

#ifndef FLTK_DIALOG_READSTDIO_H
#define FLTK_DIALOG_READSTDIO_H

#if _POSIX_C_SOURCE >= 200112L
# include <sys/select.h>
#else
# include <sys/time.h>
# include <sys/types.h>
#endif
#include <unistd.h>  /* STDIN_FILENO */

/*
// usage example:
int main(int argc, char **argv)
{
  READSTDIO(has_stdinput);

  if (has_stdinput == -1)
  {
    printf("error: select()\n");
    exit(1);
  }
  else if (has_stdinput)
  {
    printf("input received\n");
  }
  else
  {
    printf("error: no input data receiving\n");
    exit(1);
  }

  exit(0);
}
*/

#define READSTDIO(x) \
  fd_set readfds; \
  FD_ZERO(&readfds); \
  FD_SET(STDIN_FILENO, &readfds); \
  struct timeval timeout; \
  timeout.tv_sec = 0; \
  timeout.tv_usec = 0; \
  int x = select(1, &readfds, NULL, NULL, &timeout)

#endif  /* !FLTK_DIALOG_READSTDIO_H */

