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

/* Simple custom-made localization
 *
 * utf8-hex converter: https://mothereff.in/utf-8
 *
 * Resources:
 * https://www.facebook.com/translations/style_guides
 * https://dict.leo.org
 * https://translate.google.com
 * https://ar.wikipedia.org/wiki/%D9%8A%D9%86%D8%A7%D9%8A%D8%B1_(%D8%B4%D9%87%D8%B1)
 * https://ar.wikipedia.org/wiki/%D8%A7%D9%84%D8%B3%D8%A8%D8%AA
 * https://en.wikipedia.org/wiki/Arabic_Presentation_Forms-B
 */

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Menu_Item.H>

#include <string>
#include <stdlib.h>

#include "fltk-dialog.hpp"

int selected_language = LANG_EN;
Fl_Menu_Item item_month[13];

static const char *buttons[LANG_COUNT][5] =
{
  /* LANG_EN */
  { "OK", "Cancel", "Yes", "No", "Close" },

  /* LANG_AR */
  { "\xEF\xBA\x8E\xEF\xBB\xA8\xEF\xBA\xB4\xEF\xBA\xA3", /* حسنا */
    "\xEF\xBB\x8E\xEF\xBB\x9F\xEF\xBA\x83", /* ألغ */
    "\xEF\xBB\xB0\xEF\xBB\xA0\xEF\xBA\x91", /* بلى */
    "\xEF\xBB\xBB", /* لا */
    "\xEF\xBB\x96\xEF\xBB\xA0\xEF\xBB\x8F\xEF\xBA\x83" /* أغلق */
  },

  /* LANG_DE */
  { "OK", "Abbrechen", "Ja", "Nein",
    "Schlie" "\xC3\x9F" "en"  /* Schließen */
  },

  /* LANG_ES */
  { "Aceptar", "Cancelar",
    "S" "\xC3\xAD",  /* Sí */
    "No", "Cerrar"
  },

  /* LANG_FR */
  { "Ok", "Annuler", "Oui", "Non", "Fermer" },

  /* LANG_IT */
  { "OK", "Annulla",
    "S" "\xC3\xAC",  /* Sì */
    "No",  "Chiudi"
  },

  /* LANG_JA */
  { "\xE3\x82\xAA\xE3\x83\xBC\xE3\x82\xB1\xE3\x83\xBC",  /* オーケー */
    "\xE3\x82\xAD\xE3\x83\xA3\xE3\x83\xB3\xE3\x82\xBB\xE3\x83\xAB",  /* キャンセル */
    "\xE3\x81\xAF\xE3\x81\x84",  /* はい */
    "\xE3\x81\x84\xE3\x81\x84\xE3\x81\x88",  /* いいえ */
    "\xE9\x96\x89\xE3\x81\x98\xE3\x82\x8B"  /* 閉じる */
  },

  /* LANG_PT */
  { "OK", "Cancelar", "Sim",
    "N" "\xC3\xA3" "o",  /* não */
    "Fechar"
  },

  /* LANG_RU */
  { "OK",
    "\xD0\x9E\xD1\x82\xD0\xBC\xD0\xB5\xD0\xBD\xD0\xB0",  /* Отмена */
    "\xD0\x94\xD0\xB0",  /* Да */
    "\xD0\x9D\xD0\xB5\xD1\x82",  /* Нет */
    "\xD0\x97\xD0\xB0\xD0\xBA\xD1\x80\xD1\x8B\xD1\x82\xD1\x8C"  /* Закрыть */
  },

  /* LANG_ZH */
  { "\xE7\xA1\xAE\xE5\xAE\x9A",  /* 确定 */
    "\xE5\x8F\x96\xE6\xB6\x88",  /* 取消 */
    "\xE6\x98\xAF",  /* 是 */
    "\xE5\x90\xA6",  /* 否 */
    "\xE5\x85\xB3\xE9\x97\xAD"  /* 关闭 */
  },

  /* LANG_ZH_TW */
  { "\xE7\xA2\xBA\xE5\xAE\x9A",  /* 確定 */
    "\xE5\x8F\x96\xE6\xB6\x88",  /* 取消 */
    "\xE6\x98\xAF",  /* 是 */
    "\xE5\x90\xA6",  /* 否 */
    "\xE9\x97\x9C\xE9\x96\x89"  /* 關閉 */
  }
};

