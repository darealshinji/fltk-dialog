#ifndef FLTK_DIALOG_READSTDIO_HPP
#define FLTK_DIALOG_READSTDIO_HPP

/* READSTDIO
 * convenient macros to check for stdin using select(2)
 */

/**
 * usage:
 *  INIT_READSTDIO;
 *  if (!READSTDIO) {
 *    fputs("error: no input data receiving");
 *    exit(1);
 *  }
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define INIT_READSTDIO \
  fd_set readfds; \
  FD_ZERO(&readfds); \
  FD_SET(STDIN_FILENO, &readfds); \
  struct timeval timeout; \
  timeout.tv_sec = 0; \
  timeout.tv_usec = 0;

#define READSTDIO (select(1, &readfds, NULL, NULL, &timeout))

#endif  /* !FLTK_DIALOG_READSTDIO_HPP */

