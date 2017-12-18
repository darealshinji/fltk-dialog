/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, djcj <djcj@gmx.de>
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

#include <FL/Fl_PNG_Image.H>

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#ifndef USE_SYSTEM_PLUGINS
/* xxd -i rsvg_convert.so > rsvg_convert_so.h */
# include "rsvg_convert_so.h"
#endif

#include "fltk-dialog.hpp"

int rsvg_default_icon(const char *file)
{
#ifdef USE_SYSTEM_PLUGINS

# define DELETE(x)

  const char *plugin = FLTK_DIALOG_MODULE_PATH "/rsvg_convert.so";

#else

  /* save attached library to disk */

# define DELETE(x) unlink(x)

  char plugin[] = "/tmp/rsvg_convert.so.XXXXXX";
  const char *array_data;
  std::streamsize array_length;

  array_data = (char *)rsvg_convert_so;
  array_length = (std::streamsize) rsvg_convert_so_len;

  if (mkstemp(plugin) == -1)
  {
    std::cerr << "error: cannot create temporary file: " << plugin << std::endl;
    return 1;
  }

  std::ofstream out(plugin, std::ios::out|std::ios::binary);
  if (!out)
  {
    std::cerr << "error: cannot open file: " << plugin << std::endl;
    return 1;
  }

  out.write(array_data, array_length);
  out.close();

#endif

  /* dlopen() library */

  void *handle = dlopen(plugin, RTLD_LAZY);
  const char *dlsym_error = dlerror();

  if (!handle)
  {
    std::cerr << dlsym_error << std::endl;
    DELETE(plugin);
    return 1;
  }

  dlerror();

  int (*rsvg_to_png) (const char *, const char *);
  *(void **)(&rsvg_to_png) = dlsym(handle, "rsvg_to_png");

  dlsym_error = dlerror();

  if (dlsym_error)
  {
    std::cerr << "error: cannot load symbol\n" << dlsym_error << std::endl;
    dlclose(handle);
    DELETE(plugin);
    return 1;
  }

  char png[] = "/tmp/icon-png-XXXXXX";
  if (mkstemp(png) == -1)
  {
    return 1;
  }

  int ret = 1;
  if (rsvg_to_png(file, png) == 0)
  {
    Fl_Window::default_icon(new Fl_PNG_Image(png));
    ret = 0;
  }

  dlclose(handle);
  DELETE(plugin);
  unlink(png);

  return ret;
}

