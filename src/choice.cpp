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
#include <FL/fl_ask.H>

#include <string>  /* std::string, c_str */

#include "fltk-dialog.h"


int dialog_fl_choice(const char *choice_msg,
                           char *choice_title,
                           char *choice_but_yes,
                           char *choice_but_no)
{
  std::string s;

  if (choice_msg == NULL) {
    s = "Do you want to proceed?";
  } else {
    s = translate(choice_msg);
  }

  if (choice_title == NULL) {
    choice_title = (char *)"FLTK yes/no choice";
  }

  if (choice_but_yes == NULL) {
    choice_but_yes = (char *)"Yes";
  }

  if (choice_but_no == NULL) {
    choice_but_no = (char *)"No";
  }

  fl_message_title(choice_title);
  return fl_choice("%s", choice_but_yes, choice_but_no, NULL, s.c_str());
}

