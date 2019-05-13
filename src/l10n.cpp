/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2019, djcj <djcj@gmx.de>
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

#include <string.h>
#include <stdlib.h>

#include "fltk-dialog.hpp"

enum {
  BT_OK,
  BT_CANCEL,
  BT_YES,
  BT_NO,
  BT_CLOSE,
  BT_COUNT
};

int selected_language = LANG_EN;
Fl_Menu_Item item_month[13];

static const char *buttons[LANG_COUNT][BT_COUNT] =
{
  /* LANG_EN */
  { "OK", "Cancel", "Yes", "No", "Close" },

  /* LANG_AR */
  { "\xD8\xAD\xD8\xB3\xD9\x86\xD8\xA7", /* حسنا */
    "\xD8\xA3\xD9\x84\xD8\xBA", /* ألغ */
    "\xD8\xA8\xD9\x84\xD9\x89", /* بلى */
    "\xD9\x84\xD8\xA7", /* لا */
    "\xD8\xA3\xD8\xBA\xD9\x84\xD9\x82" /* أغلق */
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
    "\xD9\x8A\xD9\x86\xD8\xA7\xD9\x8A\xD8\xB1",  /* يناير */
    "\xD9\x81\xD8\xA8\xD8\xB1\xD8\xA7\xD9\x8A\xD8\xB1",  /* فبراير */
    "\xD9\x85\xD8\xA7\xD8\xB1\xD8\xB3",  /* مارس */
    "\xD8\xA3\xD8\xA8\xD8\xB1\xD9\x8A\xD9\x84",  /* أبريل */
    "\xD9\x85\xD8\xA7\xD9\x8A\xD9\x88",  /* مايو */
    "\xD9\x8A\xD9\x88\xD9\x86\xD9\x8A\xD9\x88",  /* يونيو */
    "\xD9\x8A\xD9\x88\xD9\x84\xD9\x8A\xD9\x88",  /* يوليو */
    "\xD8\xA3\xD8\xBA\xD8\xB3\xD8\xB7\xD8\xB3",  /* أغسطس */
    "\xD8\xB3\xD8\xA8\xD8\xAA\xD9\x85\xD8\xA8\xD8\xB1",  /* سبتمبر */
    "\xD8\xA3\xD9\x83\xD8\xAA\xD9\x88\xD8\xA8\xD8\xB1",  /* أكتوبر */
    "\xD9\x86\xD9\x88\xD9\x81\xD9\x85\xD8\xA8\xD8\xB1",  /* نوفمبر */
    "\xD8\xAF\xD9\x8A\xD8\xB3\xD9\x85\xD8\xA8\xD8\xB1"   /* ديسمبر */
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
  const char *env;

  /* default language */
  selected_language = LANG_EN;

  if (!(env = getenv("LANG")) && !(env = getenv("LANGUAGE"))) {
    /* fall back to English */
    env = "en";
  }

  if (strncmp("en", env, 2) != 0 && strcmp("C", env) != 0) {
    if (strncmp(env, "ar", 2) == 0) {
      selected_language = LANG_AR;
    } else if (strncmp(env, "de", 2) == 0) {
      selected_language = LANG_DE;
      if (strncmp(env, "de_CH", 5) == 0) {
        buttons[LANG_DE][BT_CLOSE] = "Schliessen";
      } else if (strncmp(env, "de_AT", 5) == 0) {
        month_names[LANG_DE][1] = "J" "\xC3\xA4" "nner";  // Jänner
      }
    } else if (strncmp(env, "es", 2) == 0) {
      selected_language = LANG_ES;
    } else if (strncmp(env, "fr", 2) == 0) {
      selected_language = LANG_FR;
    } else if (strncmp(env, "it", 2) == 0) {
      selected_language = LANG_IT;
    } else if (strncmp(env, "ja", 2) == 0) {
      selected_language = LANG_JA;
    } else if (strncmp(env, "pt", 2) == 0) {
      selected_language = LANG_PT;
    } else if (strncmp(env, "ru", 2) == 0) {
      selected_language = LANG_RU;
    } else if (strncmp(env, "zh", 2) == 0) {
      selected_language = (strncmp(env, "zh_TW", 5) == 0) ? LANG_ZH_TW : LANG_ZH;
    }
  }

  /* Note: English language needs to be initialized too! */

  fl_ok = buttons[selected_language][BT_OK];
  fl_cancel = buttons[selected_language][BT_CANCEL];
  fl_yes = buttons[selected_language][BT_YES];
  fl_no = buttons[selected_language][BT_NO];
  fl_close = buttons[selected_language][BT_CLOSE];

#define ITEM(x)  item_month[x] = { month_names[selected_language][x+1], 0,0,0,0, FL_NORMAL_LABEL, 0, 14, 0 }
  ITEM(0); ITEM(1); ITEM(2); ITEM(3); ITEM(4); ITEM(5);
  ITEM(6); ITEM(7); ITEM(8); ITEM(9); ITEM(10); ITEM(11);
  item_month[12] = { 0,0,0,0,0,0,0,0,0 };
}

