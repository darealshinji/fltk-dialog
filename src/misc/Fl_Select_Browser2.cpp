//
// "$Id: Fl_Check_Browser.cxx 8864 2011-07-19 04:49:30Z greg.ercolano $"
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

/**
  modified version of Fl_Check_Browser.cxx
  2016/11/18 djcj <djcj@gmx.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <FL/../src/flstring.h>
#include <FL/fl_draw.H>
#include "Fl_Select_Browser2.H"

/* This uses a cache for faster access when you're scanning the list
either forwards or backwards. */

Fl_Select_Browser2::cb_item *Fl_Select_Browser2::find_item(int n) const {
	int i = n;
	cb_item *p = first;

	if (n <= 0 || n > nitems_ || p == 0) {
		return 0;
	}

	if (n == cached_item) {
		p = cache;
		n = 1;
	} else if (n == cached_item + 1) {
		p = cache->next;
		n = 1;
	} else if (n == cached_item - 1) {
		p = cache->prev;
		n = 1;
	}

	while (--n) {
		p = p->next;
	}

	/* Cast to not const and cache it. */

	((Fl_Select_Browser2 *)this)->cache = p;
	((Fl_Select_Browser2 *)this)->cached_item = i;

	return p;
}

int Fl_Select_Browser2::lineno(cb_item *p0) const {
	cb_item *p = first;

	if (p == 0) {
		return 0;
	}

	int i = 1;
	while (p) {
		if (p == p0) {
			return i;
		}
		i++;
		p = p->next;
	}

	return 0;
}

Fl_Select_Browser2::Fl_Select_Browser2(int X, int Y, int W, int H, const char *l)
/**  The constructor makes an empty browser.*/
: Fl_Browser_(X, Y, W, H, l) {
	type(FL_SELECT_BROWSER);
	when(FL_WHEN_NEVER);
	first = last = 0;
	nitems_ = nchecked_ = 0;
	cached_item = -1;
}

void *Fl_Select_Browser2::item_first() const {
	return first;
}

void *Fl_Select_Browser2::item_next(void *l) const {
	return ((cb_item *)l)->next;
}

void *Fl_Select_Browser2::item_prev(void *l) const {
	return ((cb_item *)l)->prev;
}

int Fl_Select_Browser2::item_height(void *) const {
	return textsize() + 2;
}

#define CHECK_SIZE (textsize()-2)

int Fl_Select_Browser2::item_width(void *v) const {
	fl_font(textfont(), textsize());
	return int(fl_width(((cb_item *)v)->text)) + CHECK_SIZE + 8;
}

void Fl_Select_Browser2::item_draw(void *v, int X, int Y, int, int) const {
	cb_item *i = (cb_item *)v;
	char *s = i->text;
	int tsize = textsize();
	Fl_Color col = active_r() ? textcolor() : fl_inactive(textcolor());
	int cy = Y + (tsize + 1 - CHECK_SIZE) / 2;
	X += 2;

	fl_color(active_r() ? FL_FOREGROUND_COLOR : fl_inactive(FL_FOREGROUND_COLOR));
//	fl_loop(X, cy, X, cy + CHECK_SIZE,
//	        X + CHECK_SIZE, cy + CHECK_SIZE, X + CHECK_SIZE, cy);

	int ox = X + CHECK_SIZE/2;
	int oy = Y + CHECK_SIZE/2;
	int r = CHECK_SIZE/2 - 1;
	fl_circle(ox+2, oy+2, r);

	if (i->checked) {
	  //int tx = X + 3;
	  int tw = CHECK_SIZE - 4;
	  int d1 = tw/3;
	  int d2 = tw-d1;
	  int ty = cy + (CHECK_SIZE+d2)/2-d1-2;
	  for (int n = 0; n < 3; n++, ty++) {
	    fl_pie(ox, oy, r, r, 0, 360);
	    //fl_line(tx, ty, tx+d1, ty+d1);
	    //fl_line(tx+d1, ty+d1, tx+tw-1, ty+d1-d2+1);
	  }
	}
	fl_font(textfont(), tsize);
	if (i->selected) {
		col = fl_contrast(col, selection_color());
	}
	fl_color(col);
	fl_draw(s, X + CHECK_SIZE + 8, Y + tsize - 1);
}

