/*
 * Copyright (c) 2011 Stefano Sabatini
 * Copyright (c) 2010 S.N. Hemanth Meenakshisundaram
 * Copyright (c) 2003 Gustavo Sverzut Barbieri <gsbarbieri@yahoo.com.br>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * based on shape_text() function from libavfilter_vf_drawtext.c
 */

#include <fribidi.h>
#include <string.h>

#define IS_NEWLINE(x)  (x == '\n' || x == '\r' || x == '\f' || x == '\v')

#define DEL(x)  if (x) { delete x; }

/**
 * Returns pointer to new[] string (delete[] it later) or NULL on error.
 */
char *fribidi_parse_line(const char *input)
{
  char *output = NULL;
  static const FriBidiFlags flags = FRIBIDI_FLAGS_DEFAULT | FRIBIDI_FLAGS_ARABIC;
  FriBidiChar *unicodestr = NULL;
  FriBidiStrIndex len, line_start = 0, line_end = 0, i, j;
  FriBidiParType direction = FRIBIDI_PAR_LTR;
  FriBidiLevel *embedding_levels = NULL;
  FriBidiArabicProp *ar_props = NULL;
  FriBidiCharType *bidi_types = NULL;

  len = strlen(input);
  unicodestr = new FriBidiChar[len * sizeof(*unicodestr)];
  if (!unicodestr) {
    goto out;
  }
  len = fribidi_charset_to_unicode(FRIBIDI_CHAR_SET_UTF8, input, len, unicodestr);

  bidi_types = new FriBidiCharType[len * sizeof(*bidi_types)];
  if (!bidi_types) {
    goto out;
  }

  fribidi_get_bidi_types(unicodestr, len, bidi_types);

  embedding_levels = new FriBidiLevel[len * sizeof(*embedding_levels)];
  if (!embedding_levels) {
    goto out;
  }

  if (!fribidi_get_par_embedding_levels(bidi_types, len, &direction, embedding_levels)) {
    goto out;
  }

  ar_props = new FriBidiArabicProp[len * sizeof(*ar_props)];
  if (!ar_props) {
    goto out;
  }

  fribidi_get_joining_types(unicodestr, len, ar_props);
  fribidi_join_arabic(bidi_types, len, embedding_levels, ar_props);
  fribidi_shape(flags, embedding_levels, len, ar_props, unicodestr);

  for (line_end = 0, line_start = 0; line_end < len; ++line_end) {
    if (IS_NEWLINE(unicodestr[line_end]) || line_end == len - 1) {
      if (!fribidi_reorder_line(flags, bidi_types, line_end - line_start + 1, line_start,
                                direction, embedding_levels, unicodestr, NULL))
      {
        goto out;
      }
      line_start = line_end + 1;
    }
  }

  /* Remove zero-width fill chars put in by libfribidi */
  for (i = 0, j = 0; i < len; ++i) {
    if (unicodestr[i] != FRIBIDI_CHAR_FILL) {
      unicodestr[j++] = unicodestr[i];
    }
  }
  len = j;

  /* Use len * 4, as a unicode character can be up to 4 bytes in UTF-8 */
  output = new char[len * 4 + 1];

  if (!output) {
    output = NULL;
    goto out;
  }

  //len =
  fribidi_unicode_to_charset(FRIBIDI_CHAR_SET_UTF8, unicodestr, len, output);

out:
  DEL(unicodestr)
  DEL(bidi_types)
  DEL(embedding_levels)
  DEL(ar_props)

  return output;
}

