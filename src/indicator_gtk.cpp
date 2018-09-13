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

#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "icon_png.h"
#include "indicator_gtk.h"

extern "C" int rgb_to_png(const char *file, const unsigned char *rgba, int w, int h);

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

static std::string out;
static const char *command = NULL;
static void *libgtk_handle, *libappindicator_handle;
static char *error;

static void callback(void) {
  if (command && strlen(command) > 0) {
    if (!out.empty()) {
      unlink(out.c_str());
    }
    execl("/bin/sh", "sh", "-c", command, NULL);
    _exit(127);
  }
}

static void close_cb_gtk(void) {
  gtk_main_quit();
  dlclose(libappindicator_handle);
  dlclose(libgtk_handle);
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

static bool convert_icon(const char *in, bool force_nanosvg)
{
  Fl_Image *rgb_, *img;
  char *path;
  const unsigned char *rgba;
  int img_w, img_h, limit;
  int x, y, w, work_h;
  bool rv;

  if (!in || access(in, R_OK) != 0 || fl_filename_isdir(in) ||
      !(img = img_to_rgb(in, force_nanosvg)))
  {
    return false;
  }

  Fl::screen_xywh(x, y, w, limit);
  work_h = Fl::h();
  img_w = img->w();
  img_h = img->h();

  /* guess tray icon height */
  if (limit > work_h) {
    /* 72% of tray height */
    limit = (limit - work_h) * 0.72;
    if (limit < 4) {
      limit = 16;
    } else if (limit > 64) {
      limit = 64;
    }
  } else {
    limit = 16;
  }

  if (img_w > limit || img_h > limit) {
    /* resize image */
    aspect_ratio_scale(img_w, img_h, limit);
    rgb_ = img->copy(img_w, img_h);
    delete img;
  } else {
    rgb_ = img;
  }

  /* create 0 byte file */
  path = save_to_temp(NULL, 0);
  if (!path) {
    delete rgb_;
    return false;
  }
  out = std::string(path);

  rgba = reinterpret_cast<const unsigned char *>(*rgb_->data());
  rv = (rgb_to_png(out.c_str(), rgba, img_w, img_h) == 0);

  free(path);
  delete rgb_;
  return rv;
}

static bool create_tray_entry_gtk(const char *icon)
{
  GtkWidget *window, *indicator_menu;
  GtkActionGroup *action_group;
  GtkUIManager *uim;
  AppIndicator *indicator;
  std::string s;
  const char *tooltip = NULL;
  void *handle;

  if (command) {
    const int limit = 36;
    if (strlen(command) > limit) {
      s = command;
      s.erase(limit - 5);
      s += "[...]";
      tooltip = s.c_str();
    } else {
      tooltip = command;
    }
  }

  GtkActionEntry entries[] = {
    { "Run command", NULL, "_Run command", NULL, tooltip, callback     },
    { "Quit",        NULL, "_Quit",        NULL, NULL,    close_cb_gtk },
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
    return false;
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
      return false;
    }
  }

  dlerror();

  /* load symbols */

# define LOAD_SYMBOL(func) \
  func = reinterpret_cast<func##_t>(dlsym(handle, STRINGIFY(func))); \
  if ((error = dlerror()) != NULL) { \
    handle_dlopen_error(); \
    return false; \
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
    return false;
  }

  /* create indicator */

  indicator = app_indicator_new ("fltk-dialog", icon, APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
  indicator_menu = gtk_ui_manager_get_widget (uim, "/ui/IndicatorPopup");
  app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);
  app_indicator_set_attention_icon (indicator, "indicator-messages-new");
  app_indicator_set_menu (indicator, reinterpret_cast<GtkMenu *>(indicator_menu));

  gtk_main();

  return true;
}

bool start_indicator_gtk(const char *command_, const char *icon, bool force_nanosvg)
{
  std::string default_icon;
  bool ret = false;
  char *path;

  command = command_;

  if (convert_icon(icon, force_nanosvg)) {
    /* provided icon is okay */
    ret = create_tray_entry_gtk(out.c_str());
    unlink(out.c_str());
    return ret;
  }

  /* save default icon */
  path = save_to_temp(src_icon_png, src_icon_png_len);
  if (!path) {
    return create_tray_entry_gtk("");
  }

  default_icon = std::string(path);
  free(path);

  /* convert default icon */
  if (convert_icon(default_icon.c_str(), false)) {
    unlink(default_icon.c_str());
    default_icon.clear();
    ret = create_tray_entry_gtk(out.c_str());
  } else {
    ret = create_tray_entry_gtk("");
  }

  if (!default_icon.empty()) {
    unlink(default_icon.c_str());
  }
  if (!out.empty()) {
    unlink(out.c_str());
  }

  return ret;
}

