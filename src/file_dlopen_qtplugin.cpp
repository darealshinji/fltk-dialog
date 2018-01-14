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

#include <fstream>
#include <iostream>
#include <string>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef USE_SYSTEM_PLUGINS
/* xxd -i qt4gui.so > qtgui_so.h
 * xxd -i qt5gui.so >> qtgui_so.h
 */
# include "qtgui_so.h"
#endif

#include "fltk-dialog.hpp"


int dlopen_getfilenameqt(int qt_major, int mode, int argc, char **argv)
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

