/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2018, djcj <djcj@gmx.de>
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

/*** empty template ***
#define FDATE_MO_ ""
#define FDATE_TU_ ""
#define FDATE_WE_ ""
#define FDATE_TH_ ""
#define FDATE_FR_ ""
#define FDATE_SA_ ""
#define FDATE_SU_ ""
#define FDATE_MON_JAN_ ""
#define FDATE_MON_FEB_ ""
#define FDATE_MON_MAR_ ""
#define FDATE_MON_APR_ ""
#define FDATE_MON_MAY_ ""
#define FDATE_MON_JUN_ ""
#define FDATE_MON_JUL_ ""
#define FDATE_MON_AUG_ ""
#define FDATE_MON_SEP_ ""
#define FDATE_MON_OCT_ ""
#define FDATE_MON_NOV_ ""
#define FDATE_MON_DEC_ ""
***/

/* utf8-hex converter: https://mothereff.in/utf-8
 *
 * Resources:
 * https://www.facebook.com/translations/style_guides
 * https://dict.leo.org
 * https://translate.google.com
 * https://ar.wikipedia.org/wiki/%D9%8A%D9%86%D8%A7%D9%8A%D8%B1_(%D8%B4%D9%87%D8%B1)
 * https://ar.wikipedia.org/wiki/%D8%A7%D9%84%D8%B3%D8%A8%D8%AA
 * https://en.wikipedia.org/wiki/Arabic_Presentation_Forms-B
 */


/*** ar ***/

#define YEH_I   "\xEF\xBB\xB3"
#define YEH_M   "\xEF\xBB\xB4"
#define NOON_I  "\xEF\xBB\xA7"
#define NOON_M  "\xEF\xBB\xA8"
#define ALEF    "\xEF\xBA\x8D"
#define ALEF_F  "\xEF\xBA\x8E"
#define REH     "\xEF\xBA\xAD"
#define REH_F   "\xEF\xBA\xAE"
#define SEEN    "\xEF\xBA\xB1"
#define SEEN_F  "\xEF\xBA\xB2"
#define SEEN_I  "\xEF\xBA\xB3"
#define SEEN_M  "\xEF\xBA\xB4"
#define HAH_I   "\xEF\xBA\xA3"
#define ALEF_HAMZA_ABOVE "\xEF\xBA\x83"
#define BEH_I   "\xEF\xBA\x91"
#define BEH_M   "\xEF\xBA\x92"
#define LAM_F   "\xEF\xBB\x9E"
#define LAM_I   "\xEF\xBB\x9F"
#define LAM_M   "\xEF\xBB\xA0"
#define GHAIN_F "\xEF\xBB\x8E"
#define GHAIN_I "\xEF\xBB\x8F"
#define ALEF_MAKSURA_F "\xEF\xBB\xB0"
#define QAF_F   "\xEF\xBB\x96"
#define FEH_I   "\xEF\xBB\x93"
#define MEEM_I  "\xEF\xBB\xA3"
#define MEEM_M  "\xEF\xBB\xA4"
#define WAW_F   "\xEF\xBB\xAE"
#define TAH_M   "\xEF\xBB\x84"
#define TEH_M   "\xEF\xBA\x98"
#define KAF_I   "\xEF\xBB\x9B"
#define DAL     "\xEF\xBA\xA9"

#define FL_OK_AR     ALEF_F NOON_M SEEN_M HAH_I /* حسنا */
#define FL_CANCEL_AR GHAIN_F LAM_I ALEF_HAMZA_ABOVE  /* ألغ */
#define FL_YES_AR    ALEF_MAKSURA_F LAM_M BEH_I  /* بلى */
#define FL_NO_AR     "\xEF\xBB\xBB"  /* لا */
#define FL_CLOSE_AR  QAF_F LAM_M GHAIN_I ALEF_HAMZA_ABOVE  /* أغلق */

