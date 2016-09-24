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
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Valuator.H>
#include <FL/fl_ask.H>  /* fl_ok, fl_cancel */
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Widget.H>

#include <string>    /* std::string */
#include <iostream>  /* std::cout, std::endl */
#include <stdlib.h>  /* exit, atof */
#include <string.h>  /* strlen */

#include "fltk-dialog.hpp"


Fl_Value_Slider *slider;
double slidval_round;

static void slider_cb(Fl_Widget *o)
{
  slidval_round = ((Fl_Valuator *)o)->value();
}

static void slider_ok_cb(Fl_Widget *w)
{
  w->window()->hide();
  return;
}

static void slider_cancel_cb(Fl_Widget*)
{
  delete slider;
  exit(1);
}

int dialog_fl_value_slider(char *slider_min,
                           char *slider_max,
                           char *slider_step,
                           char *slider_val)
{
  Fl_Window        *win;
  Fl_Box           *box;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  std::string s;
  int winw = 320;
  int slidh = 30;
  int bord = 10;
  int textheight = 18;
  int textlines = 1;
  int butw = 100;
  int buth = 26;

  double min = 0;
  double max = 100;
  double step = 1;
  double val = min;

  if (msg == NULL)
  {
    s = "Simple FLTK scaler";
  }
  else
  {
    s = translate(msg);
    for (size_t i = 0; i < strlen(msg); i++)
    {
      if (msg[i] == '\n')
      {
        textlines++;
      }
    }
  }

  if (title == NULL)
  {
    title = (char *)"FLTK slider window";
  }

  if (slider_min != NULL)
  {
    min = atof(slider_min);
  }

  if (slider_max != NULL)
  {
    max = atof(slider_max);
  }

  if (slider_step != NULL)
  {
    step = atof(slider_step);
  }

  if (slider_val != NULL)
  {
    val = atof(slider_val);
  }

  int boxh = textlines*textheight + bord*2;

  win = new Fl_Window(winw, boxh+slidh+bord*3+textheight, title);
  win->callback(slider_cancel_cb);  /* exit(1) */
  {
    box = new Fl_Box(0, 0, bord, boxh, s.c_str());
    box->box(FL_NO_BOX);
    box->align(FL_ALIGN_RIGHT);

    slider = new Fl_Value_Slider(bord, boxh, winw-bord*2, slidh, NULL);
    slider->type(FL_HOR_NICE_SLIDER);
    slider->box(FL_FLAT_BOX);
    slider->minimum(min);
    slider->maximum(max);
    slider->step(step);
    slidval_round = slider->round(val);
    slider->value(slidval_round);
    slider->callback(slider_cb);

    but_ok = new Fl_Return_Button(winw-butw*2-bord*2, boxh+textheight+buth,
                                  butw, buth, fl_ok);
    but_ok->callback(slider_ok_cb);
    but_cancel = new Fl_Button(winw-butw-bord, boxh+textheight+buth,
                               butw, buth, fl_cancel);
    but_cancel->callback(slider_cancel_cb);
  }
  win->end();
  win->show();

  int ret = Fl::run();
  std::cout << slidval_round << std::endl;
  delete slider;
  return ret;
}

