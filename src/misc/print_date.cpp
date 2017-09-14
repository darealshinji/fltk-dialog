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

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <time.h>

#include "fltk-dialog.hpp"

void print_date(std::string format, int y, int m, int d)
{
  std::stringstream ss;

  char date[512] = {0};
  struct tm time;

  if (format == "")
  {
    format = "%Y-%m-%d";
  }
  else
  {
    /* glibc date formats
     * example date: 2006-01-08 */
    repstr("\\n", "\n",  format);  /* newline */
    repstr("\\t", "\t",  format);  /* tab */
    repstr("%",   "%%",  format);  /* literal % */
    repstr("d",   "%-d", format);  /* day (8) */
    repstr("D",   "%d",  format);  /* day (08) */
    repstr("m",   "%-m", format);  /* month (1) */
    repstr("M",   "%m",  format);  /* month (01) */
    repstr("y",   "%y",  format);  /* year (06) */
    repstr("Y",   "%Y",  format);  /* year (2006) */
    repstr("j",   "%-j", format);  /* day of the year (8) */
    repstr("J",   "%j",  format);  /* day of the year (008) */
    repstr("W",   "%A",  format);  /* weekday name (Sunday) */
    repstr("w",   "%a",  format);  /* weekday name (Sun) */
    repstr("n",   "%-V", format);  /* ISO 8601 week number (1) */
    repstr("N",   "%V",  format);  /* ISO 8601 week number (01) */
    repstr("B",   "%B",  format);  /* month name (January) */
    repstr("b",   "%b",  format);  /* month name (Jan) */
    repstr("u",   "%u",  format);  /* day of the week, Monday being 1 (7) */
  }

  ss << y << "-" << m << "-" << d;
  memset(&time, 0, sizeof(struct tm));
  strptime(ss.str().c_str(), "%Y-%m-%d", &time);
  strftime(date, sizeof(date)-1, format.c_str(), &time);

  std::cout << date << std::endl;
}
