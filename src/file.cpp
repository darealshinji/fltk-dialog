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

#include <iostream>    /* std::cout, std::endl */
#include <sys/stat.h>  /* stat */

#include "fltk-dialog.hpp"


int dialog_fl_file_chooser(char *file_chooser_title)
{
  struct stat s;

  if (file_chooser_title == NULL) {
    file_chooser_title = (char *)"Select a file";
  }

  char *file = fl_file_chooser(file_chooser_title, "*", NULL);

  if (!((stat(file, &s) == 0) && (s.st_mode &S_IFREG))) {
    return 1;
  }
  std::cout << file << std::endl;
  return 0;
}

int dialog_fl_dir_chooser(char* dir_chooser_title)
{
  struct stat s;

  if (dir_chooser_title == NULL) {
    dir_chooser_title = (char *)"Select a directory";
  }

  char *dir = fl_dir_chooser(dir_chooser_title, NULL);

  if (!((stat(dir, &s) == 0) && (s.st_mode &S_IFDIR))) {
    return 1;
  }
  std::cout << dir << std::endl;
  return 0;
}

int dialog_fl_native_file_chooser(char *fnfc_title,
                                  int   fnfc_dir)
{
  Fl_Native_File_Chooser fnfc;
  char *fnfc_def_title = NULL;

  if (fnfc_title == NULL) {
    if (fnfc_dir == DIR_CHOOSER) {
      fnfc.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
      fnfc_def_title = (char *)"Select a directory";
    } else {
      fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
      fnfc_def_title = (char *)"Select a file";
    }
    fnfc_title = fnfc_def_title;
  }

  fnfc.title(fnfc_title);

  if (fnfc.show()) {
    return 1;
  }
  std::cout << fnfc.filename() << std::endl;
  return 0;
}

