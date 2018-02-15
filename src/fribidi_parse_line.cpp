#include <string>
#include <fribidi.h>
#include <stdlib.h>
#include <string.h>

/***
main_CXXFLAGS += $(shell pkg-config --cflags fribidi)
main_LIBS     += $(shell pkg-config --libs fribidi)
***/

/* leaves string untouched on error */
void fribidi_parse_line(std::string &text)
{
  FriBidiParType base = FRIBIDI_PAR_LTR;
  FriBidiStrIndex len, inlen, *ltov = NULL, *vtol = NULL;
  FriBidiChar *logical, *visual;
  FriBidiLevel *levels = NULL;
  FriBidiCharSet charset;
  const char *instring;

  if (text.empty() || text == "") {
    return;
  }

  instring = text.c_str();

  if ((inlen = strlen(instring)) == 0) {
    return;
  }

  char temp[inlen+1] = {0};

  charset = fribidi_parse_charset("UTF-8");
  fribidi_set_mirroring(true);
  fribidi_set_reorder_nsm(false);

  strcat(temp, instring);
  temp[sizeof(temp) - 1] = '\0';
  len = strlen(temp);

  logical = (FriBidiChar *)malloc(sizeof(FriBidiChar) * (len + 1));
  len = fribidi_charset_to_unicode(charset, temp, len, logical);

  if (len == 0) {
    if (logical) {
      free(logical);
    }
    return;
  }

  visual = (FriBidiChar *)malloc(sizeof(FriBidiChar) * (len + 1));

  if (fribidi_log2vis(logical, len, &base, visual, ltov, vtol, levels)) {
    len = fribidi_remove_bidi_marks(visual, len, ltov, vtol, levels);
    if (len > 0) {  // len != -1
      memset(temp, '\0', sizeof(temp));
      fribidi_unicode_to_charset(charset, visual, len, temp);
      text = std::string(temp);
    }
  }

  if (logical) {
    free(logical);
  }
  if (visual) {
    free(visual);
  }

  return;
}

