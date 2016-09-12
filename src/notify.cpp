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


int dialog_notify(const char  *appname,
                  const char  *notify_timeout,
                  const char  *notify_msg,
                  const char  *notify_title,
                  std::string  notify_icon)
{
  int timeout = 5;
  const char *notify_icon_c = notify_icon.c_str();
  NotifyNotification *n;

  if (notify_msg == NULL)
  {
    notify_msg = "This is a notification message";
  }

  if (notify_title == NULL)
  {
    notify_title = "Notification";
  }

  if (notify_timeout != NULL)
  {
    timeout = atoi(notify_timeout);
  }

  if (timeout < 1)
  {
    std::cerr << "error: timeout shorter than 1 second: " << timeout
      << std::endl;
    return 1;
  }

  if (timeout > 30)
  {
    std::cerr << "error: timeout longer than 30 seconds: " << timeout
      << std::endl;
    return 1;
  }

#ifdef DYNAMIC_NOTIFY
  /* dlopen() libnotify */

  void *handle;

  gboolean (*dl_notify_init)(const char*);
  gboolean (*dl_notify_is_initted)(void);
  NotifyNotification* (*dl_notify_notification_new)(const char*, const char*, const char*);
  void (*dl_notify_notification_set_timeout)(NotifyNotification*, gint);
  gboolean (*dl_notify_notification_show)(NotifyNotification*, GError**);
  void (*dl_notify_uninit)(void);

  char *error;

  handle = dlopen("libnotify.so.4", RTLD_LAZY);
  if (!handle)
  {
    std::cerr << "error: cannot open library: " << dlerror() << std::endl;
    return 1;
  }
  dlerror();

  *(void **)(&dl_notify_init) =
    dlsym(handle, "notify_init");

  *(void **)(&dl_notify_is_initted) =
    dlsym(handle, "notify_is_initted");

  *(void **)(&dl_notify_notification_new) =
    dlsym(handle, "notify_notification_new");

  *(void **)(&dl_notify_notification_set_timeout) =
    dlsym(handle, "notify_notification_set_timeout");

  *(void **)(&dl_notify_notification_show) =
    dlsym(handle, "notify_notification_show");

  *(void **)(&dl_notify_uninit) =
    dlsym(handle, "notify_uninit");

  if ((error = dlerror()) != NULL)
  {
    std::cerr << "error: cannot load symbol: " << error << std::endl;
    return 1;
  }

#else  /* DYNAMIC_NOTIFY */

/* use shared or static library symbols */
#  define dl_notify_init notify_init
#  define dl_notify_is_initted notify_is_initted
#  define dl_notify_notification_new notify_notification_new
#  define dl_notify_notification_set_timeout notify_notification_set_timeout
#  define dl_notify_notification_show notify_notification_show
#  define dl_notify_uninit notify_uninit

#endif  /* DYNAMIC_NOTIFY */

  (*dl_notify_init)(appname);

  if (!(*dl_notify_is_initted)())
  {
    std::cerr << "error: notify_init()" << std::endl;
#ifdef DYNAMIC_NOTIFY
    dlclose(handle);
#endif
    return 1;
  }

  n = (*dl_notify_notification_new)(notify_title, notify_msg, notify_icon_c);
  (*dl_notify_notification_set_timeout)(n, timeout*1000);

  if (!(*dl_notify_notification_show)(n, NULL))
  {
    std::cerr << "error: notify_notification_show()" << std::endl;
    (*dl_notify_uninit)();
#ifdef DYNAMIC_NOTIFY
    dlclose(handle);
#endif
    return 1;
  }

  (*dl_notify_uninit)();
#ifdef DYNAMIC_NOTIFY
  dlclose(handle);
#endif
  return 0;
}