const char *weekdays[LANG_COUNT][7] =
{
  /* LANG_EN */
  { "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su" },

  /* LANG_AR */
  { "\xD9\x86", /* ن */
    "\xD8\xAB", /* ث */
    "\xD8\xA8", /* ب */
    "\xD8\xAE", /* خ */
    "\xD8\xAC", /* ج */
    "\xD8\xB3", /* س */
    "\xD8\xAD"  /* ح */
  },

  /* LANG_DE */
  { "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" },

  /* LANG_ES */
  { "lu", "ma", "mi", "ju", "vi",
    "s" "\xC3\xA1",  /* sá */
    "do"
  },

  /* LANG_FR */
  { "lun", "mar", "mer", "jeu", "ven", "sam", "dim" },

  /* LANG_IT */
  { "lun", "mar", "mer", "gio", "ven", "sab", "dom" },

  /* LANG_JA */
  { "\xE6\x9C\x88",  /* 月 */
    "\xE7\x81\xAB",  /* 火 */
    "\xE6\xB0\xB4",  /* 水 */
    "\xE6\x9C\xA8",  /* 木 */
    "\xE9\x87\x91",  /* 金 */
    "\xE5\x9C\x9F",  /* 土 */
    "\xE6\x97\xA5"   /* 日 */
  },

  /* LANG_PT */
  { "seg", "ter", "qua", "qui", "sex",
    "s" "\xC3\xA1" "b",  /* Sáb */
    "dom"
  },

  /* LANG_RU */
  { "\xD0\x9F\xD0\xBE\xD0\xBD",  /* Пон */
    "\xD0\x92\xD1\x82\xD0\xBE",  /* Вто */
    "\xD0\xA1\xD1\x80\xD0\xB5",  /* Сре */
    "\xD0\xA7\xD0\xB5\xD1\x82",  /* Чет */
    "\xD0\x9F\xD1\x8F\xD1\x82",  /* Пят */
    "\xD0\xA1\xD1\x83\xD0\xB1",  /* Суб */
    "\xD0\x92\xD0\xBE\xD1\x81"   /* Вос */
  },

  /* LANG_ZH */
  { "\xE5\x91\xA8\xE4\xB8\x80",  /* 周一 */
    "\xE5\x91\xA8\xE4\xBA\x8C",  /* 周二 */
    "\xE5\x91\xA8\xE4\xB8\x89",  /* 周三 */
    "\xE5\x91\xA8\xE5\x9B\x9B",  /* 周四 */
    "\xE5\x91\xA8\xE4\xBA\x94",  /* 周五 */
    "\xE5\x91\xA8\xE5\x85\xAD",  /* 周六 */
    "\xE5\x91\xA8\xE6\x97\xA5"   /* 周日 */
  },

  /* LANG_ZH_TW */
  { "\xE9\x80\xB1\xE4\xB8\x80",  /* 週一 */
    "\xE9\x80\xB1\xE4\xBA\x8C",  /* 週二 */
    "\xE9\x80\xB1\xE4\xB8\x89",  /* 週三 */
    "\xE9\x80\xB1\xE5\x9B\x9B",  /* 週四 */
    "\xE9\x80\xB1\xE4\xBA\x94",  /* 週五 */
    "\xE9\x80\xB1\xE5\x85\xAD",  /* 週六 */
    "\xE9\x80\xB1\xE6\x97\xA5"   /* 週日 */
  }
};

/* Japanese and Chinese month names */
static const char *mzh[] =
{
  "\xE4\xB8\x80\xE6\x9C\x88",  /* 一月 */
  "\xE4\xBA\x8C\xE6\x9C\x88",  /* 二月 */
  "\xE4\xB8\x89\xE6\x9C\x88",  /* 三月 */
  "\xE5\x9B\x9B\xE6\x9C\x88",  /* 四月 */
  "\xE4\xBA\x94\xE6\x9C\x88",  /* 五月 */
  "\xE5\x85\xAD\xE6\x9C\x88",  /* 六月 */
  "\xE4\xB8\x83\xE6\x9C\x88",  /* 七月 */
  "\xE5\x85\xAB\xE6\x9C\x88",  /* 八月 */
  "\xE4\xB9\x9D\xE6\x9C\x88",  /* 九月 */
  "\xE5\x8D\x81\xE6\x9C\x88",  /* 十月 */
  "\xE5\x8D\x81\xE4\xB8\x80\xE6\x9C\x88",  /* 十一月 */
  "\xE5\x8D\x81\xE4\xBA\x8C\xE6\x9C\x88"   /* 十二月 */
};

