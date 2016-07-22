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

/* std::cout, std::endl */
#include <iostream>
/* std::setfill, std::setw */
#include <iomanip>
/* std::hex */
#include <ios>
/* round */
#include <math.h>


#define INT_TO_HEX(x) std::setfill('0') << std::setw(2) << std::hex << x

int dialog_fl_color(char *color_title,
                    int   html)
{
  double r,g,b;
  r = g = b = 1.0;

  if (color_title == NULL) {
    color_title = (char *)"FLTK color chooser";
  }

  if (fl_color_chooser(color_title, r,g,b, 1)) {
    size_t colr = round(255*r);
    size_t colg = round(255*g);
    size_t colb = round(255*b);
    if (html == 1) {
      /* html hex value */
      std::cout << "#" << INT_TO_HEX(colr) << INT_TO_HEX(colg) << INT_TO_HEX(colb) << std::endl;
    } else {
      /* RGB values ranging from 0 to 255 */
      std::cout << colr << " " << colg << " " << colb << std::endl;
    }
    return 0;
  } else {
    return 1;
  }
}

#undef INT_TO_HEX

