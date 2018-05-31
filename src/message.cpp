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
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Valuator.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <string.h>

#include "fltk-dialog.hpp"

static Fl_Double_Window *win;
static double value = 0, scale_step = 1;
static int ret = 1;

static void close_cb(Fl_Widget *, long p) {
  win->hide();
  ret = (int) p;
}

static void callback(Fl_Widget *o, void *p)
{
  std::stringstream ss;
  value = ((Fl_Valuator *)o)->value();

  if (scale_step == (float)((int) scale_step)) {
    /* integer value */
    ss << (int) value;
  } else {
    /* floating point value;
     * convert into char with fixed positions after decimal point */
    ss << (scale_step - (float)((int) scale_step));
    int precision = ss.str().size() - 2;
    ss.str("");
    ss.clear();
    ss << std::fixed << std::setprecision(precision) << value;
  }

  ((Fl_Box *)p)->copy_label(ss.str().c_str());
  Fl::redraw();
}

int dialog_message(int type
,                  bool with_icon_box
,                  const char *label_but_alt
,                  bool arabic
,                  double scale_min
,                  double scale_max
,                  double scale_step_
,                  double scale_init)
{
  Fl_Group         *g_icon, *g_box, *g_message, *g_middle, *g_buttons;
  Fl_Box           *icon, *box, *slider_box = NULL, *dummy;
  Fl_Input         *input = NULL;
  Fl_Slider        *slider = NULL;
  Fl_Return_Button *but_ret;
  Fl_Button        *but, *but_alt;

  std::string s;
  int msg_w, msg_h, win_w, win_h, min_w, min_h, input_off = 0;
  int label_but_ret_w, label_but_w = 0, label_but_alt_w = 0;
  int esc_ret = 1;
  bool input_field = false;
  bool scaler_field = false;
  const char *label_icon = "!";
  const char *label_but_ret = fl_ok;
  const char *label_but = fl_cancel;


  /* labels and message type settings */

  if (!title) {
    title = "Window Title";
  }

  if (msg) {
#ifdef WITH_FRIBIDI
    char *tmp = NULL;
    if (use_fribidi) {
      tmp = fribidi_parse_line(msg);
    }
    if (tmp) {
      s = translate(tmp);
      delete tmp;
      msg = s.c_str();
    } else
#endif
    {
      s = translate(msg);
      msg = s.c_str();
    }
  }

  if (!msg) {
    msg = "No message";
  }

  switch (type) {
    case MESSAGE_TYPE_INFO:
      label_but_ret = fl_close;
      label_but_alt = NULL;
      label_icon = "i";
      esc_ret = 0;
      break;
    case MESSAGE_TYPE_QUESTION:
      label_but_ret = fl_yes;
      label_but = fl_no;
      label_icon = "?";
      break;
    case MESSAGE_TYPE_INPUT:
    case MESSAGE_TYPE_PASSWORD:
      input_field = true;
      with_icon_box = false;
      break;
    case MESSAGE_TYPE_SCALE:
      scaler_field = true;
      with_icon_box = false;
      scale_step = scale_step_;
      if (scale_init < scale_min) {
        scale_init = scale_min;
      } else if (scale_init > scale_max) {
        scale_init = scale_max;
      }
      break;
  }


  /* window measures */

  Fl_Box *tmp = new Fl_Box(0,0,0,0, msg);
  fl_font(tmp->labelfont(), tmp->labelsize());
  msg_w = msg_h = 0;
  fl_measure(tmp->label(), msg_w, msg_h);
  delete tmp;

  label_but_ret_w = measure_button_width(label_but_ret, 40);

  if (type != MESSAGE_TYPE_INFO) {
    label_but_w = measure_button_width(label_but, 15);
  }
  if (label_but_alt) {
    label_but_alt_w = measure_button_width(label_but_alt, 15);
  }

  win_w = msg_w + 26;
  win_h = msg_h + 58;

  min_h = 74;
  min_w = label_but_ret_w + label_but_w + label_but_alt_w + 20;

  if (label_but_alt) {
    min_w += 20;
  } else if (type != MESSAGE_TYPE_INFO) {
    min_w += 10;
  }

  if (with_icon_box) {
    win_w += 60;
    win_h += 40;
    min_h += 30;
  }

  if (input_field) {
    input_off = 40;
  } else if (scaler_field) {
    input_off = 60;
  }
  win_h += input_off;
  min_h += input_off;

  if (win_w < min_w) {
    win_w = min_w;
  }

  if (win_h < min_h) {
    win_h = min_h;
  }


  /* initialize window */

  win = new Fl_Double_Window(win_w, win_h, title);
  win->size_range(min_w, min_h, max_w, max_h);
  win->callback(close_cb, esc_ret);
  {
    int box_x, box_w;
    int box_h = win_h - input_off - 58;

    g_message = new Fl_Group(0, 0, win_w, box_h);
    {
      /* icon box */
      if (with_icon_box) {
        box_x = 70;
        box_w = win_w - 80;

        g_icon = new Fl_Group(0, 0, box_x, box_h);
        {
          icon = new Fl_Box(10, 10, 50, 50);
          icon->box(FL_THIN_UP_BOX);
          icon->labelfont(FL_TIMES_BOLD);
          icon->labelsize(34);
          icon->color(FL_WHITE);
          icon->labelcolor(FL_BLUE);
          icon->label(label_icon);
        }
        g_icon->resizable(NULL);
        g_icon->end();
      } else {
        box_x = 10;
        box_w = win_w - 20;
      }

      /* message area */
      g_box = new Fl_Group(box_x, 0, box_w, box_h);
      {
        box = new Fl_Box(box_x, 10, box_w, box_h, msg);
        Fl_Align align_tlr = arabic ? FL_ALIGN_TOP_RIGHT : FL_ALIGN_TOP_LEFT;
        box->align(FL_ALIGN_INSIDE|align_tlr|FL_ALIGN_WRAP);
        box->box(FL_NO_BOX);
      }
      g_box->resizable(box);
      g_box->end();
    }
    g_message->resizable(box);
    g_message->end();

    /* input field / scaler */
    if (input_field) {
      g_middle = new Fl_Group(/*x*/ 0,
                              /*y*/ win_h - input_off - 38,
                              /*w*/ win_w,
                              /*h*/ 30);
      {
        input = new Fl_Input(/*x*/ 10,
                             /*y*/ win_h - input_off - 38,
                             /*w*/ win_w - 20,
                             /*h*/ 30);
        if (type == MESSAGE_TYPE_PASSWORD) {
          input->type(FL_SECRET_INPUT);
        }
      }
      g_middle->resizable(input);
      g_middle->end();
    } else if (scaler_field) {
      g_middle = new Fl_Group(/*x*/ 0,
                              /*y*/ win_h - input_off - 38,
                              /*w*/ win_w,
                              /*h*/ 30);
      {
        slider_box = new Fl_Box(/*x*/ 10,
                                /*y*/ win_h - input_off - 8,
                                /*w*/ win_w - 20,
                                /*h*/ 30);
        slider_box->box(FL_FLAT_BOX);
        slider_box->align(FL_ALIGN_INSIDE|FL_ALIGN_RIGHT);

        slider = new Fl_Slider(/*x*/ 10,
                               /*y*/ win_h - input_off - 38,
                               /*w*/ win_w - 20,
                               /*h*/ 30);
        slider->type(FL_HOR_NICE_SLIDER);
        slider->box(FL_NO_BOX);
        slider->bounds(scale_min, scale_max);
        slider->step(scale_step);
        value = slider->round(scale_init);
        slider->value(value);
        slider->callback(callback, slider_box);
        callback(slider, slider_box); /* set initial label */
      }
      g_middle->resizable(slider);
      g_middle->end();
    }

    /* buttons */
    g_buttons = new Fl_Group(0, win_h - 58, win_w, 58);
    {
      int offset_alt = label_but_alt ? label_but_alt_w + 10 : 0;
      int offset = label_but_ret_w + offset_alt;
      offset += (type == MESSAGE_TYPE_INFO) ? 10 : label_but_w + 20;

      dummy = new Fl_Box(/*x*/ win_w - offset - 1,
                         /*y*/ win_h - 48,
                         /*w*/ 1,
                         /*h*/ 1);
      dummy->box(FL_NO_BOX);

      but_ret = new Fl_Return_Button(/*x*/ win_w - offset,
                                     /*y*/ win_h - 38,
                                     /*w*/ label_but_ret_w,
                                     /*h*/ 28,
                                     /*l*/ label_but_ret);
      but_ret->callback(close_cb, 0);

      if (type != MESSAGE_TYPE_INFO) {
        but = new Fl_Button(/*x*/ win_w - label_but_w - 10 - offset_alt,
                            /*y*/ win_h - 38,
                            /*w*/ label_but_w,
                            /*h*/ 28,
                            /*l*/ label_but);
        but->callback(close_cb, 1);
      }

      if (label_but_alt) {
        but_alt = new Fl_Button(/*x*/ win_w - label_but_alt_w - 10,
                                /*y*/ win_h - 38,
                                /*w*/ label_but_alt_w,
                                /*h*/ 28,
                                /*l*/ label_but_alt);
        but_alt->callback(close_cb, 2);
      }
    }
    g_buttons->resizable(dummy);
    g_buttons->end();
  }
  run_window(win, g_box);

  if (ret == 0) {
    if (input_field) {
      std::cout << input->value() << std::endl;
    } else if (scaler_field) {
      std::cout << slider_box->label() << std::endl;
    }
  }

  return ret;
}