const char *month_names[LANG_COUNT][13] =
{
  /* LANG_EN */
  { NULL,
    "January", "Febuary", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"
  },

  /* LANG_AR */
  { NULL,
    "\xEF\xBA\xAE\xEF\xBB\xB3\xEF\xBA\x8E\xEF\xBB\xA8\xEF\xBB\xB3",  /* يناير */
    "\xEF\xBA\xAE\xEF\xBB\xB3\xEF\xBA\x8D\xEF\xBA\xAE\xEF\xBA\x92\xEF\xBB\x93",  /* فبراير */
    "\xEF\xBA\xB1\xEF\xBA\xAD\xEF\xBA\x8E\xEF\xBB\xA3",  /* مارس */
    "\xEF\xBB\x9E\xEF\xBB\xB3\xEF\xBA\xAE\xEF\xBA\x91\xEF\xBA\x83",  /* أبريل */
    "\xEF\xBB\xAE\xEF\xBB\xB3\xEF\xBA\x8E\xEF\xBB\xA3",  /* مايو */
    "\xEF\xBB\xAE\xEF\xBB\xB4\xEF\xBB\xA7\xEF\xBB\xAE\xEF\xBB\xB3",  /* يونيو */
    "\xEF\xBB\xAE\xEF\xBB\xB4\xEF\xBB\x9F\xEF\xBB\xAE\xEF\xBB\xB3",  /* يوليو */
    "\xEF\xBA\xB2\xEF\xBB\x84\xEF\xBA\xB4\xEF\xBB\x8F\xEF\xBA\x83",  /* أغسطس */
    "\xEF\xBA\xAE\xEF\xBA\x92\xEF\xBB\xA4\xEF\xBA\x98\xEF\xBA\x92\xEF\xBA\xB3",  /* سبتمبر */
    "\xEF\xBA\xAE\xEF\xBA\x91\xEF\xBB\xAE\xEF\xBA\x98\xEF\xBB\x9B\xEF\xBA\x83",  /* أكتوبر */
    "\xEF\xBA\xAE\xEF\xBA\x92\xEF\xBB\xA4\xEF\xBB\x93\xEF\xBB\xAE\xEF\xBB\xA7",  /* نوفمبر */
    "\xEF\xBA\xAE\xEF\xBA\x92\xEF\xBB\xA4\xEF\xBA\xB4\xEF\xBB\xB3\xEF\xBA\xA9"   /* ديسمبر */
  },

  /* LANG_DE */
  { NULL,
    "Januar", "Februar",
    "M" "\xC3\xA4" "rz",  /* März */
    "April", "Mai", "Juni", "Juli", "August",
    "September", "Oktober", "November", "Dezember"
  },

  /* LANG_ES */
  { NULL,
    "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio",
    "Agosto", "Septiembre", "Octubre", "Noviembre", "Deciembre"
  },

  /* LANG_FR */
  { NULL,
    "Janvier",
    "F" "\xC3\xA9" "vrier",  /* Février */
    "Mars", "Avril", "Mai", "Juin", "Julliet",
    "Ao" "\xC3\xBB" "t",  /* Août */
    "Septembre", "Octobre", "Novembre",
    "D" "\xC3\xA9" "cembre"  /* Décembre */
  },

  /* LANG_IT */
  { NULL,
    "Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno",
    "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre"
  },

  /* LANG_JA */
  { NULL,
    mzh[0], mzh[1], mzh[2], mzh[3], mzh[4], mzh[5],
    mzh[6], mzh[7], mzh[8], mzh[9], mzh[10], mzh[11]
  },

  /* LANG_PT */
  { NULL,
    "Janeiro", "Fevereiro",
    "Mar" "\xC3\xA7" "o",  /* Março */
    "Abril", "Maio", "Junho", "Julho", "Agosto",
    "Setembro", "Outubro", "Novembro", "Dezembro"
  },

  /* LANG_RU */
  { NULL,
    "\xD0\xAF\xD0\xBD\xD0\xB2\xD0\xB0\xD1\x80\xD1\x8C",  /* Январь */
    "\xD0\xA4\xD0\xB5\xD0\xB2\xD1\x80\xD0\xB0\xD0\xBB\xD1\x8C",  /* Февраль */
    "\xD0\x9C\xD0\xB0\xD1\x80\xD1\x82",  /* Март */
    "\xD0\x90\xD0\xBF\xD1\x80\xD0\xB5\xD0\xBB\xD1\x8C",  /* Апрель */
    "\xD0\x9C\xD0\xB0\xD0\xB9",  /* Май */
    "\xD0\x98\xD1\x8E\xD0\xBD\xD1\x8C",  /* Июнь */
    "\xD0\x98\xD1\x8E\xD0\xBB\xD1\x8C",  /* Июль */
    "\xD0\x90\xD0\xB2\xD0\xB3\xD1\x83\xD1\x81\xD1\x82",  /* Август */
    "\xD0\xA1\xD0\xB5\xD0\xBD\xD1\x82\xD1\x8F\xD0\xB1\xD1\x80\xD1\x8C",  /* Сентябрь */
    "\xD0\x9E\xD0\xBA\xD1\x82\xD1\x8F\xD0\xB1\xD1\x80\xD1\x8C",  /* Октябрь */
    "\xD0\x9D\xD0\xBE\xD1\x8F\xD0\xB1\xD1\x80\xD1\x8C",  /* Ноябрь */
    "\xD0\x94\xD0\xB5\xD0\xBA\xD0\xB0\xD0\xB1\xD1\x80\xD1\x8C"  /* Декабрь */
  },

  /* LANG_ZH */
  { NULL,
    mzh[0], mzh[1], mzh[2], mzh[3], mzh[4], mzh[5],
    mzh[6], mzh[7], mzh[8], mzh[9], mzh[10], mzh[11]
  },

  /* LANG_ZH_TW */
  { NULL,
    mzh[0], mzh[1], mzh[2], mzh[3], mzh[4], mzh[5],
    mzh[6], mzh[7], mzh[8], mzh[9], mzh[10], mzh[11]
  }
};

