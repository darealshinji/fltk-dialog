/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018, djcj <djcj@gmx.de>
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

#include "fltk-dialog.hpp"

class help_dialog : public Fl_Help_Dialog
{
public:
  help_dialog() : Fl_Help_Dialog() {}
  void border(int b) { window_->border(b); }
  void always_on_top() { set_always_on_top(window_); }
  void size_range(int minw, int minh, int maxw, int maxh) {
    window_->size_range(minw, minh, maxw, maxh);
  }
};

int dialog_html_viewer(const char *file)
{
  help_dialog *o = new help_dialog();
  o->load(file);

  if (!window_taskbar) {
    o->border(0);
  }

  o->show();

  int new_x = o->x();
  int new_y = o->y();
  int new_w = o->w();
  int new_h = o->h();

  if (resizable) {
    if (override_w > 0) {
      new_w = override_w;
    }
    if (override_h > 0) {
      new_h = override_h;
    }
  }

  if (position_center) {
    new_x = (max_w - new_w) / 2;
    new_y = (max_h - new_h) / 2;
  } else {
    if (override_x >= 0) {
      new_x = override_x;
    }
    if (override_y >= 0) {
      new_y = override_y;
    }
  }

  o->resize(new_x, new_y, new_w, new_h);
  o->border(window_decoration ? 1 : 0);

  if (always_on_top) {
    o->always_on_top();
  }

  if (resizable) {
    o->size_range(340, 100, max_w, max_h);
  } else {
    o->size_range(o->w(), o->h(), o->w(), o->h());
  }

  Fl::run();

  return 0;
}

