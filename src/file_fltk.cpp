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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fltk-dialog.hpp"


#ifdef DYNAMIC_MAGIC

# include <dlfcn.h>

# define MAGIC_PRESERVE_ATIME     0x0000080
# define MAGIC_ERROR              0x0000200
# define MAGIC_NO_CHECK_APPTYPE   0x0008000
# define MAGIC_NO_CHECK_COMPRESS  0x0001000
# define MAGIC_NO_CHECK_ELF       0x0010000
# define MAGIC_NO_CHECK_TAR       0x0002000

typedef struct magic_set *magic_t;

PROTO( magic_t, magic_open, (int) )
PROTO( int, magic_load, (magic_t, const char *) )
PROTO( const char *, magic_file, (magic_t, const char *) )
PROTO( void, magic_close, (magic_t) )

static void *handle = NULL;

#else

# include <magic.h>

#endif  /* DYNAMIC_MAGIC */

#include "octicons.h"

static Fl_Double_Window *win;
static Fl_Hold_Browser *br;
static Fl_Box *addrline, *infobox;
static Fl_Button *bt_up;
static Fl_Return_Button *bt_ok;
static Fl_Input *input;

static std::string current_dir = "/", home_dir = "/home";
static int selection = 0;
static bool show_dotfiles = false, list_files = true, sort_reverse = false;
static char *selected_file = NULL;

static void br_change_dir(void);
static void selection_timeout(void);
static Fl_Timeout_Handler th = reinterpret_cast<Fl_Timeout_Handler>(selection_timeout);

#define PNG(a,b)  static Fl_PNG_Image a(NULL, octicons_##b##_png, octicons_##b##_png_len);
PNG(eye, eye)
PNG(eye_closed, eye_closed)
PNG(go_up, arrow_up)
PNG(go_up_gray, arrow_up_gray)
PNG(icon_any, file)
PNG(icon_dir, file_directory)
PNG(icon_link_any, file_symlink_file)
PNG(icon_link_dir, file_symlink_directory)
PNG(sort_order1, list_ordered_1)
PNG(sort_order2, list_ordered_2)

/* Format is XDG_xxx_DIR="$HOME/yyy", where yyy is a shell-escaped
 * homedir-relative path, or XDG_xxx_DIR="/yyy", where /yyy is an
 * absolute path. No other format is supported.
 */
static bool xdg_user_dir_lookup_real(std::vector<std::string> &vec)
{
  std::ifstream ifs;
  std::string line;
  char *xdg_conf;
  const char *valid_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    //"abcdefghijklmnopqrstuvwxyz_"
    /**/;

  xdg_conf = getenv("XDG_CONFIG_HOME");

  if (xdg_conf && strlen(xdg_conf) > 0) {
    ifs.open(std::string(xdg_conf) + "/user-dirs.dirs", std::ios::in|std::ios::ate);
    if (!ifs.is_open()) {
      xdg_conf = NULL;
    }
  }

  if (!xdg_conf) {
    ifs.open(home_dir + "/.config/user-dirs.dirs", std::ios::in|std::ios::ate);
    if (!ifs.is_open()) {
      return false;
    }
  }

  if (ifs.tellg() > 1024*1024) {
    ifs.close();
    return false;
  }
  ifs.seekg(0, std::ios::beg);

  while (std::getline(ifs, line)) {
    size_t pos, pos2;

    if ((pos = line.find_first_not_of(" \t")) > 0) {
      /* remove preceding spaces and tabs */
      line.erase(0, pos);
    }

    if (line[0] == '#' || line.substr(0,4) != "XDG_" || (pos = line.find("_DIR=\"")) == std::string::npos) {
      /* ignore comment lines or anything else that's
       * not beginning with »XDG_*_DIR="« */
      continue;
    }

    if (line.substr(4, pos - 4).find_first_not_of(valid_chars) != std::string::npos) {
      /* »type« contains invalid characters */
      continue;
    }

    pos += 4;
    line.erase(pos, 2);  /* remove »="« */

    if ((pos2 = line.find('"')) == std::string::npos) {
      /* missing a closing quotation mark */
      continue;
    }
    line.erase(pos2);

    if (line.substr(pos, 6) == "$HOME/") {
      line.replace(pos, 5, home_dir);
    } else if (line[pos] != '/') {
      continue;
    }

    vec.push_back(line);
  }

  ifs.close();

  return vec.size() > 0;
}

