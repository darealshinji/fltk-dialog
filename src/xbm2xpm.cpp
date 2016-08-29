/*
 * This is a modified version of FLTK's Fl_XBM_Image.cxx file.
 *
 * Copyright (c) 1997-2010, Bill Spitzak and others.
 * Copyright (c) 2016, djcj <djcj@gmx.de>
 *
 * The FLTK library and included programs are provided under the terms of the
 * GNU Library General Public License (LGPL) with the following exceptions:
 *
 * 1. Modifications to the FLTK configure script, config header file, and
 * makefiles by themselves to support a specific platform do not constitute a
 * modified or derivative work.
 * The authors do request that such modifications be contributed to the FLTK
 * project - send all contributions to "fltk-bugs@fltk.org".
 *
 * 2. Widgets that are subclassed from FLTK widgets do not constitute a
 * derivative work.
 *
 * 3. Static linking of applications and widgets to the FLTK library does not
 * constitute a derivative work and does not require the author to provide
 * source code for the application or widget, use the shared FLTK libraries, or
 * link their applications or widgets against a user-supplied version of FLTK.
 * If you link the application or widget to a modified version of FLTK, then
 * the changes to FLTK must be provided under the terms of the LGPL in sections
 * 1, 2, and 4.
 *
 * 4. You do not have to provide a copy of the FLTK license with programs that
 * are linked to the FLTK library, nor do you have to identify the FLTK license
 * in your program or documentation as required by section 6 of the LGPL.
 * However, programs must still identify their use of FLTK. The following example
 * statement can be included in user documentation to satisfy this requirement:
 * [program/widget] is based in part on the work of the FLTK project (http://www.fltk.org).
 *
 *     This library is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU Library General Public
 *     License as published by the Free Software Foundation; either
 *     version 2 of the License, or (at your option) any later version.
 *
 *     This library is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Library General Public License for more details.
 *
 *     You should have received a copy of the GNU Library General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <bitset>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


std::ofstream tmp_xpm;

static const char alphanum[] =
  "_-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;

char genRandom()
{
  return alphanum[rand() % stringLength];
}

template <typename T>
void getBits(const T &t)
{
  char *ptr((char *)(&t));
  for(int byte = 0; byte < sizeof(T); byte++){
    for(int bit = 0; bit < 8; bit++){
      tmp_xpm << (((ptr[byte]) >> bit) &1);
    }
  }
};

char *xbm2xpm(const char *input)
{
  FILE *f;
  unsigned char	*ptr;

  if ((f = fopen(input, "rb")) == NULL) {
    return NULL;
  }

  char buffer[1024];
  char junk[1024];
  int wh[2]; /* width and height */
  int i;
  for (i = 0; i < 2; i++) {
    for (;;) {
      if (!fgets(buffer, 1024, f)) {
        fclose(f);
        return NULL;
      }
      int r = sscanf(buffer, "#define %s %d", junk, &wh[i]);
      if (r >= 2) {
        break;
      }
    }
  }

  /* skip to data array: */
  for (;;) {
    if (!fgets(buffer, 1024, f)) {
      fclose(f);
      return NULL;
    }
    if (!strncmp(buffer, "static ", 7)) {
      break;
    }
  }

  int w = wh[0];
  int h = wh[1];
  int n = ((w + 7) / 8) * h;
  unsigned char *array = new unsigned char[n];

  /* read the data: */
  for (i = 0, ptr = (unsigned char *)array; i < n;) {
    if (!fgets(buffer, 1024, f)) {
      fclose(f);
      return NULL;
    }
    const char *a = buffer;
    while (*a && i<n) {
      unsigned int t;
      if (sscanf(a, " 0x%x", &t) > 0) {
        *ptr++ = (unsigned char) t;
        i++;
      }
      while (*a && *a++ != ',') { /**/ }
    }
  }

  fclose(f);

  std::string tmp_s = P_tmpdir "/";
  srand(time(0));
  for(int i = 0; i < 20; ++i) {
    tmp_s += genRandom();
  }
  char *tmp = (char *)tmp_s.c_str();

  tmp_xpm.open(tmp);
  tmp_xpm << "/* XPM */\n"
    << "static const char *icon_xpm[] = {\n"
    << "\"" << w << " " << h << " 2 1\",\n"
    << "\"0 c #FFFFFF\",\n"
    << "\"1 c #000000\",\n"
    << "\"";

  for (int it = 0; it < i; it++) {
    getBits(array[it]);
    if ((it+1) == i) {
      tmp_xpm << "\"\n};\n";
    } else {
      if (((it+1) * 8) % w == 0) {
        tmp_xpm << "\",\n"
          << "\"";
      }
    }
  }

  tmp_xpm.close();

  return tmp;
}
