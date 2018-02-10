#ifndef NOTIFY_HPP
#define NOTIFY_HPP

/* GLIB type definitions */
typedef char gchar;
typedef int gint;
typedef gint gboolean;
typedef unsigned int guint;
typedef guint guint32;
typedef guint32 GQuark;
typedef unsigned long gulong;
typedef gulong GType;
typedef struct _GObject GObject;
typedef struct _GTypeInstance GTypeInstance;
typedef struct _GTypeClass GTypeClass;
typedef struct _GError GError;
typedef struct _GData GData;
struct _GTypeInstance { GTypeClass *g_class; };
struct _GTypeClass { GType g_type; };
struct _GObject {
  GTypeInstance g_type_instance;
  volatile guint ref_count;
  GData *qdata;
};
struct _GError {
  GQuark domain;
  gint code;
  gchar *message;
};

/* libnotify type definitions */
typedef struct _NotifyNotification NotifyNotification;
typedef struct _NotifyNotificationPrivate NotifyNotificationPrivate;
struct _NotifyNotification {
  GObject parent_object;
  NotifyNotificationPrivate *priv;
};

#endif  /* !NOTIFY_HPP */