static bool xdg_user_dir_lookup(std::vector<std::string> &vec)
{
  if (xdg_user_dir_lookup_real(vec)) {
    return true;
  }

  /* run xdg-user-dirs-update and try it again */
  int rv = system("/usr/bin/xdg-user-dirs-update 2>/dev/null >/dev/null");
  (void)rv;

  return xdg_user_dir_lookup_real(vec);
}

static std::string getfsize(double size)
{
  std::string str;
  const char *unit;
  char ch[32] = {0};
  const int KiBYTES = 1024;
  const int MiBYTES = 1024 * KiBYTES;
  const int GiBYTES = 1024 * MiBYTES;

  if (size > GiBYTES) {
    size /= GiBYTES;
    unit = " GiB";
  } else if (size > MiBYTES) {
    size /= MiBYTES;
    unit = " MiB";
  } else if (size > KiBYTES) {
    size /= KiBYTES;
    unit = " kiB";
  } else {
    snprintf(ch, sizeof(ch) - 1, "%d", static_cast<int>(size));
    str = std::string(ch) + " Bytes";
    return str;
  }

  snprintf(ch, sizeof(ch) - 1, "%.1f", size);
  ch[strlen(ch) - 2] = '.';  /* replace comma with dot */
  str = std::string(ch) + unit;
  return str;
}

static std::string get_filetype(const char *file)
{
  magic_t mcookie;
  const char *desc;
  std::string str = "";
  std::size_t pos;

  const int flags = MAGIC_PRESERVE_ATIME
    | MAGIC_ERROR
    | MAGIC_NO_CHECK_APPTYPE
    | MAGIC_NO_CHECK_COMPRESS
    | MAGIC_NO_CHECK_ELF
    | MAGIC_NO_CHECK_TAR;

#ifdef DYNAMIC_MAGIC
  if (!handle) {
    return "";
  }
#endif

  if ((mcookie = magic_open(flags)) == NULL) {
    return "";
  }

  if (magic_load(mcookie, NULL) != 0) {
    magic_close(mcookie);
    return "";
  }

  if ((desc = magic_file(mcookie, file)) != NULL) {
    str = std::string(desc);
    if ((pos = str.find_first_of(',')) != std::string::npos) {
      str.erase(pos);
    }
  }
  magic_close(mcookie);

  return str;
}

static void fileInfo(const char *file)
{
#ifdef DYNAMIC_MAGIC
  if (!handle) {
    return;
  } else
#endif
  if (!file || strlen(file) == 0) {
    infobox->label(NULL);
    return;
  } else if (br->icon(selection) == &icon_dir) {
    infobox->label("directory");
    return;
  }

  std::string str = get_filetype(file);

  if (str != "directory" && str != "") {
    const size_t len = 24;  /* strlen("broken symbolic link to ") */

    if (str.size() > len && str.substr(0, len) == "broken symbolic link to ") {
      /* get actual link size */
      str = getfsize(static_cast<double>(str.size() - len)) + ",  broken symbolic link";
    } else {
      /* get target filesize */
      struct stat st;
      stat(file, &st);
      if (br->icon(selection) != &icon_link_dir) {
        str = getfsize(static_cast<double>(st.st_size)) + ",  " + str;
      }
    }
  }
  infobox->copy_label(str.c_str());
}

static void up_callback(Fl_Widget *)
{
  if (current_dir != "/") {
    current_dir.erase(current_dir.find_last_of("/"));

    if (current_dir.empty()) {
      current_dir = "/";
    }

    br_change_dir();
  }
}

static void xdg_callback(Fl_Widget *, void *v) {
  if (v) {
    current_dir = reinterpret_cast<const char *>(v);
    br_change_dir();
  }
}

static void top_callback(Fl_Widget *) {
  current_dir = "/";
  br_change_dir();
}

static void home_callback(Fl_Widget *) {
  current_dir = home_dir;
  br_change_dir();
}

static void hidden_callback(Fl_Widget *o)
{
  Fl_Button *b = dynamic_cast<Fl_Button *>(o);

  if (show_dotfiles) {
    show_dotfiles = false;
    b->image(eye_closed);
  } else {
    show_dotfiles = true;
    b->image(eye);
  }

  br_change_dir();
}

