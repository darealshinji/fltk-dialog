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

/* Simple custom-made localization */

#include <FL/Fl.H>
#include <FL/fl_ask.H>

#include <string>
#include <stdlib.h>

#include "Flek/FDate.H"
#include "l10n.hpp"


#define FL_OK_(x)     fl_ok = FL_OK_##x
#define FL_CANCEL_(x) fl_cancel = FL_CANCEL_##x
#define FL_YES_(x)    fl_yes = FL_YES_##x
#define FL_NO_(x)     fl_no = FL_NO_##x
#define FL_CLOSE_(x)  fl_close = FL_CLOSE_##x

#define FDATE_MO_(x) fdate_mo = FDATE_MO_##x
#define FDATE_TU_(x) fdate_tu = FDATE_TU_##x
#define FDATE_WE_(x) fdate_we = FDATE_WE_##x
#define FDATE_TH_(x) fdate_th = FDATE_TH_##x
#define FDATE_FR_(x) fdate_fr = FDATE_FR_##x
#define FDATE_SA_(x) fdate_sa = FDATE_SA_##x
#define FDATE_SU_(x) fdate_su = FDATE_SU_##x

#define FDATE_MON_JAN_(x) fdate_mon_jan = FDATE_MON_JAN_##x
#define FDATE_MON_FEB_(x) fdate_mon_feb = FDATE_MON_FEB_##x
#define FDATE_MON_MAR_(x) fdate_mon_mar = FDATE_MON_MAR_##x
#define FDATE_MON_APR_(x) fdate_mon_apr = FDATE_MON_APR_##x
#define FDATE_MON_MAY_(x) fdate_mon_may = FDATE_MON_MAY_##x
#define FDATE_MON_JUN_(x) fdate_mon_jun = FDATE_MON_JUN_##x
#define FDATE_MON_JUL_(x) fdate_mon_jul = FDATE_MON_JUL_##x
#define FDATE_MON_AUG_(x) fdate_mon_aug = FDATE_MON_AUG_##x
#define FDATE_MON_SEP_(x) fdate_mon_sep = FDATE_MON_SEP_##x
#define FDATE_MON_OCT_(x) fdate_mon_oct = FDATE_MON_OCT_##x
#define FDATE_MON_NOV_(x) fdate_mon_nov = FDATE_MON_NOV_##x
#define FDATE_MON_DEC_(x) fdate_mon_dec = FDATE_MON_DEC_##x

#define FL_L10N_(x) \
  FL_OK_(x); \
  FL_CANCEL_(x); \
  FL_YES_(x); \
  FL_NO_(x); \
  FL_CLOSE_(x)

#define WEEKDAYS_L10N_(x) \
  FDATE_MO_(x); \
  FDATE_TU_(x); \
  FDATE_WE_(x); \
  FDATE_TH_(x); \
  FDATE_FR_(x); \
  FDATE_SA_(x); \
  FDATE_SU_(x)

#define MONTHS_L10N_(x) \
  FDATE_MON_JAN_(x); \
  FDATE_MON_FEB_(x); \
  FDATE_MON_MAR_(x); \
  FDATE_MON_APR_(x); \
  FDATE_MON_MAY_(x); \
  FDATE_MON_JUN_(x); \
  FDATE_MON_JUL_(x); \
  FDATE_MON_AUG_(x); \
  FDATE_MON_SEP_(x); \
  FDATE_MON_OCT_(x); \
  FDATE_MON_NOV_(x); \
  FDATE_MON_DEC_(x)


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
      WEEKDAYS_L10N_(DE);
      MONTHS_L10N_(DE);
      if (region == "de_CH")
      {
        FL_CLOSE_(DE_CH);
        FDATE_MON_JAN_(DE_CH);
      }
      else if (region == "de_AT")
      {
        FDATE_MON_JAN_(DE_AT);
      }
    }
    else if (lang == "zh")
    {
      FL_L10N_(ZH);
      MONTHS_L10N_(ZH);
      if (region == "zh_TW")
      {
        FL_OK_(ZH_TW);
        FL_CLOSE_(ZH_TW);
        WEEKDAYS_L10N_(ZH_TW);
      }
      else
      {
        WEEKDAYS_L10N_(ZH);
      }
    }
    else if (lang == "ar") { FL_L10N_(AR); WEEKDAYS_L10N_(AR); MONTHS_L10N_(AR); }
    else if (lang == "es") { FL_L10N_(ES); WEEKDAYS_L10N_(ES); MONTHS_L10N_(ES); }
    else if (lang == "fr") { FL_L10N_(FR); WEEKDAYS_L10N_(FR); MONTHS_L10N_(FR); }
    else if (lang == "it") { FL_L10N_(IT); WEEKDAYS_L10N_(IT); MONTHS_L10N_(IT); }
    else if (lang == "ja") { FL_L10N_(JA); WEEKDAYS_L10N_(JA); MONTHS_L10N_(JA); }
    else if (lang == "pt") { FL_L10N_(PT); WEEKDAYS_L10N_(PT); MONTHS_L10N_(PT); }
    else if (lang == "ru") { FL_L10N_(RU); WEEKDAYS_L10N_(RU); MONTHS_L10N_(RU); }
  }
}