#define FDATE_MO_AR "\xD9\x86"  /* ن */
#define FDATE_TU_AR "\xD8\xAB"  /* ث */
#define FDATE_WE_AR "\xD8\xA8"  /* ب */
#define FDATE_TH_AR "\xD8\xAE"  /* خ */
#define FDATE_FR_AR "\xD8\xAC"  /* ج */
#define FDATE_SA_AR "\xD8\xB3"  /* س */
#define FDATE_SU_AR "\xD8\xAD"  /* ح */

#define FDATE_MON_JAN_AR  REH_F YEH_I ALEF_F NOON_M YEH_I  /* يناير */
#define FDATE_MON_FEB_AR  REH_F YEH_I ALEF REH_F NOON_M FEH_I  /* فبراير */
#define FDATE_MON_MAR_AR  SEEN REH ALEF_F MEEM_I  /* مارس */
#define FDATE_MON_APR_AR  LAM_F YEH_I REH_F NOON_I ALEF_HAMZA_ABOVE  /* أبريل */
#define FDATE_MON_MAY_AR  WAW_F YEH_I ALEF_F MEEM_I  /* مايو */
#define FDATE_MON_JUN_AR  WAW_F YEH_M NOON_I WAW_F YEH_I  /* يونيو */
#define FDATE_MON_JUL_AR  WAW_F YEH_M LAM_I WAW_F YEH_I  /* يوليو */
#define FDATE_MON_AUG_AR  SEEN_F TAH_M SEEN_M GHAIN_I ALEF_HAMZA_ABOVE  /* أغسطس */
#define FDATE_MON_SEP_AR  REH_F BEH_M MEEM_M TEH_M BEH_M SEEN_I  /* سبتمبر */
#define FDATE_MON_OCT_AR  REH_F BEH_I WAW_F TEH_M KAF_I ALEF_HAMZA_ABOVE  /* أكتوبر */
#define FDATE_MON_NOV_AR  REH_F BEH_M MEEM_M FEH_I WAW_F NOON_I  /* نوفمبر */
#define FDATE_MON_DEC_AR  REH_F BEH_M MEEM_M SEEN_M YEH_I DAL  /* ديسمبر */


/*** de ***/

#define FL_OK_DE       "OK"
#define FL_CANCEL_DE   "Abbrechen"
#define FL_YES_DE      "Ja"
#define FL_NO_DE       "Nein"
#define FL_CLOSE_DE    "Schlie" "\xC3\x9F" "en"  /* Schließen */
#define FL_CLOSE_DE_CH "Schliessen"

#define FDATE_MO_DE "Mo"
#define FDATE_TU_DE "Di"
#define FDATE_WE_DE "Mi"
#define FDATE_TH_DE "Do"
#define FDATE_FR_DE "Fr"
#define FDATE_SA_DE "Sa"
#define FDATE_SU_DE "So"

#define FDATE_MON_JAN_DE    "Januar"
#define FDATE_MON_JAN_DE_AT "J" "\xC3\xA4" "nnar"  /* Jänner */
#define FDATE_MON_FEB_DE    "Februar"
#define FDATE_MON_MAR_DE    "M" "\xC3\xA4" "rz"  /* März */
#define FDATE_MON_APR_DE    "April"
#define FDATE_MON_MAY_DE    "Mai"
#define FDATE_MON_JUN_DE    "Juni"
#define FDATE_MON_JUL_DE    "Juli"
#define FDATE_MON_AUG_DE    "August"
#define FDATE_MON_SEP_DE    "September"
#define FDATE_MON_OCT_DE    "Oktober"
#define FDATE_MON_NOV_DE    "November"
#define FDATE_MON_DEC_DE    "Dezember"


/*** es ***/

#define FL_OK_ES     "Aceptar"
#define FL_CANCEL_ES "Cancelar"
#define FL_YES_ES    "S" "\xC3\xAD"  /* Sí */
#define FL_NO_ES     "No"
#define FL_CLOSE_ES  "Cerrar"