static void sort_callback(Fl_Widget *o)
{
  Fl_Button *b = dynamic_cast<Fl_Button *>(o);

  if (sort_reverse) {
    sort_reverse = false;
    b->image(sort_order1);
  } else {
    sort_reverse = true;
    b->image(sort_order2);
  }

  br_change_dir();
}

static void selection_timeout(void) {
  selection = 0;
}

static void close_cb(Fl_Widget *, long l)
{
  if (l == 0) {  /* OK button pressed */
    int line = br->value();

    if (line > 0) {
      selected_file = reinterpret_cast<char *>(br->data(line));

      if (list_files && fl_filename_isdir(selected_file)) {
        /* don't return path but change directory */
        current_dir = std::string(selected_file);
        selected_file = NULL;
        br_change_dir();
        return;
      }
      br->data(line, NULL);
    } else {  /* nothing selected */
      if (list_files) {
        return;
      }
      selected_file = strdup(current_dir.c_str());
    }
  }

  for (int i = 0; i <= br->size(); ++i) {
    if (br->data(i)) {
      free(br->data(i));
    }
  }

  win->hide();

#ifdef DYNAMIC_MAGIC
  if (handle) {
    dlclose(handle);
  }
#endif
}

static void br_callback(Fl_Widget *)
{
  const char *fname = reinterpret_cast<const char *>(br->data(br->value()));

  /* some workaround to change directories on double-click */
  if (selection == 0) {
    selection = br->value();
    Fl::add_timeout(1.0, th);
  } else {
    Fl::remove_timeout(th);
    if (br->value() == selection) {
      selection = 0;

      if (fl_filename_isdir(fname)) {
        /* double-clicked on directory */
        if (access(fname, R_OK) == 0) {
          current_dir = std::string(fname);
          br_change_dir();
        }
      } else {
        /* double-clicked on file */
        close_cb(NULL, 0);
        return;
      }
    } else {
      selection = br->value();
      Fl::add_timeout(1.0, th);
    }
  }

  if (br->value() == 0) {
    input->value("");
    infobox->label(NULL);
  } else {
    fileInfo(fname);
    input->value(basename(fname));
  }
}

/* make sure to call setlocale(LC_ALL, "") at startup */
static bool ignorecaseSort(std::string s1, std::string s2)
{
  if (sort_reverse) {
    return (strcoll(s1.c_str(), s2.c_str()) > 0);
  }
  return (strcoll(s1.c_str(), s2.c_str()) < 0);
}

/* sort by basename */
static bool ignorecaseSortXDG(std::string s1, std::string s2) {
  return (strcoll(s1.c_str() + s1.rfind('/') + 1, s2.c_str() + s2.rfind('/') + 1) < 0);
}

static void br_change_dir(void)
{
  struct dirent **list;
  std::vector<std::string> vec, vec2;

  const char *white = "@B255@. ", *yellow = "@B17@. ";

  int n = fl_filename_list(current_dir.c_str(), &list);

  if (n > 0) {
    for (int i = 0; i < n; ++i) {
      size_t len = strlen(list[i]->d_name);

      if (len > 1 && list[i]->d_name[len-1] == '/') {
        /* remove '.' and '..' entries */
        if (strcmp(list[i]->d_name, "./") == 0 || strcmp(list[i]->d_name, "../") == 0) {
          free(list[i]);
          continue;
        }
        list[i]->d_name[len-1] = '\0'; /* remove trailing '/' */
      }
      vec.push_back(std::string(list[i]->d_name));
      free(list[i]);
    }
    free(list);
  }

  /* clear browser */
  for (int i = 0; i <= br->size(); ++i) {
    if (br->data(i)) {
      free(br->data(i));
    }
  }
  br->clear();

  selection = 0;

  if (vec.size() > 0) {
    std::sort(vec.begin(), vec.end(), ignorecaseSort);

    /* list directories */
    for (auto it = vec.begin(); it != vec.end(); ++it) {
      std::string &s = *it;
      if (s[0] != '.' || (s[0] == '.' && show_dotfiles)) {
        std::string path = current_dir;
        if (path != "/") {
          path.push_back('/');
        }
        path += s;

        if (fl_filename_isdir(path.c_str())) {
          std::string entry = (br->size() % 2 == 0) ? white : yellow;
          entry += s;
          br->add(entry.c_str(), reinterpret_cast<void *>(strdup(path.c_str())));

          struct stat st;
          lstat(path.c_str(), &st);

          if (S_ISLNK(st.st_mode)) {
            br->icon(br->size(), &icon_link_dir);
          } else {
            br->icon(br->size(), &icon_dir);
          }
        } else if (list_files) {
          vec2.push_back(s);
        }
      }
    }
    vec.clear();

    /* list files */
    if (list_files) {
      for (auto it = vec2.begin(); it != vec2.end(); ++it) {
        std::string &s = *it;
        if (s[0] != '.' || (s[0] == '.' && show_dotfiles)) {
          std::string path = current_dir;
          if (path != "/") {
            path.push_back('/');
          }
          path += s;

          if (!fl_filename_isdir(path.c_str())) {
            std::string entry = (br->size() % 2 == 0) ? white : yellow;
            entry += s;
            br->add(entry.c_str(), reinterpret_cast<void *>(strdup(path.c_str())));

            struct stat st;
            lstat(path.c_str(), &st);

            if (S_ISLNK(st.st_mode)) {
              br->icon(br->size(), &icon_link_any);
            } else {
              br->icon(br->size(), &icon_any);
            }
          }
        }
      }
      vec2.clear();
    }
  }

  std::string s = " " + current_dir;
  addrline->copy_label(s.c_str());

  if (current_dir == "/" && bt_up->active()) {
    bt_up->deactivate();
    bt_up->image(go_up_gray);
  } else if (current_dir != "/" && !bt_up->active()) {
    bt_up->activate();
    bt_up->image(go_up);
  }

  input->value("");
  infobox->label(NULL);
}

