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

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Color_Chooser.H>

#include <math.h>
#include <stdio.h>

#include "fltk-dialog.hpp"

static Fl_Double_Window *win;

class ColorChip : public Fl_Widget
{
  void draw();

public:
  uchar r,g,b;

  ColorChip(int X, int Y, int W, int H) : Fl_Widget(X,Y,W,H) {
    box(FL_ENGRAVED_FRAME);
  }
};

static void callback(Fl_Widget *o, void *vv)
{
  Fl_Color_Chooser *c = (Fl_Color_Chooser *)o;
  ColorChip *v = (ColorChip *)vv;
  v->r = uchar(255 * c->r() + 0.5);
  v->g = uchar(255 * c->g() + 0.5);
  v->b = uchar(255 * c->b() + 0.5);
  v->damage(FL_DAMAGE_EXPOSE);
}

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = (int) p;
}

int dialog_color()
{
  Fl_Color_Chooser *chooser;
  ColorChip        *color;
  Fl_Box           *dummy;
  Fl_Group         *buttongroup;
  Fl_Return_Button *ok_button;
  Fl_Button        *cancel_button;

  double r = 1, g = 1, b = 1;

  if (!title) {
    title = "color chooser";
  }

  win = new Fl_Double_Window(215, 200, title);
  win->size_range(215, 200, max_w, max_h);
  win->callback(close_cb, 1);
  {
    chooser = new Fl_Color_Chooser(10, 10, 195, 115);
    color = new ColorChip(10, 130, 195, 25);
    color->r = color->g = color->b = uchar(255);

    buttongroup = new Fl_Group(10, 165, 195, 25);
    {
      dummy = new Fl_Box(0, 165, 1, 25);
      ok_button = new Fl_Return_Button(10, 165, 95, 25, fl_ok);
      ok_button->callback(close_cb, 0);
      cancel_button = new Fl_Button(110, 165, 95, 25, fl_cancel);
      cancel_button->callback(close_cb, 1);
    }
    buttongroup->resizable(dummy);
    buttongroup->end();

    chooser->rgb(r,g,b);
    chooser->callback(callback, color);
    chooser->mode(1);
  }
  win->end();
  run_window(win, chooser);

  if (ret == 0) {
    r = chooser->r();
    g = chooser->g();
    b = chooser->b();

    int colr = round(255 * r);
    int colg = round(255 * g);
    int colb = round(255 * b);

    double h = 0, s = 0, v = 0;
    Fl_Color_Chooser::rgb2hsv(r,g,b, h,s,v);

    printf("%.3f %.3f %.3f|", r, g, b);         /* RGB values [0.000-1.000] */
    printf("%d %d %d|", colr, colg, colb);      /* RGB values [0-255] */
    printf("#%02x%02x%02x|", colr, colg, colb); /* HTML hex value */
    printf("%.3f %.3f %.3f\n", h,s,v);          /* HSV values */
  }

  return ret;
}

