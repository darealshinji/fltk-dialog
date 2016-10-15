#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Valuator.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Window.H>

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "fltk-dialog.hpp"

static Fl_Window *message_win;

static void message_close_cb(Fl_Widget *, long p)
{
  message_win->hide();
  ret = (int) p;
}

static void message_scale_cb(Fl_Widget *o)
{
  scale_value = ((Fl_Valuator *)o)->value();
}

void measure_button_width(Fl_Widget *o, int &w, int off)
{
  int h;
  w = h = 0;
  fl_font(o->labelfont(), o->labelsize());
  fl_measure(o->label(), w, h);
  w += off;
  if (w < 90)
  {
    w = 90;
  }
  delete o;
}

int dialog_message(
  const char *label_but_ret,
  const char *label_but,
  const char *label_but_alt,
  int type,
  bool with_icon_box)
{
  Fl_Group         *g_icon, *g_box, *g_middle, *g_buttons;
  Fl_Box           *tmp, *icon, *box, *dummy;
  Fl_Input         *input = NULL;
  Fl_Value_Slider  *scale = NULL;
  Fl_Return_Button *but_ret;
  Fl_Button        *but, *but_alt;

  std::string s;
  int msg_w, msg_h, win_w, win_h, min_w, min_h, input_off;
  int label_but_ret_w, label_but_w, label_but_alt_w;
  int esc_ret = 1;
  bool input_field = false;
  bool scaler_field = false;
  const char *label_icon = "!";


  /* labels and message type settings */

  if (title == NULL)
  {
    title = "Window Title";
  }

  if (msg == NULL)
  {
    msg = "No message";
  }
  else
  {
    s = translate(msg);
    msg = s.c_str();
  }

  if (type == MESSAGE_TYPE_INFO)
  {
    label_but_ret = fl_close;
    label_but_alt = NULL;
    label_icon = "i";
    esc_ret = 0;
  }
  else if (type == MESSAGE_TYPE_QUESTION)
  {
    label_but_ret = fl_yes;
    label_but = fl_no;
    label_icon = "?";
  }
  else if (type == MESSAGE_TYPE_INPUT || type == MESSAGE_TYPE_PASSWORD)
  {
    input_field = true;
    with_icon_box = false;
  }
  else if (type == MESSAGE_TYPE_SCALE)
  {
    scaler_field = true;
    with_icon_box = false;
  }

  if (label_but_ret == NULL)
  {
    label_but_ret = fl_ok;
  }

  if (label_but == NULL)
  {
    label_but = fl_cancel;
  }


  /* window measures */

  tmp = new Fl_Box(0,0,0,0, msg);
  fl_font(tmp->labelfont(), tmp->labelsize());
  msg_w = msg_h = 0;
  fl_measure(tmp->label(), msg_w, msg_h);
  delete tmp;

  but_ret = new Fl_Return_Button(0,0,0,0, label_but_ret);
  measure_button_width(but_ret, label_but_ret_w, 40);

  but = new Fl_Button(0,0,0,0, label_but);
  measure_button_width(but, label_but_w, 15);

  if (label_but_alt != NULL)
  {
    but_alt = new Fl_Button(0,0,0,0, label_but_alt);
    measure_button_width(but_alt, label_but_alt_w, 15);
  }

  win_w = msg_w + 30;
  win_h = msg_h + 58;
  min_h = 74;

  if (with_icon_box)
  {
    win_w += 50;
    win_h += 40;
    min_h += 40;
  }

  if (input_field || scaler_field)
  {
    input_off = 40;
    win_h += input_off;
    min_h += input_off;
  }
  else
  {
    input_off = 0;
  }

  if (label_but_alt == NULL)
  {
    if (type == MESSAGE_TYPE_INFO)
    {
      min_w = label_but_ret_w + 20;
    }
    else
    {
      min_w = label_but_ret_w + label_but_w + 30;
    }
  }
  else
  {
    min_w = label_but_ret_w + label_but_w + label_but_alt_w + 40;
  }

  if (with_icon_box)
  {
    min_w += 60;
  }

  if (win_w < min_w)
  {
    win_w = min_w;
  }
  if (win_h < min_h)
  {
    win_h = min_h;
  }


  /* initialize window */

  message_win = new Fl_Window(win_w, win_h, title);
  message_win->size_range(win_w, win_h, max_w, max_h);
  message_win->callback(message_close_cb, esc_ret);
  {
    /* icon box */
    if (with_icon_box)
    {
      g_icon = new Fl_Group(/*x*/ 0,
                            /*y*/ 0,
                            /*w*/ 70,
                            /*h*/ win_h - input_off - 58);
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
    }

    /* message area */
    g_box = new Fl_Group(/*x*/ 0,
                         /*y*/ 0,
                         /*w*/ win_w - 60,
                         /*h*/ win_h - input_off - 58);
    {
      int box_x, box_w;
      if (with_icon_box)
      {
        box_x = 69;
        box_w = win_w - 69;
      }
      else
      {
        box_x = 10;
        box_w = win_w - 20;
      }
      box = new Fl_Box(/*x*/ box_x,
                       /*y*/ 10,
                       /*w*/ box_w,
                       /*h*/ win_h - input_off - 58,
                       /*l*/ msg);
      box->align(FL_ALIGN_INSIDE|FL_ALIGN_TOP_LEFT|FL_ALIGN_WRAP);
      box->box(FL_NO_BOX);
    }
    g_box->resizable(box);
    g_box->end();

    /* extra group for input field or scaler */
    if (input_field)
    {
      g_middle = new Fl_Group(/*x*/ 0,
                              /*y*/ win_h - input_off - 38,
                              /*w*/ win_w,
                              /*h*/ 30);
      {
        input = new Fl_Input(/*x*/ 10,
                             /*y*/ win_h - input_off - 38,
                             /*w*/ win_w - 20,
                             /*h*/ 30);
        if (type == MESSAGE_TYPE_PASSWORD)
        {
          input->type(FL_SECRET_INPUT);
        }
      }
      g_middle->resizable(input);
      g_middle->end();
    }
    else if (scaler_field)
    {
      g_middle = new Fl_Group(/*x*/ 0,
                              /*y*/ win_h - input_off - 38,
                              /*w*/ win_w,
                              /*h*/ 30);
      {
        scale = new Fl_Value_Slider(/*x*/ 10,
                                    /*y*/ win_h - input_off - 38,
                                    /*w*/ win_w - 20,
                                    /*h*/ 30,
                                    /*l*/ NULL);
        scale->type(FL_HOR_NICE_SLIDER);
        scale->box(FL_FLAT_BOX);
        scale->minimum(scale_min);
        scale->maximum(scale_max);
        scale->step(scale_step);
        scale_value = scale->round(scale_init);
        scale->value(scale_value);
        scale->callback(message_scale_cb);
      }
      g_middle->resizable(scale);
      g_middle->end();
    }

    /* buttons */
    g_buttons = new Fl_Group(/*x*/ 0,
                             /*y*/ win_h - 58,
                             /*w*/ win_w,
                             /*h*/ 58);
    {
      int offset_alt, offset;

      if (label_but_alt == NULL)
      {
        offset_alt = 0;
      }
      else
      {
        offset_alt = label_but_alt_w + 10;
      }

      if (type == MESSAGE_TYPE_INFO)
      {
        offset = label_but_ret_w + 10 + offset_alt;
      }
      else
      {
        offset = label_but_ret_w + label_but_w + 20 + offset_alt;
      }

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
      but_ret->callback(message_close_cb, 0);

      if (type != MESSAGE_TYPE_INFO)
      {
        but = new Fl_Button(/*x*/ win_w - label_but_w - 10 - offset_alt,
                            /*y*/ win_h - 38,
                            /*w*/ label_but_w,
                            /*h*/ 28,
                            /*l*/ label_but);
        but->callback(message_close_cb, 1);
      }

      if (label_but_alt != NULL)
      {
        but_alt = new Fl_Button(/*x*/ win_w - label_but_alt_w - 10,
                                /*y*/ win_h - 38,
                                /*w*/ label_but_alt_w,
                                /*h*/ 28,
                                /*l*/ label_but_alt);
        but_alt->callback(message_close_cb, 2);
      }
    }
    g_buttons->resizable(dummy);
    g_buttons->end();
  }
  set_size(message_win, g_box);
  set_position(message_win);
  message_win->end();
  message_win->show();

  Fl::run();

  if (ret == 0)
  {
    if (input_field)
    {
      std::cout << input->value() << std::endl;
    }
    else if (scaler_field)
    {
      std::cout << scale_value << std::endl;
    }
  }
  return ret;
}
