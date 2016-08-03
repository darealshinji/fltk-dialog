/* dialog types */
enum dialogTypes {
  DIALOG_ABOUT,
  DIALOG_ALERT,
  DIALOG_HTML,
  DIALOG_FL_CALENDAR,
  DIALOG_FL_CHOICE,
  DIALOG_FL_COLOR,
  DIALOG_FL_DIR_CHOOSER,
  DIALOG_FL_FILE_CHOOSER,
  DIALOG_FL_INPUT,
  DIALOG_FL_MESSAGE,
  DIALOG_FL_PASSWORD,
  DIALOG_FL_PROGRESS,
  DIALOG_FL_VALUE_SLIDER
};

/* getopt long_options */
enum longOpts {
  LO_ABOUT,
  LO_ALT_LABEL,
  LO_AUTO_CLOSE,
  LO_CALENDAR,
  LO_COLOR,
  LO_DIRECTORY,
  LO_ENTRY,
  LO_FILE,
  LO_FORMAT,
  LO_HTML,
  LO_MAX_VALUE,
  LO_MIN_VALUE,
  LO_NATIVE,
  LO_NO_CLOSE,
  LO_NO_ESCAPE,
  LO_NO_LABEL,
  LO_PASSWORD,
  LO_PROGRESS,
  LO_QUESTION,
  LO_SCALE,
  LO_SCHEME,
  LO_STEP,
  LO_TEXT,
  LO_TITLE,
  LO_VALUE,
  LO_WARNING,
  LO_YES_LABEL
};

/* don't use fltk's '@' symbols */
static int use_symbols = 0;

/* djcj: I find this easier than using extra print-error functions */
#define P_ERR(x) \
 std::cerr << argv[0] << ": " << x << std::endl; \
 return 1
#define P_ERRX(x) \
 std::cerr << x << std::endl; \
 return 1

