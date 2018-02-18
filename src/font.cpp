//
// "$Id: fonts.cxx 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Font demo program for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//
// Modified 2016, 2018 by djcj <djcj@gmx.de> for fltk-dialog

/*                              FLTK License
 *                            December 11, 2001
 *
 * The FLTK library and included programs are provided under the terms
 * of the GNU Library General Public License (LGPL) with the following
 * exceptions:
 *
 *     1. Modifications to the FLTK configure script, config
 *        header file, and makefiles by themselves to support
 *        a specific platform do not constitute a modified or
 *        derivative work.
 *
 *       The authors do request that such modifications be
 *       contributed to the FLTK project - send all contributions
 *       through the "Software Trouble Report" on the following page:
 *
 *            http://www.fltk.org/str.php
 *
 *     2. Widgets that are subclassed from FLTK widgets do not
 *        constitute a derivative work.
 *
 *     3. Static linking of applications and widgets to the
 *        FLTK library does not constitute a derivative work
 *        and does not require the author to provide source
 *        code for the application or widget, use the shared
 *        FLTK libraries, or link their applications or
 *        widgets against a user-supplied version of FLTK.
 *
 *        If you link the application or widget to a modified
 *        version of FLTK, then the changes to FLTK must be
 *        provided under the terms of the LGPL in sections
 *        1, 2, and 4.
 *
 *     4. You do not have to provide a copy of the FLTK license
 *        with programs that are linked to the FLTK library, nor
 *        do you have to identify the FLTK license in your
 *        program or documentation as required by section 6
 *        of the LGPL.
 * 
 *        However, programs must still identify their use of FLTK.
 *        The following example statement can be included in user
 *        documentation to satisfy this requirement:
 *
 *            [program/widget] is based in part on the work of
 *            the FLTK project (http://www.fltk.org).
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 *  USA.
 */

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fltk-dialog.hpp"


class FontDisplay : public Fl_Widget
{
  void draw() {
    draw_box();
    fl_font((Fl_Font) font, size);
    fl_color(FL_BLACK);
    fl_draw(label(), x() + 3, y() + 3, w() - 6, h() - 6, align());
  }

public:
    int font, size;

    FontDisplay(Fl_Boxtype B, int X, int Y, int W, int H, const char *L=0)
      : Fl_Widget(X, Y, W, H, L)
    {
      box(B);
      font = 0;
      size = 14;
    }
};

static Fl_Double_Window *font_win;
static FontDisplay      *fd_text;
static Fl_Hold_Browser  *fd_fonts, *fd_size;
static int ret = 1;

static int **fd_sizes;
static int  *fd_numsizes;
static int   fd_pickedsize = 18;

static void fd_fonts_cb(Fl_Widget *)
{
  int fn = fd_fonts->value();
  if (!fn) {
    return;
  }
  fn--;
  fd_text->font = fn;
  fd_size->clear();

  int n = fd_numsizes[fn];
  int *s = fd_sizes[fn];
  if (!n) {
    /* no sizes */
  } else if (s[0] == 0) {
    /* many sizes */
    int j = 1;
    for (int i = 1; i < 64 || i < s[n - 1]; i++) {
      char buf[20];
      if (j < n && i == s[j]) {
        sprintf(buf, "@b%d", i);
        j++;
      } else {
        sprintf(buf, "%d", i);
      }
      fd_size->add(buf);
    }
    fd_size->value(fd_pickedsize);
  } else {
    /* some sizes */
    int w = 0;
    for (int i = 0; i < n; i++) {
      if (s[i] <= fd_pickedsize) {
        w = i;
      }
      char buf[20];
      sprintf(buf, "@b%d", s[i]);
      fd_size->add(buf);
    }
    fd_size->value(w + 1);
  }
  fd_text->redraw();
}

static void fd_size_cb(Fl_Widget *)
{
  int i = fd_size->value();
  if (!i) {
    return;
  }

  const char *c = fd_size->text(i);
  while (*c < '0' || *c > '9') {
    c++;
  }

  fd_pickedsize = atoi(c);
  fd_text->size = fd_pickedsize;
  fd_text->redraw();
}

static void close_cb(Fl_Widget *, long p) {
  font_win->hide();
  ret = (int) p;
}

