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

#ifndef FLTK_DIALOG_H
#define FLTK_DIALOG_H

/* choice.cpp */
int dialog_fl_choice(char *choice_msg, char *choice_title,
                     char *choice_but_yes, char *choice_but_no);

/* color.cpp */
int dialog_fl_color(char *color_title, int html=0);

/* file.cpp */
int dialog_fl_file_chooser(char *file_chooser_title);
int dialog_fl_dir_chooser(char *dir_chooser_title);
int dialog_fl_native_file_chooser(char *fnfc_title, int fnfc_dir=0);

/* input.cpp */
int dialog_fl_input(char *input_msg, char *input_title);

/* message.cpp */
int dialog_fl_message(char *message_msg, char *message_title, int alert=0);

/* password.cpp */
int dialog_fl_password(char *password_msg, char *password_title);

/* progress.cpp */
int dialog_fl_progress(char *progress_msg, char *progress_title, int autoclose=0);

/* slider.cpp */
int dialog_fl_value_slider(char *slider_msg, char *slider_title,
                           char *slider_min, char *slider_max,
                           char *slider_step, char *slider_val);

/* translate.cpp */
char *translate(char *inputText);

/* version.cpp */
void print_fltk_version(void);

#endif  /* FLTK_DIALOG_H */

