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

#include <string>

#include "fltk.xpm"
#include "fltk-dialog.hpp"

static Fl_Window *about_win;

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
  Fl_Text_Buffer   *about_buffer, *license_buffer, *patches_buffer;
  Fl_Text_Display  *about_display, *license_display, *patches_display;
  Fl_Return_Button *but_close;

  std::string getver = get_fltk_version();
  std::string about_text = //"\n"
    //"- FLTK dialog -\n"
    //"run dialog boxes from shell scripts\n"
    //"\n"
    "Using FLTK version " + getver + "\n"
    "http://www.fltk.org\n"
    "\n"
    /* http://www.utf8-chartable.de/ */
    "Copyright \xc2\xa9 2016 djcj <djcj@gmx.de>\n"
    "https://github.com/darealshinji/fltk-dialog\n"
    "\n"
#ifdef WITH_FONT
    "The FLTK library and the font widget are\n"
    "copyright \xc2\xa9 1998-2016 by Bill Spitzak and others.\n"
#else
    "The FLTK library is copyright \xc2\xa9 1998-2016 by\n"
    "Bill Spitzak and others.\n"
#endif
    "\n"
    "The calendar widget is copyright \xc2\xa9 1999-2000 by\n"
    "the Flek development team and copyright \xc2\xa9 2016\n"
    "by djcj <djcj@gmx.de>\n"
#ifdef WITH_DEFAULT_ICON
    "\nThe application icon is copyright \xc2\xa9 2016 by Haiku, Inc."
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
    "--- a/FL/fl_ask.H\n"
    "+++ b/FL/fl_ask.H\n"
    "@@ -69,6 +69,8 @@\n"
    " FL_EXPORT void fl_message_title(const char *title);\n"
    " FL_EXPORT void fl_message_title_default(const char *title);\n"
    " \n"
    "+FL_EXPORT void fl_message_size(int w, int h);\n"
    "+\n"
    " // pointers you can use to change FLTK to a foreign language:\n"
    " extern FL_EXPORT const char* fl_no;\n"
    " extern FL_EXPORT const char* fl_yes;\n"
    "--- a/src/fl_ask.cxx\n"
    "+++ b/src/fl_ask.cxx\n"
    "@@ -58,6 +58,9 @@\n"
    " extern \"C\" void NSBeep(void);\n"
    " #endif\n"
    " \n"
    "+static int override_w = 0;\n"
    "+static int override_h = 0;\n"
    "+\n"
    " static char avoidRecursion = 0;\n"
    " \n"
    " // Sets the global return value (ret_val) and closes the window.\n"
    "@@ -146,6 +149,9 @@\n"
    "   if (message_h < 30)\n"
    "     message_h = 30;\n"
    " \n"
    "+  if (override_w > 60) message_w = override_w;\n"
    "+  if (override_h > 30) message_h = override_h;\n"
    "+\n"
    "   fl_font(button[0]->labelfont(), button[0]->labelsize());\n"
    " \n"
    "   memset(button_w, 0, sizeof(button_w));\n"
    "@@ -588,6 +594,8 @@\n"
    "   message_form->copy_label(title);\n"
    " }\n"
    " \n"
    "+void fl_message_size(int w, int h) { override_w = w; override_h = h; }\n"
    "+\n"
    " /** Sets the default title of the dialog window used in many common dialogs.\n"
    " \n"
    "     This window \\p title will be used in all subsequent calls of one of the\n"
    /* patches_buffer_text end */;

  about_pixmap = new Fl_Pixmap(fltk_xpm);

  about_win = new Fl_Window(450, 490, "About FLTK dialog");
  about_win->callback(about_close_cb);
  {
    about_tab = new Fl_Tabs(10, 10, 430, 434);
    {
      g_about = new Fl_Group(10, 40, 430, 404, "About");
      {
        box = new Fl_Box(10, 40, 430, 150, "\n"
                         "- FLTK dialog -\n"
                         "run dialog boxes from shell scripts");
        box->box(FL_NO_BOX);
        box->image(about_pixmap);

        about_buffer = new Fl_Text_Buffer();
        about_display = new Fl_Text_Display(20, 195, 410, 230);
        about_display->box(FL_NO_BOX);
        about_display->color(FL_BACKGROUND_COLOR);
        //about_display->align(FL_ALIGN_CENTER);  /* no effect */
        about_display->buffer(about_buffer);
        about_buffer->text(about_text_c);
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

    but_close = new Fl_Return_Button(350, 454, 90, 26, fl_close);
    but_close->callback(about_close_cb);
  }
  set_position(about_win);
  about_win->end();
  about_win->show();

  Fl::run();
  return 0;
}

