#ifndef NOTIFY_MINI_HPP
#define NOTIFY_MINI_HPP
#include "glib_mini.hpp"
typedef struct _NotifyNotification NotifyNotification;
typedef struct _NotifyNotificationPrivate NotifyNotificationPrivate;
struct _NotifyNotification {
  GObject parent_object;
  NotifyNotificationPrivate *priv;
};
gboolean notify_init(const char *app_name);
void notify_uninit(void);
gboolean notify_is_initted(void);
NotifyNotification *notify_notification_new(const char *summary, const char *body, const char *icon);
gboolean notify_notification_show(NotifyNotification *notification, GError **error);
void notify_notification_set_timeout(NotifyNotification *notification, gint timeout);
#endif  /* !NOTIFY_MINI_HPP */
