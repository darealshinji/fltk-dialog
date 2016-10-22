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

#include <FL/Fl.H>
#include <FL/Fl_Color_Chooser.H>

#include <iostream>
#include <iomanip>
#include <ios>
#include <math.h>

#include "fltk-dialog.hpp"


int dialog_fl_color()
{
  double r = 1;
  double g = 1;
  double b = 1;

  if (title == NULL)
  {
    title = "FLTK color chooser";
  }

  if (fl_color_chooser(title, r,g,b, 1))
  {
    size_t colr = round(255 * r);
    size_t colg = round(255 * g);
    size_t colb = round(255 * b);

    double h = 0;
    double s = 0;
    double v = 0;

    Fl_Color_Chooser::rgb2hsv(r,g,b, h,s,v);

    std::cout
      /* RGB values [0.000-1.000] */
      << std::fixed << std::setprecision(3)
      << r << " " << g << " " << b
      << "|"
      /* RGB values [0-255] */
      << colr << " " << colg << " " << colb
      << "|"
      /* HTML hex value */
      << "#" << std::setfill('0') << std::setw(2) << std::hex
      << colr << colg << colb
      << "|"
      /* HSV values */
      << h << " " << s << " " << v
      << std::endl;

    return 0;
  }

  return 1;
}