#define FDATE_MO_ES "lu"
#define FDATE_TU_ES "ma"
#define FDATE_WE_ES "mi"
#define FDATE_TH_ES "ju"
#define FDATE_FR_ES "vi"
#define FDATE_SA_ES "s" "\xC3\xA1"  /* Sá */
#define FDATE_SU_ES "do"

#define FDATE_MON_JAN_ES "Enero"
#define FDATE_MON_FEB_ES "Febrero"
#define FDATE_MON_MAR_ES "Marzo"
#define FDATE_MON_APR_ES "Abril"
#define FDATE_MON_MAY_ES "Mayo"
#define FDATE_MON_JUN_ES "Junio"
#define FDATE_MON_JUL_ES "Julio"
#define FDATE_MON_AUG_ES "Agosto"
#define FDATE_MON_SEP_ES "Septiembre"
#define FDATE_MON_OCT_ES "Octubre"
#define FDATE_MON_NOV_ES "Noviembre"
#define FDATE_MON_DEC_ES "Deciembre"


/*** fr ***/

#define FL_OK_FR     "Ok"
#define FL_CANCEL_FR "Annuler"
#define FL_YES_FR    "Oui"
#define FL_NO_FR     "Non"
#define FL_CLOSE_FR  "Fermer"

#define FDATE_MO_FR "lun"
#define FDATE_TU_FR "mar"
#define FDATE_WE_FR "mer"
#define FDATE_TH_FR "jeu"
#define FDATE_FR_FR "ven"
#define FDATE_SA_FR "sam"
#define FDATE_SU_FR "dim"

#define FDATE_MON_JAN_FR "Janvier"
#define FDATE_MON_FEB_FR "F" "\xC3\xA9" "vrier"  /* Février */
#define FDATE_MON_MAR_FR "Mars"
#define FDATE_MON_APR_FR "Avril"
#define FDATE_MON_MAY_FR "Mai"
#define FDATE_MON_JUN_FR "Juin"
#define FDATE_MON_JUL_FR "Julliet"
#define FDATE_MON_AUG_FR "Ao" "\xC3\xBB" "t"  /* Août */
#define FDATE_MON_SEP_FR "Septembre"
#define FDATE_MON_OCT_FR "Octobre"
#define FDATE_MON_NOV_FR "Novembre"
#define FDATE_MON_DEC_FR "D" "\xC3\xA9" "cembre"  /* Décembre */


/*** it ***/

#define FL_OK_IT     "OK"
#define FL_CANCEL_IT "Annulla"
#define FL_YES_IT    "S" "\xC3\xAC"  /* Sì */
#define FL_NO_IT     "No"
#define FL_CLOSE_IT  "Chiudi"

#define FDATE_MO_IT "lun"
#define FDATE_TU_IT "mar"
#define FDATE_WE_IT "mer"
#define FDATE_TH_IT "gio"
#define FDATE_FR_IT "ven"
#define FDATE_SA_IT "sab"
#define FDATE_SU_IT "dom"

#define FDATE_MON_JAN_IT "Gennaio"
#define FDATE_MON_FEB_IT "Febbraio"
#define FDATE_MON_MAR_IT "Marzo"
#define FDATE_MON_APR_IT "Aprile"
#define FDATE_MON_MAY_IT "Maggio"
#define FDATE_MON_JUN_IT "Giugno"
#define FDATE_MON_JUL_IT "Luglio"
#define FDATE_MON_AUG_IT "Agosto"
#define FDATE_MON_SEP_IT "Settembre"
#define FDATE_MON_OCT_IT "Ottobre"
#define FDATE_MON_NOV_IT "Novembre"
#define FDATE_MON_DEC_IT "Dicembre"


/*** ja ***/

