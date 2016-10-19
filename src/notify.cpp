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

#include <iostream>
#include <string>
#ifdef DYNAMIC_NOTIFY
#  include <dlfcn.h>
#endif
#include <libnotify/notify.h>

#include "fltk-dialog.hpp"


int dialog_notify(const char *appname,
                         int  timeout,
                  const char *notify_icon)
{
  if (msg == NULL)
  {
    msg = "No message";
  }

  if (title == NULL)
  {
    title = "No title";
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

  gboolean            (*notify_init)                     (const char*);
  gboolean            (*notify_is_initted)               (void);
  NotifyNotification* (*notify_notification_new)         (const char*, const char*, const char*);
  void                (*notify_notification_set_timeout) (NotifyNotification*, gint);
  gboolean            (*notify_notification_show)        (NotifyNotification*, GError**);
  void                (*notify_uninit)                   (void);

  void *handle = dlopen("libnotify.so.4", RTLD_LAZY);
  const char *dlsym_error = dlerror();

  if (!handle)
  {
    std::cerr << dlsym_error << std::endl;
    return 1;
  }

  dlerror();  /* clear/reset errors */

#  define STRINGIFY(x) #x
#  define LOAD_SYMBOL(x) \
  *(void **)(&x) = dlsym(handle, STRINGIFY(x)); \
  dlsym_error = dlerror(); \
  if (dlsym_error) \
  { \
    std::cerr << "error: cannot load symbol\n" << dlsym_error \
      << std::endl; \
    return 1; \
  } \
  dlerror()

  LOAD_SYMBOL(notify_init);
  LOAD_SYMBOL(notify_is_initted);
  LOAD_SYMBOL(notify_notification_new);
  LOAD_SYMBOL(notify_notification_set_timeout);
  LOAD_SYMBOL(notify_notification_show);
  LOAD_SYMBOL(notify_uninit);

#  define DLCLOSE_NOTIFY dlclose(handle)

#endif  /* DYNAMIC_NOTIFY */

#ifndef DLCLOSE_NOTIFY
#  define DLCLOSE_NOTIFY
#endif

  notify_init(appname);

  if (!notify_is_initted())
  {
    std::cerr << "error: notify_init()" << std::endl;
    DLCLOSE_NOTIFY;
    return 1;
  }

  NotifyNotification *n = notify_notification_new(title, msg, notify_icon);
  notify_notification_set_timeout(n, timeout * 1000);

  if (!notify_notification_show(n, NULL))
  {
    std::cerr << "error: notify_notification_show()" << std::endl;
    notify_uninit();
    DLCLOSE_NOTIFY;
    return 1;
  }

  notify_uninit();
  DLCLOSE_NOTIFY;
  return 0;
}

