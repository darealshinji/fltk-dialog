/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018, djcj <djcj@gmx.de>
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

// pipe=$(mktemp -u)
// mkfifo $pipe
// exec 3<> $pipe
// echo "ICON:newIcon" >&3

#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "icon_png.h"
#include "indicator_gtk.h"

PROTO( void,             gtk_init,                            (int*, char***) )
PROTO( GtkWidget*,       gtk_window_new,                      (GtkWindowType) )
PROTO( GtkActionGroup*,  gtk_action_group_new,                (const gchar*) )
PROTO( void,             gtk_action_group_add_actions,        (GtkActionGroup*, const GtkActionEntry*, guint, gpointer) )
PROTO( GtkUIManager*,    gtk_ui_manager_new,                  (void) )
PROTO( void,             gtk_ui_manager_insert_action_group,  (GtkUIManager*, GtkActionGroup*, gint) )
PROTO( guint,            gtk_ui_manager_add_ui_from_string,   (GtkUIManager*, const gchar*, gssize, GError**) )
PROTO( GtkWidget*,       gtk_ui_manager_get_widget,           (GtkUIManager*, const gchar*) )
PROTO( void,             gtk_main,                            (void) )
PROTO( void,             gtk_main_quit,                       (void) )
PROTO( void,             g_object_set_data,                   (GObject*, const gchar*, gpointer) )
PROTO( AppIndicator*,    app_indicator_new,                   (const gchar*, const gchar*, AppIndicatorCategory) )
PROTO( void,             app_indicator_set_status,            (AppIndicator*, AppIndicatorStatus) )
PROTO( void,             app_indicator_set_attention_icon,    (AppIndicator*, const gchar*) )
PROTO( void,             app_indicator_set_menu,              (AppIndicator*, GtkMenu*) )
PROTO( void,             app_indicator_set_icon,              (AppIndicator*, const gchar*) )
PROTO( void,             app_indicator_set_icon_theme_path,   (AppIndicator*, const gchar*) )

static std::string out;
static const char *command = NULL;
static bool listen, auto_close;
static void *libgtk_handle, *libappindicator_handle;
static char *error;
static pthread_t t1;

static void close_cb(void)
{
  if (listen) {
    pthread_cancel(t1);
  }
  gtk_main_quit();
  dlclose(libappindicator_handle);
  dlclose(libgtk_handle);
}

static void callback(void)
{
  if (command && strlen(command) > 0) {
    if (auto_close) {
      if (!out.empty()) {
        unlink(out.c_str());
      }
      close_cb();
      execl("/bin/sh", "sh", "-c", command, NULL);
      _exit(127);
    } else {
      int i = system(command);
      static_cast<void>(i);
    }
  }
}

static void handle_dlopen_error(void)
{
  if (error) {
    std::cerr << error << std::endl;
  }

  if (libappindicator_handle) {
    dlclose(libappindicator_handle);
  }

  if (libgtk_handle) {
    dlclose(libgtk_handle);
  }
}

static void set_icon(const char *icon, AppIndicator *indicator)
{
  char *resolved_path;

  if (!icon) {
    return;
  }

  unlink(out.c_str());

  if ((resolved_path = realpath(icon, NULL)) != NULL) {
    std::string in = resolved_path;
    free(resolved_path);

    out = "/tmp/indicator-" + get_random() + in.substr(in.rfind('/') + 1);

    if (symlink(in.c_str(), out.c_str()) == 0) {
      in = out.substr(5, out.rfind('.') - 5);
      app_indicator_set_icon (indicator, in.c_str());
      app_indicator_set_icon_theme_path (indicator, "/tmp");
    }
  } else {
    app_indicator_set_icon (indicator, icon);
  }
}

extern "C" void *getline_gtk(void *v)
{
  std::string line;

  while (true) {
    if (std::getline(std::cin, line)) {
      if (strcasecmp(line.c_str(), "quit") == 0) {
        gtk_main_quit();
        dlclose(libappindicator_handle);
        dlclose(libgtk_handle);
        return nullptr;
      } else if (line.length() > 5 && strcasecmp(line.substr(0,5).c_str(), "icon:") == 0) {
        set_icon(line.substr(5).c_str(), reinterpret_cast<AppIndicator *>(v));
      }
      usleep(300000);  /* 300ms */
    }
  }
  return nullptr;
}

