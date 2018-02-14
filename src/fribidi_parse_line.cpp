#include <string>
#include <fribidi.h>
#include <stdlib.h>
#include <string.h>

// TODO: FLTK button width issues?

/***
main_CXXFLAGS += $(shell pkg-config --cflags fribidi)
main_LIBS     += $(shell pkg-config --libs fribidi)
***/

/* returns pointer to text.c_str() on success and NULL on error */
const char *fribidi_parse_line(std::string &text)
{
  FriBidiParType base = FRIBIDI_PAR_LTR;
  FriBidiStrIndex len, inlen, *ltov = NULL, *vtol = NULL;
  FriBidiChar *logical, *visual;
  FriBidiLevel *levels = NULL;
  FriBidiCharSet charset;
  const char *instring, *p = NULL;

  if (text.empty() || text == "") {
    return NULL;
  }

  instring = text.c_str();

  if ((inlen = strlen(instring)) == 0) {
    return NULL;
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
    return NULL;
  }

  visual = (FriBidiChar *)malloc(sizeof(FriBidiChar) * (len + 1));

  if (fribidi_log2vis(logical, len, &base, visual, ltov, vtol, levels)) {
    len = fribidi_remove_bidi_marks(visual, len, ltov, vtol, levels);
    if (len > 0) {  // len != -1
      memset(temp, '\0', sizeof(temp));
      fribidi_unicode_to_charset(charset, visual, len, temp);
      text = std::string(temp);
      p = text.c_str();
    }
  }

  if (logical) {
    free(logical);
  }
  if (visual) {
    free(visual);
  }

  return p;
}

