/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017, djcj <djcj@gmx.de>
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

# ifndef FLTK_DIALOG_MODULE_PATH
#   define FLTK_DIALOG_MODULE_PATH "/usr/local/lib/fltk-dialog"
# endif

  char filename[] = FLTK_DIALOG_MODULE_PATH "/qt5gui.so";

#ifdef HAVE_QT4
  if (qt_major == 4)
  {
    filename[strlen(filename)-7] = '4';
  }
#endif

#else

  /* save attached libraries to disk */

# define DELETE(x) unlink(x)

  char filename[] = "/tmp/qt5gui.so.XXXXXX";
  const char *array_data;
  std::streamsize array_length;

#ifdef HAVE_QT4
  if (qt_major == 4)
  {
    filename[7] = '4';
    array_data = (char *)qt4gui_so;
    array_length = (std::streamsize) qt4gui_so_len;
  }
  else
  {
#endif
    array_data = (char *)qt5gui_so;
    array_length = (std::streamsize) qt5gui_so_len;
#ifdef HAVE_QT4
  }
#endif

  if (mkstemp(filename) == -1)
  {
    std::cerr << "error: cannot create temporary file: " << filename << std::endl;
    return -1;
  }

  std::ofstream out(filename, std::ios::out|std::ios::binary);
  if(!out)
  {
    std::cerr << "error: cannot open file: " << filename << std::endl;
    return -1;
  }
  //std::cout << "temporary file created: " << filename << std::endl;

  out.write(array_data, array_length);
  out.close();

#endif  /* USE_SYSTEM_PLUGINS */

  /* dlopen() library */

  void *handle = dlopen(filename, RTLD_LAZY);
  const char *dlsym_error = dlerror();

  if (!handle)
  {
    std::cerr << dlsym_error << std::endl;
    DELETE(filename);
    return -1;
  }

  dlerror();

  int (*getfilenameqt) (int, const char*, const char*, int, char **);
  *(void **)(&getfilenameqt) = dlsym(handle, "getfilenameqt");

  dlsym_error = dlerror();

  if (dlsym_error)
  {
    std::cerr << "error: cannot load symbol\n" << dlsym_error << std::endl;
    dlclose(handle);
    DELETE(filename);
    return -1;
  }

  if (title == NULL)
  {
    title = (mode == DIR_CHOOSER) ? "Select a directory" : "Select one or more files";
  }

  int ret = getfilenameqt(mode, separator_s.c_str(), title, argc, argv);
  dlclose(handle);
  DELETE(filename);

  return ret;
}

