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

#include <math.h>
#include <stdio.h>

#include "fltk-dialog.hpp"

static Fl_Double_Window *win;
static int ret = 1;

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
  Fl_Color_Chooser *c = dynamic_cast<Fl_Color_Chooser *>(o);
  ColorChip *v = reinterpret_cast<ColorChip *>(vv);
  v->r = static_cast<uchar>(255 * c->r() + 0.5);
  v->g = static_cast<uchar>(255 * c->g() + 0.5);
  v->b = static_cast<uchar>(255 * c->b() + 0.5);
  v->damage(FL_DAMAGE_EXPOSE);
}

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = p;
}

int dialog_color()
{
  Fl_Color_Chooser *chooser;
  ColorChip        *color;
  Fl_Box           *dummy;
  Fl_Group         *buttongroup;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  double r = 1, g = 1, b = 1;

  if (!title) {
    title = "color chooser";
  }

  win = new Fl_Double_Window(195 + 20, 200, title);
  win->callback(close_cb, 1);
  {
    chooser = new Fl_Color_Chooser(10, 10, 195, 115);
    color = new ColorChip(10, 130, 195, 25);
    color->r = color->g = color->b = uchar(255);

    buttongroup = new Fl_Group(10, 165, 195, 25);
    {
      int but_w = measure_button_width(fl_cancel, 20);
      but_cancel = new Fl_Button(win->w() - 10 - but_w, 165, but_w, 26, fl_cancel);
      but_cancel->callback(close_cb, 1);
      but_w = measure_button_width(fl_ok, 40);
      but_ok = new Fl_Return_Button(but_cancel->x() - 10 - but_w, 165, but_w, 26, fl_ok);
      but_ok->callback(close_cb, 0);
      dummy = new Fl_Box(but_ok->x() - 1, 165, 1, 1);
      dummy->box(FL_NO_BOX);
    }
    buttongroup->resizable(dummy);
    buttongroup->end();

    chooser->rgb(r,g,b);
    chooser->callback(callback, color);
    chooser->mode(1);
  }
  run_window(win, chooser, win->w(), win->h());

  if (ret == 0) {
    r = chooser->r();
    g = chooser->g();
    b = chooser->b();

    /* need to use round() so it matches
     * the values from the widgets */
    int colr = round(255 * r);
    int colg = round(255 * g);
    int colb = round(255 * b);

    double h = 0, s = 0, v = 0;
    Fl_Color_Chooser::rgb2hsv(r,g,b, h,s,v);

    printf("%s%.3f %.3f %.3f%s|", quote, r,g,b, quote);         /* RGB values [0.000-1.000] */
    printf("%s%d %d %d%s|", quote, colr,colg,colb, quote);      /* RGB values [0-255] */
    printf("%s#%02x%02x%02x%s|", quote, colr,colg,colb, quote); /* HTML hex value */
    printf("%s%.3f %.3f %.3f%s\n", quote, h,s,v, quote);        /* HSV values */
  }

  return ret;
}

