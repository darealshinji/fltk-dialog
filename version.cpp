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

#include <FL/Fl.H>   /* Fl::api_version */

#include <string>    /* std::string, c_str, substr, */
#include <iostream>  /* std::cout, std::endl */
#include <sstream>   /* std::stringstream, str */
#include <string.h>  /* strcmp */


/* get the version strings from the linked in static or
 * the runtime library without leading zeros */
std::string get_fltk_version()
{
  std::stringstream ss;
  std::string ver, maj, min1, min2, pat1, pat2;
  int api = Fl::api_version();
  ss << api;
  ver  = ss.str();
  maj  = ver.substr(0,1);
  min1 = ver.substr(1,1);
  min2 = ver.substr(2,1);
  pat1 = ver.substr(3,1);
  pat2 = ver.substr(4,1);
  if (strcmp(min1.c_str(), "0") == 0) { min1 = ""; }
  if (strcmp(pat1.c_str(), "0") == 0) { pat1 = ""; }
  return maj + "." + min1 + min2 + "." + pat1 + pat2;
}

void print_fltk_version()
{
  std::cout << "using FLTK version " << get_fltk_version() << std::endl;
}

