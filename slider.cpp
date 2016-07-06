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
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Widget.H>

/* std::cout, std::endl */
#include <iostream>
/* atof */
#include <stdlib.h>
/* strlen */
#include <string.h>

#include "fltk-dialog.h"

double sliderval = 0;


static void sliderval_callback(Fl_Widget* o, void*)
{
  sliderval = ((Fl_Valuator*)o)->value();
}

void slider_ok_cb(Fl_Widget*, void*)
{
  std::cout << sliderval << std::endl;
  exit(0);
}

void slider_close_cb(Fl_Widget*, void*)
{
  exit(1);
}

int dialog_fl_value_slider(char *slider_msg,
                           char *slider_title,
                           char *slider_min,
                           char *slider_max,
                           char *slider_step,
                           char *slider_val)
{
  Fl_Window *w;
  int winw = 320;
  int slidh = 30;
  int bord = 10;
  int textheight = FL_NORMAL_SIZE + 4;
  int textlines = 1;
  int butw = 100;
  int buth = 25;

  double min = 0;
  double max = 100;
  double step = 1;
  double val = min;

  if (slider_msg == NULL) {
    slider_msg = (char *)"Simple FLTK scaler";
  } else {
    slider_msg = translate(slider_msg);
    for (size_t i = 0; i < strlen(slider_msg); i++) {
      if (slider_msg[i] == '\n')
        textlines++;
    }
  }

  if (slider_title == NULL) {
    slider_title = (char *)"FLTK slider window";
  }

  if (slider_min  != NULL) { min = atof(slider_min); }
  if (slider_max  != NULL) { max = atof(slider_max); }
  if (slider_step != NULL) { step = atof(slider_step); }
  if (slider_val  != NULL) { val = atof(slider_val); }

  int boxh = textlines*textheight + bord*2;
  { Fl_Window *o = new Fl_Window(winw,
                                 boxh+slidh+bord*3+textheight,
                                 slider_title);
    w = o; if (w) {/**/}
    { Fl_Box *o = new Fl_Box(0,
                             0,
                             bord,
                             boxh,
                             slider_msg);
      o->box(FL_NO_BOX);
      o->align(FL_ALIGN_RIGHT);
    } // Fl_Box *o
    { Fl_Value_Slider *o = new Fl_Value_Slider(bord,
                                               boxh,
                                               winw-bord*2,
                                               slidh,
                                               NULL);
      o->type(FL_HOR_NICE_SLIDER);
      o->box(FL_FLAT_BOX);
      o->minimum(min);
      o->maximum(max);
      o->step(step);
      o->value(val);
      o->callback((Fl_Callback*)sliderval_callback);
    } // Fl_Value_Slider *o
    { Fl_Return_Button *o = new Fl_Return_Button(winw-butw*2-bord*2,
                                                 boxh+textheight+buth,
                                                 butw,
                                                 buth,
                                                 fl_ok);
      o->callback(slider_ok_cb);
    } // Fl_Button *o
    { Fl_Button *o = new Fl_Button(winw-butw-bord,
                                   boxh+textheight+buth,
                                   butw,
                                   buth,
                                   fl_cancel);
      o->callback(slider_close_cb);
    } // Fl_Button *o
    o->end();
  } // Fl_Window *o
  w->show();
  int ret = Fl::run();
  std::cout << sliderval << std::endl;

  return ret;
}

