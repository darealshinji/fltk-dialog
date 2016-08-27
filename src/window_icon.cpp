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

/* TO DO:
 * figure out how to convert Fl_XBM_Image to Fl_RGB_Image
 */

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_PNM_Image.H>
/* #include <FL/Fl_XBM_Image.H> */
#include <FL/Fl_XPM_Image.H>
#include <FL/Fl_RGB_Image.H>

#include <string>    /* std::string, c_str */
#include <algorithm> /* std::transform */
#include <locale>    /* std::tolower */


struct to_lower {
  int operator() (int ch) {
    return std::tolower(ch);
  }
};

std::string get_ext(std::string input, unsigned int n=4)
{
  if (input.std::string::size() <= n) {
    return "";
  }
  input = input.std::string::substr(input.std::string::size() - n);
  std::transform(input.begin(), input.end(), input.begin(), to_lower());
  return input;
}

#define SET_ICON Fl_Window::default_icon(&win_icon)

void set_window_icon(std::string file)
{
  const char *file_c = file.c_str();
  if (get_ext(file) == ".png") {
    Fl_PNG_Image win_icon(file_c);
    SET_ICON;
  } else if (get_ext(file) == ".xpm") {
    Fl_XPM_Image xpm_icon(file_c);
    Fl_RGB_Image win_icon(&xpm_icon, Fl_Color(0));
    SET_ICON;
/*
  } else if (get_ext(file) == ".xbm") {
    Fl_XBM_Image win_icon(file_c);
    SET_ICON;
 */
  } else if (get_ext(file) == ".jpg" || get_ext(file, 5) == ".jpeg") {
    Fl_JPEG_Image win_icon(file_c);
    SET_ICON;
  } else if (get_ext(file) == ".bmp") {
    Fl_BMP_Image win_icon(file_c);
    SET_ICON;
  } else if (get_ext(file) == ".gif") {
    Fl_GIF_Image gif_icon(file_c);
    Fl_RGB_Image win_icon(&gif_icon, Fl_Color(0));
    SET_ICON;
  } else if (get_ext(file) == ".pnm") {
    Fl_PNM_Image win_icon(file_c);
    SET_ICON;
  }
}

#undef SET_ICON

