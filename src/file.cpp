/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020, djcj <djcj@gmx.de>
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
#include <unistd.h>
#ifdef USE_DLOPEN
# include <dlfcn.h>
#endif

#include "fltk-dialog.hpp"
#include "icon_png.h"

static int file_chooser_fltk(int mode, bool classic);
static bool check_devices = false;


#ifdef USE_DLOPEN

typedef struct _GtkWidget GtkWidget;
typedef struct _GtkFileFilterInfo GtkFileFilterInfo;
typedef int gboolean;

/* from Fl_Native_File_Chooser_GTK.cxx */
class Fl_GTK_Native_File_Chooser_Driver : public Fl_Native_File_Chooser_FLTK_Driver
{
  friend class Fl_Native_File_Chooser;
  friend class My_GTK_File_Chooser;

private:
  static int have_looked_for_GTK_libs;

  struct pair {
    Fl_GTK_Native_File_Chooser_Driver *running;
    const char *filter;
    pair(Fl_GTK_Native_File_Chooser_Driver *c, const char *f) {
      running = c;
      filter = strdup(f);
    };
    ~pair() {
      free(const_cast<char *>(filter));
    };
  };

  GtkWidget *gtkw_ptr;
  void *gtkw_slist;
  unsigned gtkw_count;
  mutable char *gtkw_filename;
  char *gtkw_title;
  const char *previous_filter;

  int fl_gtk_chooser_wrapper();
  Fl_GTK_Native_File_Chooser_Driver(int val);
  virtual ~Fl_GTK_Native_File_Chooser_Driver();
  static int did_find_GTK_libs;
  static void probe_for_GTK_libs(void);
  virtual void type(int);
  virtual int count() const;
  virtual const char *filename() const;
  virtual const char *filename(int i) const;
  virtual void title(const char *);
  virtual const char *title() const;
  virtual int show();
  void changed_output_type(const char *filter);

  static int custom_gtk_filter_function(const GtkFileFilterInfo*, Fl_GTK_Native_File_Chooser_Driver::pair*);
  static void free_pair(pair *p);
  Fl_Preferences gtk_chooser_prefs;

public:
  static gboolean want_preview;
};

class My_GTK_File_Chooser
{
  Fl_GTK_Native_File_Chooser_Driver *_fc;

public:
  My_GTK_File_Chooser(int type, const char *label);

  ~My_GTK_File_Chooser() {
    if (_fc) { delete _fc; }
  }

  int did_find_GTK_libs() {
    return Fl_GTK_Native_File_Chooser_Driver::did_find_GTK_libs;
  }

  int show() {
    return _fc ? _fc->show() : -1;
  }

  const char *filename() {
    return _fc ? _fc->filename() : NULL;
  }
};

My_GTK_File_Chooser::My_GTK_File_Chooser(int type, const char *label)
{
  _fc = NULL;

  if (Fl_GTK_Native_File_Chooser_Driver::did_find_GTK_libs == 0) {
    Fl_GTK_Native_File_Chooser_Driver::probe_for_GTK_libs();
  }

  if (Fl_GTK_Native_File_Chooser_Driver::did_find_GTK_libs != 0) {
    _fc = new Fl_GTK_Native_File_Chooser_Driver(type);
    _fc->title(label);
  }
}

static int native_file_chooser_gtk(int mode)
{
  int type = (mode == DIR_CHOOSER) ? Fl_Native_File_Chooser::BROWSE_DIRECTORY : Fl_Native_File_Chooser::BROWSE_FILE;
  My_GTK_File_Chooser *fc = new My_GTK_File_Chooser(type, title);

  if (!fc || fc->did_find_GTK_libs() == 0) {
    return -1;
  }

  if (fc->show() == 0) {
    std::cout << quote << fc->filename() << quote << std::endl;
    return 0;
  }
  return 1;
}

static int native_file_chooser_gtk_with_fallback(int mode, bool classic)
{
  int rv = native_file_chooser_gtk(mode);

  if (rv == -1) {
    std::cerr << "warning: falling back to fltk" << std::endl;
    rv = file_chooser_fltk(mode, classic);
  }

  return rv;
}

#ifdef HAVE_QT
static int dlopen_getfilenameqt(int mode)
{
  std::string plugin;
  void *handle = NULL, *p;

  PROTO(int, getfilenameqt, (int, const char*, const char*, bool, bool))
  p = dlopen_qtplugin(plugin, handle, "getfilenameqt");

  if (!handle) {
    return -1;
  }

  getfilenameqt = reinterpret_cast<getfilenameqt_t>(p);
  int rv = getfilenameqt(mode, quote, title, false, false);
  dlclose(handle);
#ifndef USE_EXTERNAL_PLUGINS
  unlink(plugin.c_str());
#endif

  return rv;
}
#endif  /* HAVE_QT */

static int native_file_chooser(int mode, bool classic)
{
  int rv = -1;

#ifdef HAVE_QT
  if (getenv("KDE_FULL_SESSION") && (rv = dlopen_getfilenameqt(mode)) == -1) {
    std::cerr << "warning: falling back to gtk" << std::endl;
  }
#endif  /* HAVE_QT */

  if (rv == -1) {
    rv = native_file_chooser_gtk(mode);
  }

  if (rv == -1) {
    std::cerr << "warning: falling back to fltk" << std::endl;
    rv = file_chooser_fltk(mode, classic);
  }

  return rv;
}

#ifdef HAVE_QT
/* the Qt equivalent to Fl_Native_File_Chooser() */
static int native_file_chooser_qt(int mode, bool classic)
{
  int rv = dlopen_getfilenameqt(mode);

  if (rv == -1) {
    std::cerr << "warning: falling back to fltk" << std::endl;
    rv = file_chooser_fltk(mode, classic);
  }
  return rv;
}
#endif  /* HAVE_QT */
#endif  /* USE_DLOPEN */

static int file_chooser_fltk(int mode, bool classic)
{
  Fl_File_Chooser *fc = NULL;
  char *file = NULL;
  int type;

  if (classic) {
    type = (mode == DIR_CHOOSER) ? Fl_File_Chooser::DIRECTORY : Fl_File_Chooser::SINGLE;
    fl_ok = "OK";
    fl_cancel = "Cancel";

    fc = new Fl_File_Chooser(NULL, NULL, type, title);
    fc->show();
    Fl::run();

    if (fc->value() && strlen(fc->value()) > 0) {
      file = strdup(fc->value());
    }
  } else {
    file = file_chooser(mode, check_devices);
  }

  if (file) {
    std::cout << quote << file << quote << std::endl;
    free(file);
    return 0;
  }
  return 1;
}

int dialog_file_chooser(int mode, int native, bool classic, bool _check_devices)
{
  if (!title) {
    title = (mode == DIR_CHOOSER) ? "Select a directory" : "Select a file";
  }

  check_devices = _check_devices;

#ifdef USE_DLOPEN
  /* Note: setting an icon doesn't work on the Qt file chooser */
  switch (native) {
    case NATIVE_ANY:
      return native_file_chooser(mode, classic);
    case NATIVE_GTK:
      return native_file_chooser_gtk_with_fallback(mode, classic);
#ifdef HAVE_QT
    case NATIVE_QT:
      return native_file_chooser_qt(mode, classic);
#endif  /* HAVE_QT */
  }
#else
  (void)native;
#endif  /* USE_DLOPEN */

  return file_chooser_fltk(mode, classic);
}

