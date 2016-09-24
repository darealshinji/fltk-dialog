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
#include <FL/fl_ask.H>  /* fl_close */
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Text_Display.H>

#include <string>    /* std::string, c_str */
#include <stdlib.h>  /* exit */

#include "fltk.xpm"
#include "fltk-dialog.hpp"


Fl_Pixmap       *about_pixmap;
Fl_Text_Buffer  *license_buffer;
Fl_Text_Display *license_display;

static void about_but_license_cb(Fl_Widget*)
{
  license();
}

static void about_hide_cb(Fl_Widget *w)
{
  w->window()->hide();
}

static void about_close_cb(Fl_Widget*)
{
  delete about_pixmap;
  delete license_display;
  delete license_buffer;
  exit(0);
}

int about()
{
  Fl_Window        *win;
  Fl_Box           *box;
  Fl_Button        *but_license;
  Fl_Return_Button *but_close;

  int winw = 450;
  int winh = 460;
  int bord = 10;
  int butw = 100;
  int buth = 26;

  std::string getver = get_fltk_version();
  std::string about_text = "\n"
    "- FLTK dialog -\n"
    "run dialog boxes from shell scripts\n"
    "\n"
    "Using FLTK version " + getver + "\n"
    "http://www.fltk.org\n"
    "\n"
    /* http://www.utf8-chartable.de/ */
    "Copyright \xc2\xa9 2016 djcj <djcj@gmx.de>\n"
    "https://github.com/darealshinji/fltk-dialog\n"
    "\n"
    "The FLTK library is copyright \xc2\xa9 1998-2016 by\n"
    "Bill Spitzak and others.\n"
    "\n"
    "The calendar widget is copyright \xc2\xa9 1999-2000 by\n"
    "the Flek development team and\n"
    "copyright \xc2\xa9 2016 by djcj <djcj@gmx.de>\n"
#ifdef WITH_DEFAULT_ICON
    "\nThe application icon is copyright \xc2\xa9 2016 by Haiku, Inc."
#endif
    /**/;
  const char *about_text_c = about_text.c_str();
  about_pixmap = new Fl_Pixmap(fltk_xpm);

  win = new Fl_Window(winw, winh, "About FLTK dialog");
  win->callback(about_close_cb);  /* exit(0) */
  {
    box = new Fl_Box(bord, bord, winw-bord*2, winh-buth-bord*3, about_text_c);
    box->box(FL_UP_BOX);
    box->image(about_pixmap);

    but_license = new Fl_Button(bord, winh-buth-bord, butw, buth, "Licenses");
    but_license->callback(about_but_license_cb);

    but_close = new Fl_Return_Button(winw-butw-bord, winh-buth-bord, butw, buth, fl_close);
    but_close->callback(about_close_cb);
  }
  win->end();
  win->show();

  int ret = Fl::run();
  delete about_pixmap;
  return ret;
}