char *file_chooser(int mode)
{
  Fl_Button *b = NULL, *bt_cancel;
  Fl_Group *g, *g_top, *g_main, *g_main_left, *g_bottom, *g_bottom_inside;
  Fl_Box *dummy;
  std::vector<std::string> vec, vec2;
  const int w = 800, h = 600;
  char *env;

  const char *xdg_types[] = {
    "XDG_DESKTOP_DIR",
    "XDG_DOCUMENTS_DIR",
    "XDG_DOWNLOAD_DIR",
    "XDG_MUSIC_DIR",
    "XDG_PICTURES_DIR",
    "XDG_VIDEOS_DIR"
  };

  list_files = (mode == FILE_CHOOSER);

  if ((env = getenv("HOME")) && strlen(env) > 0) {
    home_dir = std::string(env);
  }

  /* needed for sorting */
  setlocale(LC_ALL, "");

#ifdef DYNAMIC_MAGIC
  handle = dlopen("libmagic.so.1", RTLD_LAZY);

# define DLSYM(func) \
  if (handle) { \
    func = reinterpret_cast<func##_t>(dlsym(handle, STRINGIFY(func))); \
    if (dlerror()) { dlclose(handle); handle = NULL; } \
  }

  DLSYM(magic_open)
  DLSYM(magic_load)
  DLSYM(magic_file)
  DLSYM(magic_close)

# undef DLSYM
#endif  /* DYNAMIC_MAGIC */

  win = new Fl_Double_Window(w, h, title);
  {
    g = new Fl_Group(0, 0, w, h);
    {
      g_top = new Fl_Group(0, 0, w, 40);
      {
        const int bt_w = 36;

        addrline = new Fl_Box(10, 7, w - bt_w*3 - 25, 26, " /");
        addrline->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
        addrline->box(FL_FLAT_BOX);
        addrline->color(fl_lighter(addrline->color()));

        /* cover up the end of addrline */
       { Fl_Box *o = new Fl_Box(w - bt_w*3 - 15, 5, bt_w*3 + 15, 30);
        o->box(FL_FLAT_BOX); }

        bt_up = new Fl_Button(w - bt_w*3 - 10, 5, bt_w, 30);
        bt_up->tooltip("Parent Directory");
        bt_up->image(go_up_gray);
        bt_up->deactivate();
        bt_up->callback(up_callback);
        bt_up->clear_visible_focus();

       { Fl_Button *o = new Fl_Button(w - bt_w*2 - 10, 5, bt_w, 30);
        o->tooltip("Sort Order");
        o->image(sort_order1);
        o->callback(sort_callback);
        o->clear_visible_focus(); }

       { Fl_Button *o = new Fl_Button(w - bt_w - 10, 5, bt_w, 30);
        o->tooltip("Toggle Hidden Files/Directories");
        o->image(eye_closed);
        o->callback(hidden_callback);
        o->clear_visible_focus(); }
      }
      g_top->resizable(addrline);
      g_top->end();

      g_main = new Fl_Group(0, 40, w, h - g_top->h());
      {
        g_main_left = new Fl_Group(0, 40, 120, h - g_top->h());
        {
         { Fl_Button *o = b = new Fl_Button(10, 40, 100, 30, "/");
          o->callback(top_callback);
          o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          o->clear_visible_focus(); }

         { Fl_Button *o = new Fl_Button(10, b->y() + 30, 100, 30, "Home");
          o->callback(home_callback);
          o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          o->clear_visible_focus();
          b = o; }

          if (xdg_user_dir_lookup(vec)) {
            for (const auto type : xdg_types) {
              /* begin at the last vector entry; if there were multiple entries
               * of the same XDG type we pick the last one added to the config file */
              for (auto it = vec.end() - 1; it >= vec.begin(); --it) {
                std::string &s = *it;
                size_t len = strlen(type);
                if (s.substr(0, len) == type) {
                  std::string dir = s.substr(len);

                  if (!fl_filename_isdir(dir.c_str())) {
                    if (strcmp("XDG_DESKTOP_DIR", type) == 0) {
                      /* fallback to "$HOME/Desktop" */
                      dir = home_dir + "/Desktop";
                      if (fl_filename_isdir(dir.c_str())) {
                        vec2.push_back(dir);
                        break;
                      }
                    }
                    continue;
                  }

                  vec2.push_back(dir);
                  break;
                }
              }
            }
            vec.clear();
            std::sort(vec2.begin(), vec2.end(), ignorecaseSortXDG);

            for (auto it = vec2.begin(); it != vec2.end(); ++it) {
              std::string &s = *it;
              Fl_Button *o = new Fl_Button(10, b->y() + 30, 100, 30);
              o->label(s.c_str() + s.rfind('/') + 1);
              o->callback(xdg_callback, reinterpret_cast<void *>(const_cast<char *>(s.c_str())));
              o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
              o->clear_visible_focus();
              b = o;
            }
          }

          dummy = new Fl_Box(10, b->y() + 40, 100, g_main_left->h() - b->y() - 76);
          dummy->box(FL_NO_BOX);
        }
        g_main_left->resizable(dummy);
        g_main_left->end();

        br = new Fl_Hold_Browser(120, 40, w - 130, h - g_top->h() - 76);
        br->selection_color(fl_lighter(FL_DARK_BLUE));
        br->callback(br_callback);

        g_bottom = new Fl_Group(0, br->y() + br->h(), w, h - br->y() - br->h());
        {
          const int bt_h = 26;
          int bt_w1 = measure_button_width(fl_ok, 40);
          int bt_w2 = measure_button_width(fl_cancel, 15);
          int bt_w = (bt_w1 > bt_w2) ? bt_w1 : bt_w2;

          bt_ok = new Fl_Return_Button(w - bt_w - 10, br->y() + br->h() + 10, bt_w, bt_h, fl_ok);
          //bt_ok->deactivate();
          bt_ok->callback(close_cb, 0);
          bt_cancel = new Fl_Button(bt_ok->x(), bt_ok->y() + bt_h + 5, bt_w, bt_h, fl_cancel);
          bt_cancel->callback(close_cb, 1);

          g_bottom_inside = new Fl_Group(10, g_bottom->y(), w - bt_w - 30, g_bottom->h());
          {
            input = new Fl_Input(10, br->y() + br->h() + 10, bt_ok->x() - 20, bt_h);
            infobox = new Fl_Box(10, input->y() + input->h() + 5, input->w(), bt_h);
#ifdef DYNAMIC_MAGIC
            infobox->box(handle ? FL_THIN_DOWN_BOX : FL_NO_BOX);
#else
            infobox->box(FL_THIN_DOWN_BOX);
#endif
            infobox->labelsize(12);
            infobox->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          }
          g_bottom_inside->end();
        }
        g_bottom->resizable(g_bottom_inside);
        g_bottom->end();
      }
      g_main->resizable(br);
      g_main->end();
    }
    g->resizable(g_main);
    g->end();
  }
  win->callback(close_cb, 1);

  home_callback(NULL);
  run_window(win, g, 320, 360);

  return selected_file;
}

