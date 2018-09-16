/* Glib type definitions  */
typedef struct _GError GError;
typedef void * gpointer;
typedef int gint;
typedef gint gboolean;
typedef double gdouble;
typedef char gchar;
typedef unsigned char guint8;
typedef unsigned long gsize;
typedef void (*GDestroyNotify) (gpointer);

/* cairo type definitions */
typedef struct _cairo cairo_t;
typedef struct _cairo_surface cairo_surface_t;
typedef enum _cairo_status {
  CAIRO_STATUS_SUCCESS = 0,
  CAIRO_STATUS_NO_MEMORY,
  CAIRO_STATUS_INVALID_RESTORE,
  CAIRO_STATUS_INVALID_POP_GROUP,
  CAIRO_STATUS_NO_CURRENT_POINT,
  CAIRO_STATUS_INVALID_MATRIX,
  CAIRO_STATUS_INVALID_STATUS,
  CAIRO_STATUS_NULL_POINTER,
  CAIRO_STATUS_INVALID_STRING,
  CAIRO_STATUS_INVALID_PATH_DATA,
  CAIRO_STATUS_READ_ERROR,
  CAIRO_STATUS_WRITE_ERROR,
  CAIRO_STATUS_SURFACE_FINISHED,
  CAIRO_STATUS_SURFACE_TYPE_MISMATCH,
  CAIRO_STATUS_PATTERN_TYPE_MISMATCH,
  CAIRO_STATUS_INVALID_CONTENT,
  CAIRO_STATUS_INVALID_FORMAT,
  CAIRO_STATUS_INVALID_VISUAL,
  CAIRO_STATUS_FILE_NOT_FOUND,
  CAIRO_STATUS_INVALID_DASH,
  CAIRO_STATUS_INVALID_DSC_COMMENT
} cairo_status_t;
typedef enum _cairo_format {
  CAIRO_FORMAT_INVALID   = -1,
  CAIRO_FORMAT_ARGB32    = 0,
  CAIRO_FORMAT_RGB24     = 1,
  CAIRO_FORMAT_A8        = 2,
  CAIRO_FORMAT_A1        = 3,
  CAIRO_FORMAT_RGB16_565 = 4,
  CAIRO_FORMAT_RGB30     = 5
} cairo_format_t;
typedef cairo_status_t (*cairo_write_func_t) (void *, const unsigned char *, unsigned int);

/* librsvg type definitions */
typedef struct _RsvgHandle RsvgHandle;
typedef struct _RsvgDimensionData RsvgDimensionData;
struct _RsvgDimensionData {
  int width;
  int height;
  gdouble em;
  gdouble ex;
};
typedef void (*RsvgSizeFunc) (gint *, gint *, gpointer);