const char *license_buffer_text =
    /* sed 's|"|\\"|g; s|^|    "|g; s|$|\\n"|g' LICENSE */
    "The MIT License (MIT)\n"
    "\n"
    "Copyright (c) 2016  djcj <djcj@gmx.de>\n"
    "\n"
    "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
    "of this software and associated documentation files (the \"Software\"), to deal\n"
    "in the Software without restriction, including without limitation the rights\n"
    "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
    "copies of the Software, and to permit persons to whom the Software is\n"
    "furnished to do so, subject to the following conditions:\n"
    "\n"
    "The above copyright notice and this permission notice shall be included in all\n"
    "copies or substantial portions of the Software.\n"
    "\n"
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
    "SOFTWARE.\n"
    "\n"
    "\n"
    "\n"
    "Copyright (c) 1998-2016  Bill Spitzak and others.\n"
    "\n"
    "The FLTK library and included programs are provided under the terms of the\n"
    "GNU Library General Public License (LGPL) with the following exceptions:\n"
    "\n"
    "1. Modifications to the FLTK configure script, config header file, and\n"
    "makefiles by themselves to support a specific platform do not constitute a\n"
    "modified or derivative work.\n"
    "The authors do request that such modifications be contributed to the FLTK\n"
    "project - send all contributions to \"fltk-bugs@fltk.org\".\n"
    "\n"
    "2. Widgets that are subclassed from FLTK widgets do not constitute a\n"
    "derivative work.\n"
    "\n"
    "3. Static linking of applications and widgets to the FLTK library does not\n"
    "constitute a derivative work and does not require the author to provide\n"
    "source code for the application or widget, use the shared FLTK libraries, or\n"
    "link their applications or widgets against a user-supplied version of FLTK.\n"
    "If you link the application or widget to a modified version of FLTK, then\n"
    "the changes to FLTK must be provided under the terms of the LGPL in sections\n"
    "1, 2, and 4.\n"
    "\n"
    "4. You do not have to provide a copy of the FLTK license with programs that\n"
    "are linked to the FLTK library, nor do you have to identify the FLTK license\n"
    "in your program or documentation as required by section 6 of the LGPL.\n"
    "However, programs must still identify their use of FLTK. The following example\n"
    "statement can be included in user documentation to satisfy this requirement:\n"
    "[program/widget] is based in part on the work of the FLTK project (http://www.fltk.org).\n"
    "\n"
    "    This library is free software; you can redistribute it and/or\n"
    "    modify it under the terms of the GNU Library General Public\n"
    "    License as published by the Free Software Foundation; either\n"
    "    version 2 of the License, or (at your option) any later version.\n"
    "\n"
    "    This library is distributed in the hope that it will be useful,\n"
    "    but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
    "    Library General Public License for more details.\n"
    "\n"
    "    You should have received a copy of the GNU Library General Public License\n"
    "    along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
    "\n"
    "\n"
    "\n"
    "Copyright (c) 1999-2000  the Flek development team\n"
    "Copyright (c) 2016  djcj <djcj@gmx.de>\n"
    "\n"
    "This library is free software; you can redistribute it and/or\n"
    "modify it under the terms of the GNU Library General Public\n"
    "License as published by the Free Software Foundation; either\n"
    "version 2 of the License, or (at your option) any later version.\n"
    "\n"
    "This library is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
    "Library General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU Library General Public\n"
    "License along with this library; if not, write to the Free Software\n"
    "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307\n"
    "USA.\n"
#ifdef WITH_DEFAULT_ICON
    "\n"
    "\n"
    "\n"
    "The MIT License (MIT)\n"
    "\n"
    "Copyright (c) 2016  Haiku, Inc.\n"
    "\n"
    "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
    "of this software and associated documentation files (the \"Software\"), to deal\n"
    "in the Software without restriction, including without limitation the rights\n"
    "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
    "copies of the Software, and to permit persons to whom the Software is\n"
    "furnished to do so, subject to the following conditions:\n"
    "\n"
    "The above copyright notice and this permission notice shall be included in all\n"
    "copies or substantial portions of the Software.\n"
    "\n"
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
    "SOFTWARE.\n"
#endif  /* WITH_ICON */
    /**/;

void license()
{
  Fl_Window *win;
  Fl_Button *but_close;

  int winw = 600;
  int winh = 540;
  int bord = 10;
  int butw = 100;
  int buth = 26;

  win = new Fl_Window(winw, winh, "Terms and Conditions");
  {
    license_buffer = new Fl_Text_Buffer();
    license_display = new Fl_Text_Display(bord, bord, winw-bord*2, winh-buth-bord*3);
    license_display->buffer(license_buffer);
    license_buffer->text(license_buffer_text);
    but_close = new Fl_Button((winw-butw)/2, winh-buth-bord, butw, buth, fl_close);
    but_close->callback(about_hide_cb);
  }
  win->end();
  win->show();

  Fl::run();
  delete license_display;
  delete license_buffer;
}

