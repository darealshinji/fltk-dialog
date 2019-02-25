/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018, djcj <djcj@gmx.de>
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

#include <iostream>
#include <string>
#include <dlfcn.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "icon_png.h"

class My_GTK_File_Chooser
{
  Fl_GTK_File_Chooser *_fc;
public:
  My_GTK_File_Chooser(int type, const char *label);
  ~My_GTK_File_Chooser();
  int show() { return _fc ? _fc->show() : -1; }
  const char *filename() { return _fc ? _fc->filename() : NULL; }
};

My_GTK_File_Chooser::My_GTK_File_Chooser(int type, const char *label)
{
  _fc = NULL;

  if (Fl_GTK_File_Chooser::did_find_GTK_libs == 0) {
    Fl_GTK_File_Chooser::probe_for_GTK_libs();
  }

  if (Fl_GTK_File_Chooser::did_find_GTK_libs != 0) {
    _fc = new Fl_GTK_File_Chooser(type);
    _fc->title(label);
  }
}

My_GTK_File_Chooser::~My_GTK_File_Chooser()
{
  if (_fc) {
    delete _fc;
  }
}

static int file_chooser_fltk(int mode)
{
  char *file = file_chooser(mode);

  if (file) {
    std::cout << quote << file << quote << std::endl;
    free(file);
    return 0;
  }
  return 1;
}

static int native_file_chooser_gtk(int mode)
{
  My_GTK_File_Chooser *fc;

  int type = (mode == DIR_CHOOSER) ? Fl_Native_File_Chooser::BROWSE_DIRECTORY
    : Fl_Native_File_Chooser::BROWSE_FILE;

  fc = new My_GTK_File_Chooser(type, title);

  if (fc->show() == 0) {
    std::cout << quote << fc->filename() << quote << std::endl;
    return 0;
  }
  return 1;
}

#ifdef HAVE_QT
static int dlopen_getfilenameqt(int mode)
{
  std::string plugin;
  void *handle = NULL, *p;

  PROTO(int, getfilenameqt, (int, const char*, const char*, bool, bool))
  p = dlopen_qtplugin(plugin, handle, "getfilenameqt");

  if (!handle) {
    return -1;
  }

  getfilenameqt = reinterpret_cast<getfilenameqt_t>(p);
  int rv = getfilenameqt(mode, quote, title, false, false);
  dlclose(handle);
  DELETE(plugin.c_str());

  return rv;
}
#endif  /* HAVE_QT */

static int native_file_chooser(int mode)
{
  int rv = -1;

#ifdef HAVE_QT
  if (getenv("KDE_FULL_SESSION") && (rv = dlopen_getfilenameqt(mode)) == -1) {
    std::cerr << "warning: falling back to gtk" << std::endl;
  }
#endif  /* HAVE_QT */

  if (rv == -1) {
    rv = native_file_chooser_gtk(mode);
  }

  if (rv == -1) {
    std::cerr << "warning: falling back to fltk" << std::endl;
    rv = file_chooser_fltk(mode);
  }

  return rv;
}

#ifdef HAVE_QT
/* the Qt equivalent to Fl_Native_File_Chooser() */
static int native_file_chooser_qt(int mode)
{
  int rv = dlopen_getfilenameqt(mode);

  if (rv == -1) {
    std::cerr << "warning: falling back to fltk" << std::endl;
    rv = file_chooser_fltk(mode);
  }
  return rv;
}
#endif  /* HAVE_QT */

int dialog_file_chooser(int mode, int native)
{
  if (!title) {
    title = (mode == DIR_CHOOSER) ? "Select a directory" : "Select a file";
  }

  /* Note: setting an icon doesn't work on the Qt file chooser */

  switch (native) {
    case NATIVE_ANY:
      return native_file_chooser(mode);
    case NATIVE_GTK:
      return native_file_chooser_gtk(mode);
#ifdef HAVE_QT
    case NATIVE_QT:
      return native_file_chooser_qt(mode);
#endif  /* HAVE_QT */
  }
  return file_chooser_fltk(mode);
}