#define FL_OK_JA     "\xE3\x82\xAA\xE3\x83\xBC\xE3\x82\xB1\xE3\x83\xBC"  /* オーケー */
#define FL_CANCEL_JA "\xE3\x82\xAD\xE3\x83\xA3\xE3\x83\xB3\xE3\x82\xBB\xE3\x83\xAB"  /* キャンセル */
#define FL_YES_JA    "\xE3\x81\xAF\xE3\x81\x84"  /* はい */
#define FL_NO_JA     "\xE3\x81\x84\xE3\x81\x84\xE3\x81\x88"  /* いいえ */
#define FL_CLOSE_JA  "\xE9\x96\x89\xE3\x81\x98\xE3\x82\x8B"  /* 閉じる */

#define FDATE_MO_JA  "\xE6\x9C\x88"  /* 月 */
#define FDATE_TU_JA  "\xE7\x81\xAB"  /* 火 */
#define FDATE_WE_JA  "\xE6\xB0\xB4"  /* 水 */
#define FDATE_TH_JA  "\xE6\x9C\xA8"  /* 木 */
#define FDATE_FR_JA  "\xE9\x87\x91"  /* 金 */
#define FDATE_SA_JA  "\xE5\x9C\x9F"  /* 土 */
#define FDATE_SU_JA  "\xE6\x97\xA5"  /* 日 */

#define FDATE_MON_JAN_JA "\xE4\xB8\x80\xE6\x9C\x88"  /* 一月 */
#define FDATE_MON_FEB_JA "\xE4\xBA\x8C\xE6\x9C\x88"  /* 二月 */
#define FDATE_MON_MAR_JA "\xE4\xB8\x89\xE6\x9C\x88"  /* 三月 */
#define FDATE_MON_APR_JA "\xE5\x9B\x9B\xE6\x9C\x88"  /* 四月 */
#define FDATE_MON_MAY_JA "\xE4\xBA\x94\xE6\x9C\x88"  /* 五月 */
#define FDATE_MON_JUN_JA "\xE5\x85\xAD\xE6\x9C\x88"  /* 六月 */
#define FDATE_MON_JUL_JA "\xE4\xB8\x83\xE6\x9C\x88"  /* 七月 */
#define FDATE_MON_AUG_JA "\xE5\x85\xAB\xE6\x9C\x88"  /* 八月 */
#define FDATE_MON_SEP_JA "\xE4\xB9\x9D\xE6\x9C\x88"  /* 九月 */
#define FDATE_MON_OCT_JA "\xE5\x8D\x81\xE6\x9C\x88"  /* 十月 */
#define FDATE_MON_NOV_JA "\xE5\x8D\x81\xE4\xB8\x80\xE6\x9C\x88"  /* 十一月 */
#define FDATE_MON_DEC_JA "\xE5\x8D\x81\xE4\xBA\x8C\xE6\x9C\x88"  /* 十二月 */


/*** pt ***/

#define FL_OK_PT     "OK"
#define FL_CANCEL_PT "Cancelar"
#define FL_YES_PT    "Sim"
#define FL_NO_PT     "N" "\xC3\xA3" "o"  /* não */
#define FL_CLOSE_PT  "Fechar"

#define FDATE_MO_PT "seg"
#define FDATE_TU_PT "ter"
#define FDATE_WE_PT "qua"
#define FDATE_TH_PT "qui"
#define FDATE_FR_PT "sex"
#define FDATE_SA_PT "s" "\xC3\xA1" "b"  /* Sáb */
#define FDATE_SU_PT "dom"

#define FDATE_MON_JAN_PT "Janeiro"
#define FDATE_MON_FEB_PT "Fevereiro"
#define FDATE_MON_MAR_PT "Mar" "\xC3\xA7" "o"  /* Março */
#define FDATE_MON_APR_PT "Abril"
#define FDATE_MON_MAY_PT "Maio"
#define FDATE_MON_JUN_PT "Junho"
#define FDATE_MON_JUL_PT "Julho"
#define FDATE_MON_AUG_PT "Agosto"
#define FDATE_MON_SEP_PT "Setembro"
#define FDATE_MON_OCT_PT "Outubro"
#define FDATE_MON_NOV_PT "Novembro"
#define FDATE_MON_DEC_PT "Dezembro"


