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

#ifndef FLTK_DIALOG_HPP
#define FLTK_DIALOG_HPP

#include <FL/Fl.H>
#include <string>    /* std::string */
#include <string.h>  /* strcmp */


#define STREQ(x, y) (strcmp(x, y) == 0)
#define DEFAULT_DELIMITER '|'

#define BUTW 100  /* default button width */
#define BUTH 26   /* default button height */
#define BORD 10   /* default border size */

extern const char *title;
extern const char *msg;
extern const char *dropdown_return_number;
extern bool resizable;

/* about.cpp */
void license(void);
int about(void);

/* calendar.cpp */
#ifdef WITH_CALENDAR
int dialog_fl_calendar(std::string format);
#endif

/* checklist.cpp */
#ifdef WITH_CHECKLIST
int dialog_fl_check_button(std::string checklist_options);
#endif

/* choice.cpp */
int dialog_fl_choice(const char *choice_but_yes,
                     const char *choice_but_no,
                     const char *choice_but_alt);

/* color.cpp */
#ifdef WITH_COLOR
int dialog_fl_color(void);
#endif

/* date.cpp */
#ifdef WITH_DATE
int dialog_fdate(std::string format);
#endif

/* dnd.cpp */
#ifdef WITH_DND
int dialog_dnd(void);
#endif

/* dropdown.cpp */
#ifdef WITH_DROPDOWN
int dialog_dropdown(std::string dropdown_list);
#endif

/* file.cpp */
#ifdef WITH_FILE
int dialog_fl_file_chooser(void);
int dialog_fl_dir_chooser(void);
#define FILE_CHOOSER 0
#define DIR_CHOOSER 1
int dialog_fl_native_file_chooser(int fnfc_dir);
#endif  /* WITH_FILE */

/* font.cpp */
#ifdef WITH_FONT
int dialog_font(void);
#endif

/* html.cpp */
#ifdef WITH_HTML
int dialog_html_viewer(const char *file);
#endif

/* input.cpp */
#ifdef WITH_ENTRY
int dialog_fl_input(void);
#endif

/* message.cpp */
#define MESSAGE 0
#define ALERT 1
int dialog_fl_message(int type=MESSAGE);

/* notify.cpp */
#ifdef WITH_NOTIFY
int dialog_notify(const char  *appname,
                  const char  *notify_timeout,
                  std::string  notify_icon);
#endif

/* password.cpp */
#ifdef WITH_PASSWORD
int dialog_fl_password(void);
#endif

/* progress.cpp */
#ifdef WITH_PROGRESS
int dialog_fl_progress(bool autoclose,
                       bool hide_cancel);
#endif

/* slider.cpp */
#ifdef WITH_SCALE
int dialog_fl_value_slider(char *slider_min,
                           char *slider_max,
                           char *slider_step,
                           char *slider_val);
#endif

/* textinfo.cpp */
#ifdef WITH_TEXTINFO
int dialog_textinfo(       bool autoscroll,
                    std::string checkbox);
#endif

/* radiolist.cpp */
#ifdef WITH_RADIOLIST
int dialog_fl_radio_round_button(std::string radiolist_options,
                                        bool return_number);
#endif

/* window_icon.cpp */
#ifdef WITH_WINDOW_ICON
void set_window_icon(std::string file);
#endif

/* misc/print_date.cpp */
void print_date(std::string format,
                        int y,
                        int m,
                        int d);

/* misc/translate.cpp */
void repstr(const std::string &from,
            const std::string &to,
                  std::string &s);
std::string translate(const char *inputText);

/* version.cpp */
void print_fltk_version(void);
std::string get_fltk_version(void);

#endif  /* !FLTK_DIALOG_HPP */

