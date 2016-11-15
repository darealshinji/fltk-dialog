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
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>

#include <iostream>
#include <sys/stat.h>

#include "fltk-dialog.hpp"


int dialog_file_chooser()
{
  struct stat s;

  if (title == NULL)
  {
    title = "Select a file";
  }

  char *file = fl_file_chooser(title, "*", NULL);

  if (!((stat(file, &s) == 0) && (s.st_mode &S_IFREG)))
  {
    return 1;
  }

  std::cout << file << std::endl;
  return 0;
}

int dialog_dir_chooser()
{
  struct stat s;

  if (title == NULL)
  {
    title = "Select a directory";
  }

  char *dir = fl_dir_chooser(title, NULL);

  if (!((stat(dir, &s) == 0) && (s.st_mode &S_IFDIR)))
  {
    return 1;
  }

  std::cout << dir << std::endl;

  return 0;
}

#ifdef WITH_NATIVE_FILE_CHOOSER

int dialog_native_file_chooser_gtk(int mode)
{
  Fl_Native_File_Chooser fnfc;
  char *fnfc_def_title = NULL;

  if (title == NULL)
  {
    if (mode == DIR_CHOOSER)
    {
      fnfc.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
      fnfc_def_title = (char *)"Select a directory";
    }
    else
    {
      fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
      fnfc_def_title = (char *)"Select a file";
    }
    title = fnfc_def_title;
  }

  fnfc.title(title);

  if (fnfc.show())
  {
    return 1;
  }

  std::cout << fnfc.filename() << std::endl;
  return 0;
}

/* Save the attached Qt5 GUI module to disk and try to dlopen() it.
 * If it fails (i.e. because Qt5 libraries are missing) try the same with the Qt4 module.
 * If that fails too, fall back to Fl_Native_File_Chooser();
 */
int dialog_native_file_chooser(int mode, int argc, char **argv)
{
#ifdef HAVE_QT
  ret = -1;

  if (getenv("KDE_FULL_SESSION"))
  {
#if defined(HAVE_QT5) && defined(HAVE_QT4)
    ret = dlopen_getfilenameqt(5, mode, argc, argv);

    if (ret == -1)
    {
      ret = dlopen_getfilenameqt(4, mode, argc, argv);
    }
#elif defined(HAVE_QT5)
    ret = dlopen_getfilenameqt(5, mode, argc, argv);
#elif defined(HAVE_QT4)
    ret = dlopen_getfilenameqt(4, mode, argc, argv);
#endif  /* HAVE_QT5 && HAVE_QT4 */
  }

  if (ret == -1)
  {
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
int dialog_native_file_chooser_qt(int qt_major, int mode, int argc, char **argv)
{
  ret = dlopen_getfilenameqt(qt_major, mode, argc, argv);

  if (ret == -1)
  {
    if (mode == DIR_CHOOSER)
    {
      ret = dialog_fl_dir_chooser();
    }
    else
    {
      ret = dialog_fl_file_chooser();
    }
  }
  return ret;
}
#endif  /* HAVE_QT */

#endif  /* WITH_NATIVE_FILE_CHOOSER */

