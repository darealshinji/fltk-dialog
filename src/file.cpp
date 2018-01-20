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

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>

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

static int dialog_file_chooser_fltk(int mode)
{
  Fl_File_Chooser *fc;
  int flags = Fl_File_Chooser::CREATE;

  if (!title) {
    title = (mode == DIR_CHOOSER) ? "Select a directory" : "Select a file";
  }

  if (mode == DIR_CHOOSER) {
    flags |= Fl_File_Chooser::DIRECTORY;
  }

  fc = new Fl_File_Chooser(".", "*", flags, title);

  /* update buttons with correct size and position */
  fc->ok_label(fl_ok);
  fc->cancel_label(fl_cancel);

  if (!window_taskbar) {
    fc->border(0);
  }

  fc->show();

  int new_x = fc->x();
  int new_y = fc->y();
  int new_w = fc->w();
  int new_h = fc->h();

  if (resizable) {
    if (override_w > 0) {
      new_w = override_w;
    }
    if (override_h > 0) {
      new_h = override_h;
    }
  }

  if (position_center) {
    new_x = (max_w - new_w) / 2;
    new_y = (max_h - new_h) / 2;
  } else {
    if (override_x >= 0) {
      new_x = override_x;
    }
    if (override_y >= 0) {
      new_y = override_y;
    }
  }

  fc->resize(new_x, new_y, new_w, new_h);

  if (window_decoration) {
    fc->border(1);
  } else {
    fc->border(0);
  }

  if (always_on_top) {
    fc->always_on_top();
  }

  Fl::run();

  if (fc->value()) {
    std::cout << fc->value() << std::endl;
    return 0;
  }
  return 1;
}

static int dialog_native_file_chooser_gtk(int mode)
{
  Fl_Native_File_Chooser *fc;
  int type = Fl_Native_File_Chooser::BROWSE_FILE;

  if (!title) {
    title = (mode == DIR_CHOOSER) ? "Select a directory" : "Select a file";
  }

  if (mode == DIR_CHOOSER) {
    type = Fl_Native_File_Chooser::BROWSE_DIRECTORY;
  }

  fc = new Fl_Native_File_Chooser(type);
  fc->title();

  if (fc->show() == 0) {
    std::cout << fc->filename() << std::endl;
    return 0;
  }
  return 1;
}

#ifdef HAVE_QT
static int dlopen_getfilenameqt(int qt_major, int mode, int argc, char **argv)
{
#ifdef USE_SYSTEM_PLUGINS

# define DELETE(x)

  char plugin[] = FLTK_DIALOG_MODULE_PATH "/qt5gui.so";

# ifdef HAVE_QT4
  if (qt_major == 4) {
    plugin[strlen(plugin)-7] = '4';
  }
# endif

#else

  /* save attached libraries to disk */

# define DELETE(x) unlink(x)

  char plugin[] = "/tmp/qtgui.so.XXXXXX";
  const char *array_data;
  std::streamsize array_length;

  array_data = (char *)QTGUI_SO;
  array_length = (std::streamsize) QTGUI_SO_LEN;

# if (QTDEF == 5) && defined(HAVE_QT4)
  if (qt_major == 4) {
    array_data = (char *)qt4gui_so;
    array_length = (std::streamsize) qt4gui_so_len;
  }
# endif

  if (mkstemp(plugin) == -1) {
    std::cerr << "error: cannot create temporary file: " << plugin << std::endl;
    return -1;
  }

  std::ofstream out(plugin, std::ios::out|std::ios::binary);
  if (!out) {
    std::cerr << "error: cannot open file: " << plugin << std::endl;
    return -1;
  }

  out.write(array_data, array_length);
  out.close();

#endif  /* USE_SYSTEM_PLUGINS */

  /* dlopen() library */

  void *handle = dlopen(plugin, RTLD_LAZY);
  const char *error = dlerror();

  if (!handle) {
    std::cerr << error << std::endl;
    DELETE(plugin);
    return -1;
  }

  dlerror();

  int (*getfilenameqt) (int, const char*, const char*, int, char **);
  *(void **)(&getfilenameqt) = dlsym(handle, "getfilenameqt");

  error = dlerror();

  if (error) {
    std::cerr << "error: cannot load symbol\n" << error << std::endl;
    dlclose(handle);
    DELETE(plugin);
    return -1;
  }

  if (!title) {
    title = (mode == DIR_CHOOSER) ? "Select a directory" : "Select one or more files";
  }

  int ret = getfilenameqt(mode, separator_s.c_str(), title, argc, argv);
  dlclose(handle);
  DELETE(plugin);

  return ret;
}
#endif  /* HAVE_QT */

/* Save the attached Qt5 GUI module to disk and try to dlopen() it.
 * If it fails (i.e. because Qt5 libraries are missing) try the same with the Qt4 module.
 * If that fails too, fall back to Fl_Native_File_Chooser();
 */
static int dialog_native_file_chooser(int mode, int argc, char **argv)
{
#ifdef HAVE_QT
  ret = -1;

  if (getenv("KDE_FULL_SESSION")) {
    ret = dlopen_getfilenameqt(QTDEF, mode, argc, argv);

# if (QTDEF == 5) && defined(HAVE_QT4)
    if (ret == -1) {
      std::cerr << "warning: falling back to Qt4" << std::endl;
      ret = dlopen_getfilenameqt(4, mode, argc, argv);
    }
# endif

    if (ret == -1) {
      std::cerr << "warning: falling back to gtk" << std::endl;
    }
  }

  if (ret == -1) {
    ret = dialog_native_file_chooser_gtk(mode);
  }

  return ret;
#else  /* HAVE_QT */
  (void) argc;
  (void) argv;
  return dialog_native_file_chooser_gtk(mode);
#endif  /* HAVE_QT */
}

#ifdef HAVE_QT
/* the Qt equivalent to Fl_Native_File_Chooser() */
static int dialog_native_file_chooser_qt(int qt_major, int mode, int argc, char **argv)
{
  ret = dlopen_getfilenameqt(qt_major, mode, argc, argv);

  if (ret == -1) {
    std::cerr << "warning: falling back to fltk" << std::endl;
    ret = dialog_file_chooser_fltk(mode);
  }
  return ret;
}
#endif  /* HAVE_QT */

int dialog_file_chooser(int file_mode, int native_mode, int argc, char **argv)
{
  switch (native_mode) {
    case NATIVE_ANY:
      return dialog_native_file_chooser(file_mode, argc, argv);
    case NATIVE_GTK:
      return dialog_native_file_chooser_gtk(file_mode);
#ifdef HAVE_QT
#  ifdef HAVE_QT4
    case NATIVE_QT4:
      return dialog_native_file_chooser_qt(4, file_mode, argc, argv);
#  endif
#  ifdef HAVE_QT5
    case NATIVE_QT5:
      return dialog_native_file_chooser_qt(5, file_mode, argc, argv);
#  endif
#endif  /* HAVE_QT */
  }
  return dialog_file_chooser_fltk(file_mode);
}