void l10n(void)
{
  std::string region, lang;
  char *env;

  if (!(env = getenv("LANG")) && !(env = getenv("LANGUAGE"))) {
    return;
  }

  region = std::string(env);
  if (region.size() > 5) {
    region.erase(5);
  }
  lang = region.substr(0,2);

  if (lang == "en" || lang == "" || lang == "C") {
    return;
  } else if (lang == "ar") {
    selected_language = LANG_AR;
  } else if (lang == "de") {
    selected_language = LANG_DE;
    if (region == "de_CH") {
      buttons[LANG_DE][4] = "Schliessen";
    } else if (region == "de_AT") {
      month_names[LANG_DE][1] = "J" "\xC3\xA4" "nner";  /* Jänner */
    }
  } else if (lang == "es") {
    selected_language = LANG_ES;
  } else if (lang == "fr") {
    selected_language = LANG_FR;
  } else if (lang == "it") {
    selected_language = LANG_IT;
  } else if (lang == "ja") {
    selected_language = LANG_JA;
  } else if (lang == "pt") {
    selected_language = LANG_PT;
  } else if (lang == "ru") {
    selected_language = LANG_RU;
  } else if (lang == "zh") {
    selected_language = (region == "zh_TW") ? LANG_ZH_TW : LANG_ZH;
  }

  fl_ok = buttons[selected_language][0];
  fl_cancel = buttons[selected_language][1];
  fl_yes = buttons[selected_language][2];
  fl_no = buttons[selected_language][3];
  fl_close = buttons[selected_language][4];

#define ITEM(x)  item_month[x] = { month_names[selected_language][x+1], 0,0,0,0, FL_NORMAL_LABEL, 0, 14, 0 }
  ITEM(0); ITEM(1); ITEM(2); ITEM(3); ITEM(4); ITEM(5);
  ITEM(6); ITEM(7); ITEM(8); ITEM(9); ITEM(10); ITEM(11);
  item_month[12] = { 0,0,0,0,0,0,0,0,0 };
}