/*** ru ***/

#define FL_OK_RU     "OK"
#define FL_CANCEL_RU "\xD0\x9E\xD1\x82\xD0\xBC\xD0\xB5\xD0\xBD\xD0\xB0"  /* Отмена */
#define FL_YES_RU    "\xD0\x94\xD0\xB0"  /* Да */
#define FL_NO_RU     "\xD0\x9D\xD0\xB5\xD1\x82"  /* Нет */
#define FL_CLOSE_RU  "\xD0\x97\xD0\xB0\xD0\xBA\xD1\x80\xD1\x8B\xD1\x82\xD1\x8C"  /* Закрыть */

#define FDATE_MO_RU "\xD0\x9F\xD0\xBE\xD0\xBD"  /* Пон */
#define FDATE_TU_RU "\xD0\x92\xD1\x82\xD0\xBE"  /* Вто */
#define FDATE_WE_RU "\xD0\xA1\xD1\x80\xD0\xB5"  /* Сре */
#define FDATE_TH_RU "\xD0\xA7\xD0\xB5\xD1\x82"  /* Чет */
#define FDATE_FR_RU "\xD0\x9F\xD1\x8F\xD1\x82"  /* Пят */
#define FDATE_SA_RU "\xD0\xA1\xD1\x83\xD0\xB1"  /* Суб */
#define FDATE_SU_RU "\xD0\x92\xD0\xBE\xD1\x81"  /* Вос */

#define FDATE_MON_JAN_RU "\xD0\xAF\xD0\xBD\xD0\xB2\xD0\xB0\xD1\x80\xD1\x8C"  /* Январь */
#define FDATE_MON_FEB_RU "\xD0\xA4\xD0\xB5\xD0\xB2\xD1\x80\xD0\xB0\xD0\xBB\xD1\x8C"  /* Февраль */
#define FDATE_MON_MAR_RU "\xD0\x9C\xD0\xB0\xD1\x80\xD1\x82"  /* Март */
#define FDATE_MON_APR_RU "\xD0\x90\xD0\xBF\xD1\x80\xD0\xB5\xD0\xBB\xD1\x8C"  /* Апрель */
#define FDATE_MON_MAY_RU "\xD0\x9C\xD0\xB0\xD0\xB9"  /* Май */
#define FDATE_MON_JUN_RU "\xD0\x98\xD1\x8E\xD0\xBD\xD1\x8C"  /* Июнь */
#define FDATE_MON_JUL_RU "\xD0\x98\xD1\x8E\xD0\xBB\xD1\x8C"  /* Июль */
#define FDATE_MON_AUG_RU "\xD0\x90\xD0\xB2\xD0\xB3\xD1\x83\xD1\x81\xD1\x82"  /* Август */
#define FDATE_MON_SEP_RU "\xD0\xA1\xD0\xB5\xD0\xBD\xD1\x82\xD1\x8F\xD0\xB1\xD1\x80\xD1\x8C"  /* Сентябрь */
#define FDATE_MON_OCT_RU "\xD0\x9E\xD0\xBA\xD1\x82\xD1\x8F\xD0\xB1\xD1\x80\xD1\x8C"  /* Октябрь */
#define FDATE_MON_NOV_RU "\xD0\x9D\xD0\xBE\xD1\x8F\xD0\xB1\xD1\x80\xD1\x8C"  /* Ноябрь */
#define FDATE_MON_DEC_RU "\xD0\x94\xD0\xB5\xD0\xBA\xD0\xB0\xD0\xB1\xD1\x80\xD1\x8C"  /* Декабрь */


/*** zh ***/