int dialog_font()
{
  Fl_Tile          *font_tile;
  Fl_Group         *textgroup, *fontgroup, *buttongroup;
  Fl_Box           *dummy;
  Fl_Return_Button *but_ok;
  Fl_Button        *but_cancel;

  /* create the sample string */
  char label[0x1000];
  int n = 0;
  strcpy(label, "Hello, world!\n");
  int i = strlen(label);
  unsigned long c;

  for (c = ' ' + 1; c < 127; c++) {
    if (!(c &0x1f)) {
      label[i++] = '\n';
    }
    if (c == '@') {
      label[i++] = c;
    }
    label[i++] = c;
  }
  label[i++] = '\n';

  for (c = 0xA1; c < 0x600; c += 9) {
    if (!(++n &(0x1f))) {
      label[i++] = '\n';
    }
    i += fl_utf8encode((unsigned int) c, label + i);
  }
  label[i] = 0;

  /* create the window */
  if (!title) {
    title = "FLTK Font Selector";
  }

  font_win = new Fl_Double_Window(550, 400, title);
  font_win->callback(close_cb, 1);
  {
    font_tile = new Fl_Tile(0, 0, 550, 370);
    {
      textgroup = new Fl_Group(0, 0, 550, 185);
      textgroup->box(FL_FLAT_BOX);
      {
        fd_text = new FontDisplay(FL_FRAME_BOX, 10, 10, 530, 170, label);
        fd_text->align(FL_ALIGN_TOP|FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_CLIP);
        fd_text->color(17);  /* light yellow */
      }
      textgroup->resizable(fd_text);
      textgroup->end();

      fontgroup = new Fl_Group(0, 185, 550, 185);
      fontgroup->box(FL_FLAT_BOX);
      {
        fd_fonts = new Fl_Hold_Browser(10, 190, 390, 170);
        fd_fonts->box(FL_FRAME_BOX);
        fd_fonts->callback(fd_fonts_cb);
        fd_size = new Fl_Hold_Browser(410, 190, 130, 170);
        fd_size->box(FL_FRAME_BOX);
        fd_size->callback(fd_size_cb);
      }
      fontgroup->resizable(fd_fonts);
      fontgroup->end();
    }
    font_tile->end();

    buttongroup = new Fl_Group(0, 370, 550, 30);
    {
      dummy = new Fl_Box(329, 370, 1, 26);
      dummy->box(FL_NO_BOX);
      but_ok = new Fl_Return_Button(350, 370, 90, 26, fl_ok);
      but_ok->callback(close_cb, 0);
      but_cancel = new Fl_Button(450, 370, 90, 26, fl_cancel);
      but_cancel->callback(close_cb, 1);
    }
    buttongroup->resizable(dummy);
    buttongroup->end();
  }
  set_size(font_win, font_tile);
  set_position(font_win);
  font_win->end();

  int k = Fl::set_fonts("*");
  fd_sizes = new int*[k];
  fd_numsizes = new int[k];

  for (i = 0; i < k; i++) {
    int t;
    const char *name = Fl::get_font_name((Fl_Font) i, &t);
    char buffer[128];

    if (t) {
      char *p = buffer;
      if (t &FL_BOLD) {
        *p++ = '@';
        *p++ = 'b';
      }
      if (t &FL_ITALIC) {
        *p++ = '@';
        *p++ = 'i';
      }
      /* Suppress subsequent formatting - some
       * MS fonts have '@' in their name */
      *p++ = '@';
      *p++ = '.';
      strcpy(p, name);
      name = buffer;
    }
    fd_fonts->add(name);

    int *s;
    n = Fl::get_font_sizes((Fl_Font) i, s);
    fd_numsizes[i] = n;
    if (n) {
      fd_sizes[i] = new int[n];
      for (int j = 0; j < n; j++) {
        fd_sizes[i][j] = s[j];
      }
    }
  }

  fd_fonts->value(1);
  fd_fonts_cb(fd_fonts);
  run_window(font_win, NULL);

  if (ret == 0) {
    std::cout << quote << Fl::get_font_name((Fl_Font) fd_text->font, NULL)
      << quote << "|" << quote << fd_size->value() << quote << std::endl;
  }

  delete[] fd_sizes;
  delete[] fd_numsizes;
  return ret;
}
