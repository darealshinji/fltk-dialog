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
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Window.H>
#include <FL/filename.H>

#include <string>

#include "fltk.xpm"
#include "fltk-dialog.hpp"

static Fl_Window *about_win;

static void open_uri_cb(Fl_Widget *, void *p)
{
  const char *uri = (char *)p;
  char errmsg[512];
  char warnmsg[768];

  if (!fl_open_uri(uri, errmsg, sizeof(errmsg)))
  {
    sprintf(warnmsg, "Error: %s", errmsg);
    title = "Error";
    msg = warnmsg;
    dialog_message(fl_close, NULL, NULL, MESSAGE_TYPE_INFO);
  }
}

static void about_close_cb(Fl_Widget *)
{
  about_win->hide();
}

int about()
{
  Fl_Tabs          *about_tab;
  Fl_Group         *g_about, *g_license, *g_patches;
  Fl_Pixmap        *about_pixmap;
  Fl_Box           *box;
  Fl_Text_Buffer   *license_buffer, *patches_buffer;
  Fl_Text_Display  *license_display, *patches_display;
  Fl_Button        *uri_button1, *uri_button2;
  Fl_Return_Button *but_close;

  const char *uri1 = "http://www.fltk.org";
  const char *uri2 = "https://github.com/darealshinji/fltk-dialog";
  int but_w;

  std::string getver = get_fltk_version();
  std::string about_text = "\n"
    "- FLTK dialog -\n"
    "run dialog boxes from shell scripts\n"
    "\n"
    "Using FLTK version " + getver + "\n"
    "\n" //http://www.fltk.org\n"
    "\n"
    /* http://www.utf8-chartable.de/ */
    "Copyright \xc2\xa9 2016 djcj <djcj@gmx.de>\n"
    "\n" //https://github.com/darealshinji/fltk-dialog\n"
    "\n"
#ifdef WITH_FONT
    "The FLTK library and the font widget are\n"
    "copyright \xc2\xa9 1998-2016 by Bill Spitzak and others.\n"
#else
    "The FLTK library is copyright \xc2\xa9 1998-2016 by\n"
    "Bill Spitzak and others.\n"
#endif
    "\n"
    "The calendar widget is copyright \xc2\xa9 1999-2000\n"
    "by the Flek development team and\n"
    "copyright \xc2\xa9 2016 by djcj <djcj@gmx.de>\n"
    "\n"
#ifdef WITH_DEFAULT_ICON
    "The application icon is copyright \xc2\xa9 2016 by Haiku, Inc."
#else
    "\n"
#endif
    /* about_text end */;
  const char *about_text_c = about_text.c_str();

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
    /* license_buffer_text end */;

  const char *patches_buffer_text =
    /* sed 's|\\|\\\\|g; s|"|\\"|g; s|^|    "|g; s|$|\\n"|g' fl_ask.diff */
    "--- a/3rdparty/fltk/src/Fl_Choice.cxx\n"
    "+++ b/3rdparty/fltk/src/Fl_Choice.cxx\n"
    "@@ -186,13 +186,21 @@ int Fl_Choice::handle(int e) {\n"
    "   J1:\n"
    "     if (Fl::scheme()\n"
    " 	|| fl_contrast(textcolor(), FL_BACKGROUND2_COLOR) != textcolor()) {\n"
    "-      v = menu()->pulldown(x(), y(), w(), h(), mvalue(), this);\n"
    "+      // 2016/11/09  djcj <djcj@gmx.de>\n"
    "+      // Set 5th parameter to 0 to change the dropdown menu behavior\n"
    "+      // to that of Fl_Menu_Button().  Original line:\n"
    "+      // v = menu()->pulldown(x(), y(), w(), h(), mvalue(), this);\n"
    "+      v = menu()->pulldown(x(), y(), w(), h(), 0, this);\n"
    "     } else {\n"
    "       // In order to preserve the old look-n-feel of \"white\" menus,\n"
    "       // temporarily override the color() of this widget...\n"
    "       Fl_Color c = color();\n"
    "       color(FL_BACKGROUND2_COLOR);\n"
    "-      v = menu()->pulldown(x(), y(), w(), h(), mvalue(), this);\n"
    "+      // 2016/11/09  djcj <djcj@gmx.de>\n"
    "+      // Set 5th parameter to 0 to change the dropdown menu behavior\n"
    "+      // to that of Fl_Menu_Button().  Original line:\n"
    "+      // v = menu()->pulldown(x(), y(), w(), h(), mvalue(), this);\n"
    "+      v = menu()->pulldown(x(), y(), w(), h(), 0, this);\n"
    "       color(c);\n"
    "     }\n"
    "     if (!v || v->submenu()) return 1;\n"
    /* patches_buffer_text end */;

  about_pixmap = new Fl_Pixmap(fltk_xpm);

  about_win = new Fl_Window(450, 490, "About FLTK dialog");
  about_win->callback(about_close_cb);
  {
    about_tab = new Fl_Tabs(10, 10, 430, 434);
    {
      g_about = new Fl_Group(10, 40, 430, 404, "About");
      {
        box = new Fl_Box(10, 40, 430, 404, about_text_c);
        box->box(FL_NO_BOX);
        box->image(about_pixmap);

        uri_button1 = new Fl_Button(74, 210, 302, 22, uri1);
        uri_button1->box(FL_NO_BOX);
        uri_button1->down_box(FL_NO_BOX);
        uri_button1->labelcolor(FL_BLUE);
        uri_button1->clear_visible_focus();
        uri_button1->callback(open_uri_cb, (void *)uri1);

        uri_button2 = new Fl_Button(74, 260, 302, 22, uri2);
        uri_button2->box(FL_NO_BOX);
        uri_button2->down_box(FL_NO_BOX);
        uri_button2->labelcolor(FL_BLUE);
        uri_button2->clear_visible_focus();
        uri_button2->callback(open_uri_cb, (void *)uri2);
      }
      g_about->end();

      g_license = new Fl_Group(10, 40, 430, 404, "License");
      {
        license_buffer = new Fl_Text_Buffer();
        license_display = new Fl_Text_Display(10, 40, 430, 404);
        license_display->buffer(license_buffer);
        license_buffer->text(license_buffer_text);
      }
      g_license->end();

      g_patches = new Fl_Group(10, 40, 430, 404, "Patches");
      {
        patches_buffer = new Fl_Text_Buffer();
        patches_display = new Fl_Text_Display(10, 40, 430, 404);
        patches_display->textfont(FL_COURIER);
        patches_display->buffer(patches_buffer);
        patches_buffer->text(patches_buffer_text);
      }
      g_patches->end();
    }
    about_tab->end();

    but_close = new Fl_Return_Button(0,0,0,0, fl_close);
    measure_button_width(but_close, but_w, 40);
    but_close = new Fl_Return_Button(440 - but_w, 454, but_w, 28, fl_close);
    but_close->callback(about_close_cb);
  }
  set_position(about_win);
  about_win->end();
  about_win->show();

  Fl::run();
  return 0;
}