#define FL_OK_ZH       "\xE7\xA1\xAE\xE5\xAE\x9A"  /* 确定 */
#define FL_OK_ZH_TW    "\xE7\xA2\xBA\xE5\xAE\x9A"  /* 確定 */
#define FL_CANCEL_ZH   "\xE5\x8F\x96\xE6\xB6\x88"  /* 取消 */
#define FL_YES_ZH      "\xE6\x98\xAF"  /* 是 */
#define FL_NO_ZH       "\xE5\x90\xA6"  /* 否 */
#define FL_CLOSE_ZH    "\xE5\x85\xB3\xE9\x97\xAD"  /* 关闭 */
#define FL_CLOSE_ZH_TW "\xE9\x97\x9C\xE9\x96\x89"  /* 關閉 */

#define FDATE_MO_ZH    "\xE5\x91\xA8\xE4\xB8\x80"  /* 周一 */
#define FDATE_MO_ZH_TW "\xE9\x80\xB1\xE4\xB8\x80"  /* 週一 */
#define FDATE_TU_ZH    "\xE5\x91\xA8\xE4\xBA\x8C"  /* 周二 */
#define FDATE_TU_ZH_TW "\xE9\x80\xB1\xE4\xBA\x8C"  /* 週二 */
#define FDATE_WE_ZH    "\xE5\x91\xA8\xE4\xB8\x89"  /* 周三 */
#define FDATE_WE_ZH_TW "\xE9\x80\xB1\xE4\xB8\x89"  /* 週三 */
#define FDATE_TH_ZH    "\xE5\x91\xA8\xE5\x9B\x9B"  /* 周四 */
#define FDATE_TH_ZH_TW "\xE9\x80\xB1\xE5\x9B\x9B"  /* 週四 */
#define FDATE_FR_ZH    "\xE5\x91\xA8\xE4\xBA\x94"  /* 周五 */
#define FDATE_FR_ZH_TW "\xE9\x80\xB1\xE4\xBA\x94"  /* 週五 */
#define FDATE_SA_ZH    "\xE5\x91\xA8\xE5\x85\xAD"  /* 周六 */
#define FDATE_SA_ZH_TW "\xE9\x80\xB1\xE5\x85\xAD"  /* 週六 */
#define FDATE_SU_ZH    "\xE5\x91\xA8\xE6\x97\xA5"  /* 周日 */
#define FDATE_SU_ZH_TW "\xE9\x80\xB1\xE6\x97\xA5"  /* 週日 */

/* same as in Japanese */
#define FDATE_MON_JAN_ZH "\xE4\xB8\x80\xE6\x9C\x88"  /* 一月 */
#define FDATE_MON_FEB_ZH "\xE4\xBA\x8C\xE6\x9C\x88"  /* 二月 */
#define FDATE_MON_MAR_ZH "\xE4\xB8\x89\xE6\x9C\x88"  /* 三月 */
#define FDATE_MON_APR_ZH "\xE5\x9B\x9B\xE6\x9C\x88"  /* 四月 */
#define FDATE_MON_MAY_ZH "\xE4\xBA\x94\xE6\x9C\x88"  /* 五月 */
#define FDATE_MON_JUN_ZH "\xE5\x85\xAD\xE6\x9C\x88"  /* 六月 */
#define FDATE_MON_JUL_ZH "\xE4\xB8\x83\xE6\x9C\x88"  /* 七月 */
#define FDATE_MON_AUG_ZH "\xE5\x85\xAB\xE6\x9C\x88"  /* 八月 */
#define FDATE_MON_SEP_ZH "\xE4\xB9\x9D\xE6\x9C\x88"  /* 九月 */
#define FDATE_MON_OCT_ZH "\xE5\x8D\x81\xE6\x9C\x88"  /* 十月 */
#define FDATE_MON_NOV_ZH "\xE5\x8D\x81\xE4\xB8\x80\xE6\x9C\x88"  /* 十一月 */
#define FDATE_MON_DEC_ZH "\xE5\x8D\x81\xE4\xBA\x8C\xE6\x9C\x88"  /* 十二月 */