void Fl_Select_Browser2::item_select(void *v, int state) {
	cb_item *i = (cb_item *)v;
	cb_item *p;

	nchecked_ = 0;
	for (p = first; p; p = p->next) {
		p->checked = 0;
	}
  i->checked = 1;
	redraw();

/*
	if (state) {
		if (i->checked) {
			i->checked = 0;
			nchecked_--;
		} else {
			i->checked = 1;
			nchecked_++;
		}
	}
 */
}

int Fl_Select_Browser2::item_selected(void *v) const {
	cb_item *i = (cb_item *)v;
	return i->selected;
}
/**
 Add a new unchecked line to the end of the browser.  
 \see add(char *s, int b)
*/
int Fl_Select_Browser2::add(char *s) {
	return (add(s, 0));
}

/** 
 Add a new line to the end of the browser.  The text is copied
 using the strdup() function.  It may also be NULL to make
 a blank line.  It can set the item checked if \p b is not 0.
 */
int Fl_Select_Browser2::add(char *s, int b) {
	cb_item *p = (cb_item *)malloc(sizeof(cb_item));
	p->next = 0;
	p->prev = 0;
	p->checked = b;
	p->selected = 0;
	p->text = strdup(s);

	if (b) {
		nchecked_++;
	}

	if (last == 0) {
		first = last = p;
	} else {
		last->next = p;
		p->prev = last;
		last = p;
	}
	nitems_++;

	return (nitems_);
}

/**
  Remove line n and make the browser one line shorter. Returns the 
  number of lines left in the browser.
*/
int Fl_Select_Browser2::remove(int item) {
  cb_item *p = find_item(item);
  
  // line at item exists
  if(p) {
    // tell the Browser_ what we will do
    deleting(p);

    // fix checked count
    if(p->checked)
      --nchecked_;
    
    // remove the node
    if (p->prev) 
      p->prev->next = p->next;
    else 
      first = p->next;
    if (p->next) 
      p->next->prev = p->prev;
    else 
      last = p->prev;
    
    free(p->text);
    free(p);
    
    --nitems_;
    cached_item = -1;
  }
  
  return (nitems_);
}

/**  Remove every item from the browser.*/
void Fl_Select_Browser2::clear() {
	cb_item *p = first;
	cb_item *next;

	if (p == 0) {
		return;
	}

	new_list();
	do {
		next = p->next;
		free(p->text);
		free(p);
		p = next;
	} while (p);

	first = last = 0;
	nitems_ = nchecked_ = 0;
	cached_item = -1;
}

/** Gets the current status of item item. */
int Fl_Select_Browser2::checked(int i) const {
	cb_item *p = find_item(i);

	if (p) return p->checked;
	return 0;
}

/** Sets the check status of item item to b. */
void Fl_Select_Browser2::checked(int i, int b) {
	cb_item *p = find_item(i);

	if (p && (p->checked ^ b)) {
		p->checked = b;
		if (b) {
			nchecked_++;
		} else {
			nchecked_--;
		}
		redraw();
	}
}

/**  Returns the index of the currently selected item.*/
int Fl_Select_Browser2::value() const {
	return lineno((cb_item *)selection());
}

/**  Return a pointer to an internal buffer holding item item's text.*/
char *Fl_Select_Browser2::text(int i) const {
	cb_item *p = find_item(i);

	if (p) return p->text;
	return 0;
}

/**  Sets all the items checked.*/
void Fl_Select_Browser2::check_all() {
	cb_item *p;

	nchecked_ = nitems_;
	for (p = first; p; p = p->next) {
		p->checked = 1;
	}
	redraw();
}

/**  Sets all the items unchecked.*/
void Fl_Select_Browser2::check_none() {
	cb_item *p;

	nchecked_ = 0;
	for (p = first; p; p = p->next) {
		p->checked = 0;
	}
	redraw();
}

int Fl_Select_Browser2::handle(int event) {
  if (event==FL_PUSH)
    deselect();
  return Fl_Browser_::handle(event);
}

//
// End of "$Id: Fl_Check_Browser.cxx 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
