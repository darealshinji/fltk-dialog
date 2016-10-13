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

/* Simple custom-made localization for "ok", "cancel", "yes/no" and "close" buttons
 */

#include <FL/Fl.H>
#include <FL/fl_ask.H>

#include <string>
#include <stdlib.h>

#include "l10n.hpp"


#define FL_OK_(x)     fl_ok = FL_OK_##x
#define FL_CANCEL_(x) fl_cancel = FL_CANCEL_##x
#define FL_YES_(x)    fl_yes = FL_YES_##x
#define FL_NO_(x)     fl_no = FL_NO_##x
#define FL_CLOSE_(x)  fl_close = FL_CLOSE_##x
#define FL_L10N_(x)   FL_OK_(x); FL_CANCEL_(x); FL_YES_(x); FL_NO_(x); FL_CLOSE_(x)

void l10n()
{
  std::string region = std::string(getenv("LANG"));
  std::string lang = region.substr(0,2);

  if (lang == "")
  {
    region = std::string(getenv("LANGUAGE"));
    lang = region.substr(0,2);
  }
  region = region.substr(0,5);

  if (lang != "C" && lang != "")
  {
    if (lang == "de")
    {
      FL_L10N_(DE);
      if (region == "de_CH")
      {
        FL_CLOSE_(DE_CH);
      }
    }
    else if (lang == "zh")
    {
      FL_L10N_(ZH);
      if (region == "zh_TW")
      {
        FL_OK_(ZH_TW);
        FL_CLOSE_(ZH_TW);
      }
    }
    else if (lang == "ar") { FL_L10N_(AR); }
    else if (lang == "es") { FL_L10N_(ES); }
    else if (lang == "fr") { FL_L10N_(FR); }
    else if (lang == "it") { FL_L10N_(IT); }
    else if (lang == "ja") { FL_L10N_(JA); }
    else if (lang == "pt") { FL_L10N_(PT); }
    else if (lang == "ru") { FL_L10N_(RU); }
  }
}

