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
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <string>

#include "fltk-dialog.hpp"


static Fl_Window *message_win;

static void message_dialog_cb(Fl_Widget *, long p)
{
  ret = (int) p;
  message_win->hide();
}

int dialog_message(int type)
{
  Fl_Box           *tmp, *icon, *icon_dummy, *box, *button_dummy;
  Fl_Return_Button *ret_but;
  Fl_Button        *but;
  Fl_Group         *g_left, *g_text, *g_button;

  const char *def_msg = (char *)"no message";
  const char *def_title = (char *)"FLTK message box";
  const char *iconlabel, *ret_but_label, *but_label;

  int auto_w, auto_h;
  int esc_ret_val = 1;
  int win_w = 410;
  int win_h = 150;

  if (type == MESSAGE_TYPE_WARNING)
  {
    iconlabel = (char *)"!";
    ret_but_label = label_ok;
    but_label = label_cancel;
  }
  else if (type == MESSAGE_TYPE_QUESTION)
  {
    def_title = (char *)"FLTK yes/no choice";
    def_msg = (char *)"Do you want to proceed?";
    iconlabel = (char *)"?";
    ret_but_label = label_yes;
    but_label = label_no;
  }
  else /* if (type == MESSAGE_TYPE_INFO) */
  {
    iconlabel = (char *)"i";
    ret_but_label = NULL;
    but_label = label_close;
    esc_ret_val = 0;
  }

  if (title == NULL)
  {
    title = def_title;
  }

  if (msg == NULL)
  {
    msg = def_msg;
  }
  else
  {
    std::string s = translate(msg);
    msg = s.c_str();
  }

  tmp = new Fl_Box(0, 0, 0, 0);
  tmp->label(msg);
  auto_w = auto_h = 0;
  fl_font(tmp->labelfont(), tmp->labelsize());
  fl_measure(tmp->label(), auto_w, auto_h);
  delete tmp;

  auto_w += 90;
  auto_h += 56;

  if (auto_w > max_w)
  {
    win_w = max_w;
  }
  else if (auto_w > win_w && auto_w <= max_w)
  {
    win_w = auto_w;
  }

  if (auto_h > max_h)
  {
    win_h = max_h;
  }
  else if (auto_h > win_h && auto_h <= max_h)
  {
    win_h = auto_h;
  }

  message_win = new Fl_Window(win_w, win_h, title);
  message_win->callback(message_dialog_cb, esc_ret_val);
  {
    g_left = new Fl_Group(0, 0, 50, win_h - 46);
    {
      icon = new Fl_Box(10, 10, 50, 50);
      icon->box(FL_THIN_UP_BOX);
      icon->labelfont(FL_TIMES_BOLD);
      icon->labelsize(34);
      icon->color(FL_WHITE);
      icon->labelcolor(FL_BLUE);
      icon->label(iconlabel);

      icon_dummy = new Fl_Box(10, 60, 50, win_h - 106);
      icon_dummy->box(FL_NO_BOX);
    }
    g_left->resizable(icon_dummy);
    g_left->end();

    g_text = new Fl_Group(59, 0, win_w - 59, win_h - 46);
    {
      box = new Fl_Box(70, 10, win_w - 80, win_h - 56);
      box->box(FL_NO_BOX);
      box->align(FL_ALIGN_TOP|FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP);
      box->label(msg);
    }
    g_text->resizable(box);
    g_text->end();

    g_button = new Fl_Group(0, win_h - 46, win_w, 46);
    {
      int offset = 200;
      int but_ret_val = 1;

      if (type == MESSAGE_TYPE_INFO)
      {
        offset = 100;
        but_ret_val = 0;
      }

      button_dummy = new Fl_Box(0, win_h - 46, win_w - offset, 46);
      button_dummy->box(FL_NO_BOX);

      if (type != MESSAGE_TYPE_INFO)
      {
        ret_but = new Fl_Return_Button(win_w - 200, win_h - 36, 90, 26, ret_but_label);
        ret_but->callback(message_dialog_cb, 0);
      }
      but = new Fl_Button(win_w - 100, win_h - 36, 90, 26, but_label);
      but->callback(message_dialog_cb, but_ret_val);
    }
    g_button->resizable(button_dummy);
    g_button->end();
  }
  set_size(message_win, g_text);
  set_position(message_win);
  message_win->end();
  message_win->show();

  Fl::run();
  return ret;
}

