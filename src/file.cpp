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

#ifdef HAVE_QT
# include <fstream>
# include <dlfcn.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# ifndef USE_SYSTEM_PLUGINS
#  include "qtgui_so.h"
# endif  /* !USE_SYSTEM_PLUGINS */
#endif  /* HAVE_QT */

#include "fltk-dialog.hpp"

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
  Fl_GTK_File_Chooser *fc;
  int type = Fl_Native_File_Chooser::BROWSE_FILE;

  if (mode == DIR_CHOOSER) {
    type = Fl_Native_File_Chooser::BROWSE_DIRECTORY;
  }

  if (Fl_GTK_File_Chooser::did_find_GTK_libs == 0) {
    Fl_GTK_File_Chooser::probe_for_GTK_libs();
  }

  if (Fl_GTK_File_Chooser::did_find_GTK_libs == 0) {
    return -1;
  }

  fc = new Fl_GTK_File_Chooser(type);
  fc->title(title);

  if (fc->show() == 0) {
    std::cout << quote << fc->filename() << quote << std::endl;
    return 0;
  }
  return 1;
}

#ifdef HAVE_QT
static int dlopen_getfilenameqt(int qt_major, int mode)
{
  std::string plugin;

#ifdef USE_SYSTEM_PLUGINS
# define DELETE(x)
  plugin = FLTK_DIALOG_MODULE_PATH "/qt5gui.so";

  if (qt_major == 4) {
    plugin[plugin.size() - 7] = '4';
  }
#else
# define DELETE(x)  unlink(x);
  const unsigned char *data = (qt_major == 4) ? qt4gui_so : qt5gui_so;
  const unsigned int data_len = (qt_major == 4) ? qt4gui_so_len : qt5gui_so_len;

  if (!save_to_temp(data, data_len, plugin)) {
    return -1;
  }
#endif  /* USE_SYSTEM_PLUGINS */

  /* dlopen() library */

  void *handle = dlopen(plugin.c_str(), RTLD_LAZY);
  char *error = dlerror();

  if (!handle) {
    std::cerr << error << std::endl;
    DELETE(plugin.c_str());
    return -1;
  }

  dlerror();

  GETPROCADDRESS(handle, int, getfilenameqt, (int, /* char, */ const char*, const char*))

  error = dlerror();

  if (error) {
    std::cerr << "error: " << error << std::endl;
    dlclose(handle);
    DELETE(plugin.c_str());
    return -1;
  }

  int rv = getfilenameqt(mode, quote, title);
  dlclose(handle);
  DELETE(plugin.c_str());

  return rv;
}
#endif  /* HAVE_QT */

/* Save the attached Qt5 GUI module to disk and try to dlopen() it.
 * If it fails (i.e. because Qt5 libraries are missing) try the same with the Qt4 module.
 * If that fails too, fall back to Fl_Native_File_Chooser();
 */
static int native_file_chooser(int mode)
{
  int rv = -1;

#ifdef HAVE_QT
  if (getenv("KDE_FULL_SESSION")) {
    rv = dlopen_getfilenameqt(QTDEF, mode);

# if (QTDEF == 5) && defined(HAVE_QT4)
    if (rv == -1) {
      std::cerr << "warning: falling back to Qt4" << std::endl;
      rv = dlopen_getfilenameqt(4, mode);
    }
# endif

    if (rv == -1) {
      std::cerr << "warning: falling back to gtk" << std::endl;
    }
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
static int native_file_chooser_qt(int qt_major, int mode)
{
  int rv = dlopen_getfilenameqt(qt_major, mode);

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

  switch (native) {
    case NATIVE_ANY:
      return native_file_chooser(mode);
    case NATIVE_GTK:
      return native_file_chooser_gtk(mode);
#ifdef HAVE_QT
#  ifdef HAVE_QT4
    case NATIVE_QT4:
      return native_file_chooser_qt(4, mode);
#  endif
#  ifdef HAVE_QT5
    case NATIVE_QT5:
      return native_file_chooser_qt(5, mode);
#  endif
#endif  /* HAVE_QT */
  }
  return file_chooser_fltk(mode);
}

