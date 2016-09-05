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

#include <FL/Fl.H>
#include <string>    /* std::string */
#include <string.h>  /* strcmp */
#include <unistd.h>  /* isatty */


#define STREQ(x, y) (strcmp(x, y) == 0)
#define STDIN (isatty(STDIN_FILENO) == 1)


/* about.cpp */
void license(void);
int about(void);

/* calendar.cpp */
#ifdef WITH_CALENDAR
int dialog_fl_calendar(       char *calendar_title,
                       std::string  fmt);
#endif

/* choice.cpp */
int dialog_fl_choice(const char *choice_msg,
                           char *choice_title,
                           char *choice_but_yes,
                           char *choice_but_no,
                           char *choice_but_alt);

/* color.cpp */
#ifdef WITH_COLOR
int dialog_fl_color(char *color_title);
#endif

/* dnd.cpp */
#ifdef WITH_DND
int dialog_dnd(char *dnd_label,
               char *dnd_title);
#endif

/* file.cpp */
#ifdef WITH_FILE
int dialog_fl_file_chooser(char *file_chooser_title);
int dialog_fl_dir_chooser(char *dir_chooser_title);
#define FILE_CHOOSER 0
#define DIR_CHOOSER 1
int dialog_fl_native_file_chooser(char *fnfc_title,
                                  int   fnfc_dir);
#endif  /* WITH_FILE */

/* html.cpp */
#ifdef WITH_HTML
int dialog_html_viewer(const char *file);
#endif

/* input.cpp */
#ifdef WITH_ENTRY
int dialog_fl_input(const char *input_msg,
                          char *input_title);
#endif

/* message.cpp */
#define MESSAGE 0
#define ALERT 1
int dialog_fl_message(const char *message_msg,
                            char *message_title,
                            int   type=MESSAGE);

/* notify.cpp */
#ifdef WITH_NOTIFY
int dialog_notify(const char *appname,
                  const char *notify_timeout,
                  const char *notify_msg,
                  const char *notify_title,
                  std::string notify_icon);
#endif

/* password.cpp */
#ifdef WITH_PASSWORD
int dialog_fl_password(const char *password_msg,
                             char *password_title);
#endif

/* progress.cpp */
#ifdef WITH_PROGRESS
int dialog_fl_progress(const char *progress_msg,
                             char *progress_title,
                             bool  autoclose,
                             bool  hide_cancel);
#endif

/* slider.cpp */
#ifdef WITH_SCALE
int dialog_fl_value_slider(const char *slider_msg,
                                 char *slider_title,
                                 char *slider_min,
                                 char *slider_max,
                                 char *slider_step,
                                 char *slider_val);
#endif

/* textinfo.cpp */
#ifdef WITH_TEXTINFO
int dialog_textinfo(       char *textinfo_title,
                           bool  autoscroll,
                    std::string  checkbox);
#endif

/* window_icon.cpp */
#ifdef WITH_WINDOW_ICON
void set_window_icon(std::string file);
#endif

/* xbm2xpm.cpp */
#ifdef WITH_WINDOW_ICON
char *xbm2xpm(const char *input);
#endif

/* translate.cpp */
std::string &repstr(      std::string &s,
                    const std::string &from,
                    const std::string &to);
std::string translate(const char *inputText);

/* version.cpp */
void print_fltk_version(void);
std::string get_fltk_version(void);

#endif  /* !FLTK_DIALOG_H */

