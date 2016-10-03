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
#include <FL/Fl_Help_Dialog.H>

#include "fltk-dialog.hpp"


int dialog_html_viewer(const char *file)
{
  Fl_Help_Dialog *v = new Fl_Help_Dialog();
  v->load(file);

  int new_x = v->x();
  int new_y = v->y();
  int new_w = v->w();
  int new_h = v->h();

  if (resizable)
  {
    if (override_w > 0)
    {
      new_w = override_w;
    }

    if (override_h > 0)
    {
      new_h = override_h;
    }
  }

  if (position_center)
  {
    new_x = (max_w - new_w) / 2;
    new_y = (max_h - new_h) / 2;
  }
  else
  {
    if (override_x >= 0)
    {
      new_x = override_x;
    }

    if (override_y >= 0)
    {
      new_y = override_y;
    }
  }

  v->resize(new_x, new_y, new_w, new_h);
  v->show();
  Fl::run();

  return 0;
}

