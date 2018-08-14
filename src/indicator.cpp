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

#include <FL/Fl.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/filename.H>

#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "indicator.h"
#include "default_tray_icon_png.h"

/* I could use libpng since FLTK is built against it anyway,
 * but I guess this function is easier to use and it's very small */
#define SVPNG_LINKAGE  static inline
#define SVPNG_OUTPUT   std::ostream &ofs
#define SVPNG_PUT(x)   ofs.put(x)
#include "svpng.h"

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

/* Is it safe to always assume a 22 pixels tray hight? */
static const int tray_icon_limit = 22;

static void *libgtk_handle, *libappindicator_handle;
static char *error;
static const char *command = NULL;

static void callback(void);
static void close_cb(void);

static GtkActionEntry entries[] = {
  { "Launch", NULL, "_Launch", NULL, NULL, callback },
  { "Close",  NULL, "_Close",  NULL, NULL, close_cb },
};
static const guint n_entries = 2;

static const gchar *ui_info =
  "<ui>"
    "<popup name='IndicatorPopup'>"
      "<menuitem action='Launch' />"
      "<menuitem action='Close' />"
    "</popup>"
  "</ui>";

static void callback(void) {
  execl("/bin/sh", "sh", "-c", command, NULL);
  _exit(127);
}

static void close_cb(void) {
  if (libgtk_handle) {
    gtk_main_quit();
  }
}

static void handle_dlopen_error(void)
{
  if (error) {
    std::cerr << error << std::endl;
  }

  if (libgtk_handle) {
    dlclose(libgtk_handle);
  }

  if (libappindicator_handle) {
    dlclose(libappindicator_handle);
  }
}

static bool create_tray_entry(const char *icon)
{
  GtkWidget *window, *indicator_menu;
  GtkActionGroup *action_group;
  GtkUIManager *uim;
  AppIndicator *indicator;
  void *handle;

  /* dlopen() libraries */

  libappindicator_handle = dlopen("libappindicator.so.1", RTLD_LAZY);
  error = dlerror();
  if (!libappindicator_handle) {
    handle_dlopen_error();
    return false;
  }

  libgtk_handle = dlopen("libgtk-x11-2.0.so.0", RTLD_LAZY);
  error = dlerror();
  if (!libgtk_handle) {
    handle_dlopen_error();
    return false;
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

static bool convert_icon(const char *in, std::string &out)
{
  Fl_Image *rgb, *img;
  unsigned char dummy[1] = {0};

  /* Should I write a separate function to create only a
   * temporary filename without creating the actual file? */
  if (!in || access(in, R_OK) != 0 || fl_filename_isdir(in) || !save_to_temp(dummy, 1, out)) {
    return false;
  }

  if ((img = img_to_rgb(in, false)) != NULL) {
    int img_w = img->w();
    int img_h = img->h();

    if (img_w > tray_icon_limit || img_h > tray_icon_limit) {
      /* resize image */
      aspect_ratio_scale(img_w, img_h, tray_icon_limit);
      rgb = img->copy(img_w, img_h);
      delete img;
    } else {
      rgb = img;
    }

    std::ofstream ofs(out, std::ios::out|std::ios::binary);
    if (ofs) {
      svpng(ofs, img_w, img_h, reinterpret_cast<const unsigned char*>(*rgb->data()), 1);
      ofs.close();
    }
    delete rgb;
  }

  return true;
}

int dialog_indicator(const char *command_, const char *icon)
{
  std::string tmp;
  const char *trayicon = NULL;
  bool default_icon = false;
  int ret = 0;

  command = command_;

  if (icon == NULL || strlen(icon) == 0) {
    default_icon = true;
  } else if (!convert_icon(icon, tmp)) {
    std::cerr << "error: converting icon failed; falling back to default icon" << std::endl;
    default_icon = true;
  }

  if (default_icon) {
    save_to_temp(src_default_tray_icon_png, src_default_tray_icon_png_len, tmp);
  }

  if (!tmp.empty()) {
    trayicon = tmp.c_str();
  }

  Fl::run();

  if (!create_tray_entry(trayicon)) {
    std::cerr << "error: cannot create tray/indicator entry" << std::endl;
    ret = 1;
  }

  if (!tmp.empty()) {
    unlink(tmp.c_str());
  }

  return ret;
}

