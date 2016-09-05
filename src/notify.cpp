/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, djcj <djcj@gmx.de>
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

#include <iostream>  /* std::cerr, std::endl */
#include <string>    /* std::string, c_str */
#include <stdlib.h>  /* atoi */

#ifdef DYNAMIC_NOTIFY
#  include <dlfcn.h>  /* dlopen, dlerror, dlsym, dlclose */
#endif

#include <libnotify/notify.h>


int dialog_notify(const char *appname,
                  const char *notify_timeout,
                  const char *notify_msg,
                  const char *notify_title,
                  std::string notify_icon)
{
  if (notify_msg == NULL) {
    notify_msg = "This is a notification message";
  }
  if (notify_title == NULL) {
    notify_title = "Notification";
  }

  int timeout = -1;
  if (notify_timeout != NULL) {
    timeout = atoi(notify_timeout);
  }
  if (timeout <= 1 || timeout > 30) {
    timeout = 5;
  }

  const char *notify_icon_c = notify_icon.c_str();
  NotifyNotification *n;

#ifdef DYNAMIC_NOTIFY
  /* dlopen() libnotify */

  void *handle;

  gboolean (*notify_init_t)(const char*);
  gboolean (*notify_is_initted_t)(void);
  NotifyNotification* (*notify_notification_new_t)(const char*, const char*, const char*);
  void (*notify_notification_set_timeout_t)(NotifyNotification*, gint);
  gboolean (*notify_notification_show_t)(NotifyNotification*, GError**);

  char *error;

  handle = dlopen("libnotify.so.4", RTLD_LAZY);
  if (!handle) {
    std::cerr << "error: cannot open library: "
      << dlerror() << std::endl;
    return 1;
  }
  dlerror();

  *(void **)(&notify_init_t) =
    dlsym(handle, "notify_init");

  *(void **)(&notify_is_initted_t) =
    dlsym(handle, "notify_is_initted");

  *(void **)(&notify_notification_new_t) =
    dlsym(handle, "notify_notification_new");

  *(void **)(&notify_notification_set_timeout_t) =
    dlsym(handle, "notify_notification_set_timeout");

  *(void **)(&notify_notification_show_t) =
    dlsym(handle, "notify_notification_show");

  if ((error = dlerror()) != NULL)  {
    std::cerr << "error: cannot load symbol: " << error << std::endl;
    return 1;
  }

#else  /* DYNAMIC_NOTIFY */

/* use shared or static library symbols */
# define notify_init_t notify_init
# define notify_is_initted_t notify_is_initted
# define notify_notification_new_t notify_notification_new
# define notify_notification_set_timeout_t notify_notification_set_timeout
# define notify_notification_show_t notify_notification_show

#endif  /* DYNAMIC_NOTIFY */

  (*notify_init_t)(appname);
  if (!(*notify_is_initted_t)()) {
    std::cerr << "error: notify_init()" << std::endl;
    return 1;
  }

  n = (*notify_notification_new_t)(notify_title, notify_msg, notify_icon_c);
  (*notify_notification_set_timeout_t)(n, timeout*1000);

  if (!(*notify_notification_show_t)(n, NULL)) {
    std::cerr << "error: notify_notification_show()" << std::endl;
#ifdef DYNAMIC_NOTIFY
    dlclose(handle);
#endif
    return 1;
  }

#ifdef DYNAMIC_NOTIFY
  dlclose(handle);
#endif
  return 0;
}

