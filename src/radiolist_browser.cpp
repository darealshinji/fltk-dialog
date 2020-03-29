//
// Fl_Check_Browser header file for the Fast Light Tool Kit (FLTK).
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
// Modified by djcj <djcj@gmx.de>

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

#include "radiolist_browser.hpp"

/* modified/simplified version of Fl_Check_Browser::item_draw() */

void radiolist_browser::item_draw(void *v, int X, int Y, int, int) const
{
  cb_item *i = reinterpret_cast<cb_item *>(v);
  int tsize = textsize();
  int check_size = (tsize - 2) / 2;
  Fl_Color col = active_r() ? textcolor() : fl_inactive(textcolor());
  X += 2;

  fl_color(active_r() ? FL_FOREGROUND_COLOR : fl_inactive(FL_FOREGROUND_COLOR));

  int ox = X + check_size;
  int oy = Y + check_size;
  int r = check_size - 1;
  fl_circle(ox+2, oy+2, r);

  if (i->checked) {
    fl_pie(ox, oy, r, r, 0, 360);
  }
  fl_font(textfont(), tsize);
  if (i->selected) {
    col = fl_contrast(col, selection_color());
  }
  fl_color(col);
  fl_draw(i->text, X + tsize + 6, Y + tsize - 1);
}


