#ifndef APPINDICATOR_H
#define APPINDICATOR_H

/* Gtk type definitions */
typedef struct _GtkWidget GtkWidget;
typedef struct _GtkActionGroup GtkActionGroup;
typedef struct _GtkUIManager GtkUIManager;
typedef struct _GtkActionEntry GtkActionEntry;
typedef struct _GtkMenu GtkMenu;
typedef struct _GObject GObject;
typedef struct _GError GError;
typedef char gchar;
typedef int gint;
typedef unsigned int guint;
typedef void* gpointer;
typedef signed long gssize;
typedef void (*GCallback) (void);
typedef enum {
  GTK_WINDOW_TOPLEVEL,
  GTK_WINDOW_POPUP
} GtkWindowType;
struct _GtkActionEntry {
  const gchar *name;
  const gchar *stock_id;
  const gchar *label;
  const gchar *accelerator;
  const gchar *tooltip;
  GCallback callback;
};

/* libappindicator type definitions */
typedef struct _AppIndicator AppIndicator;
typedef enum {
	APP_INDICATOR_CATEGORY_APPLICATION_STATUS,
	APP_INDICATOR_CATEGORY_COMMUNICATIONS,
	APP_INDICATOR_CATEGORY_SYSTEM_SERVICES,
	APP_INDICATOR_CATEGORY_HARDWARE,
	APP_INDICATOR_CATEGORY_OTHER
} AppIndicatorCategory;
typedef enum {
	APP_INDICATOR_STATUS_PASSIVE,
	APP_INDICATOR_STATUS_ACTIVE,
	APP_INDICATOR_STATUS_ATTENTION
} AppIndicatorStatus;

#endif  /* !APPINDICATOR_H */