int start_indicator_gtk(const char *command_, const char *icon, bool listen_, bool auto_close_)
{
  GtkWidget *window, *indicator_menu;
  GtkActionGroup *action_group;
  GtkUIManager *uim;
  AppIndicator *indicator;
  const char *tooltip = NULL;
  void *handle;
  std::string tt, tmp;

  command = command_;
  listen = listen_;
  auto_close = auto_close_;

  if (command) {
    if (strlen(command) > 36) {
      tt = command;
      tt.erase(31);
      tt += "[...]";
      tooltip = tt.c_str();
    } else {
      tooltip = command;
    }
  }

  GtkActionEntry entries[] = {
    { "Run command", NULL, "_Run command", NULL, tooltip, callback },
    { "Quit",        NULL, "_Quit",        NULL, NULL,    close_cb },
  };
  const guint n_entries = 2;

  const gchar *ui_info =
    "<ui>"
      "<popup name='IndicatorPopup'>"
        "<menuitem action='Run command' />"
        "<menuitem action='Quit' />"
      "</popup>"
    "</ui>";

  /* dlopen() libraries */

  libgtk_handle = dlopen("libgtk-x11-2.0.so.0", RTLD_LAZY|RTLD_GLOBAL);
  error = dlerror();
  if (!libgtk_handle) {
    handle_dlopen_error();
    return 1;
  }

  /* RTLD_NODELETE is needed to prevent memory access violation on dlclose() */
  libappindicator_handle = dlopen("libappindicator3.so.1", RTLD_LAZY|RTLD_NODELETE);
  error = dlerror();

  if (!libappindicator_handle) {
    if (error) {
      std::cerr << error << std::endl;
    }

    libappindicator_handle = dlopen("libappindicator.so.1", RTLD_LAZY|RTLD_NODELETE);
    error = dlerror();

    if (!libappindicator_handle) {
      handle_dlopen_error();
      return 1;
    }
  }

  dlerror();

  /* load symbols */

# define LOAD_SYMBOL(func) \
  func = reinterpret_cast<func##_t>(dlsym(handle, STRINGIFY(func))); \
  if ((error = dlerror()) != NULL) { \
    handle_dlopen_error(); \
    return 1; \
  }

  handle = libgtk_handle;
  LOAD_SYMBOL(gtk_init)
  LOAD_SYMBOL(gtk_window_new)
  LOAD_SYMBOL(gtk_action_group_new)
  LOAD_SYMBOL(gtk_action_group_add_actions)
  LOAD_SYMBOL(gtk_ui_manager_new)
  LOAD_SYMBOL(gtk_ui_manager_insert_action_group)
  LOAD_SYMBOL(gtk_ui_manager_add_ui_from_string)
  LOAD_SYMBOL(gtk_ui_manager_get_widget)
  LOAD_SYMBOL(gtk_main)
  LOAD_SYMBOL(gtk_main_quit)
  LOAD_SYMBOL(g_object_set_data)

  handle = libappindicator_handle;
  LOAD_SYMBOL(app_indicator_new)
  LOAD_SYMBOL(app_indicator_set_status)
  LOAD_SYMBOL(app_indicator_set_attention_icon)
  LOAD_SYMBOL(app_indicator_set_menu)
  LOAD_SYMBOL(app_indicator_set_icon)
  LOAD_SYMBOL(app_indicator_set_icon_theme_path)

# undef LOAD_SYMBOL

  /* create Gtk menu */

  gtk_init(NULL, NULL);
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  action_group = gtk_action_group_new ("AppActions");
  gtk_action_group_add_actions (action_group, entries, n_entries, window);

  uim = gtk_ui_manager_new();
  g_object_set_data (reinterpret_cast<GObject *>(window), "ui-manager", uim);
  gtk_ui_manager_insert_action_group (uim, action_group, 0);

  if (!gtk_ui_manager_add_ui_from_string (uim, ui_info, -1, NULL)) {
    dlclose(libgtk_handle);
    dlclose(libappindicator_handle);
    return 1;
  }

  /* create indicator */

  indicator = app_indicator_new ("fltk-dialog", "", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
  indicator_menu = gtk_ui_manager_get_widget (uim, "/ui/IndicatorPopup");
  app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);
  app_indicator_set_menu (indicator, reinterpret_cast<GtkMenu *>(indicator_menu));

  if (icon && strlen(icon) > 0) {
    set_icon(icon, indicator);
  } else if (save_to_temp(src_icon_png, src_icon_png_len, ".png", out)) {
    app_indicator_set_icon (indicator, out.substr(5, out.length() - 9).c_str());
    app_indicator_set_icon_theme_path (indicator, "/tmp");
  }

  if (listen) {
    pthread_create(&t1, 0, &getline_gtk, indicator);
  }

  gtk_main();

  if (!out.empty()) {
    unlink(out.c_str());
  }

  return 0;
}

