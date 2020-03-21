/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2020, djcj <djcj@gmx.de>
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
#include <libgen.h>
#include <limits.h>
#include <magic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fltk-dialog.hpp"
#include "octicons.h"

#define DOUBLECLICK_TIME 1.0

static Fl_Double_Window *win;
static Fl_Hold_Browser *br;
static Fl_Box *addrline, *infobox;
static Fl_Button *bt_popd, *bt_up;
static Fl_Return_Button *bt_ok;
static Fl_Input *input;

static std::string current_dir = "/", prev_dir, home_dir = "/home", selected_file;
static char *magicdb = NULL;
static int selection = 0;
static bool show_dotfiles = false, list_files = true, sort_reverse = false;

static void br_change_dir(void);
static void selection_timeout(void);
static Fl_Timeout_Handler th = reinterpret_cast<Fl_Timeout_Handler>(selection_timeout);

#define PNG(a,b)  static Fl_PNG_Image a(NULL, octicons_##b##_png, octicons_##b##_png_len);
PNG(eye, eye)
PNG(eye_closed, eye_closed)
PNG(go_up, arrow_up)
PNG(go_up_gray, arrow_up_gray)
PNG(go_back, arrow_both)
PNG(go_back_gray, arrow_both_gray)
PNG(icon_any, file)
PNG(icon_dir, file_directory)
PNG(icon_link_any, file_symlink_file)
PNG(icon_link_dir, file_symlink_directory)
PNG(sort_order1, list_ordered_1)
PNG(sort_order2, list_ordered_2)

/* Format is XDG_XXX_DIR="$HOME/yyy", where yyy is a shell-escaped
 * homedir-relative path, or XDG_XXX_DIR="/yyy", where /yyy is an
 * absolute path. No other format is supported.
 */
static bool xdg_user_dir_lookup(std::vector<std::string> &vec)
{
  std::ifstream ifs;
  std::string line;

  char *xdg_conf = getenv("XDG_CONFIG_HOME");

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

    if (line.substr(4, pos - 4).find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ_") != std::string::npos) {
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
      std::string s = home_dir;

      if (s.back() == '/') {
        s.pop_back();
      }
      line.replace(pos, 5, s);
    } else if (line[pos] != '/') {
      continue;
    }

    vec.push_back(line);
  }

  ifs.close();

  return vec.size() > 0;
}

static std::string getfsize(long bytes)
{
  const char *unit;
  char ch[64] = {0};
  const int KiBYTES = 1024;
  const int MiBYTES = 1024 * KiBYTES;
  const int GiBYTES = 1024 * MiBYTES;

  double size = bytes;

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
    snprintf(ch, sizeof(ch) - 1, "%ld", bytes);
    return std::string(ch) + " Bytes";
  }

  snprintf(ch, sizeof(ch) - 1, "%.1f", size);
  return std::string(ch) + unit;
}

static std::string get_filetype(const char *file)
{
  magic_t mcookie;
  const char *desc;
  std::string str = "";
  std::size_t pos;

  const int flags = MAGIC_PRESERVE_ATIME
    | MAGIC_ERROR
    | MAGIC_SYMLINK
    | MAGIC_NO_CHECK_APPTYPE
    | MAGIC_NO_CHECK_COMPRESS
    | MAGIC_NO_CHECK_ELF
    | MAGIC_NO_CHECK_TAR;

  if ((mcookie = magic_open(flags)) == NULL) {
    return "";
  }

  if (magic_load(mcookie, magicdb) != 0) {
    magic_close(mcookie);
    return "";
  }

  if ((desc = magic_file(mcookie, file)) != NULL) {
    str = desc;
    if ((pos = str.find_first_of(',')) != std::string::npos) {
      str.erase(pos);
    }
  }

  magic_close(mcookie);

  return str;
}

static void fileInfo(const char *file)
{
  std::string str = "";
  char *resolved = NULL;
  struct stat st;

  if (!file || strlen(file) == 0) {
    infobox->label(NULL);
    return;
  }

  auto icon = br->icon(selection);

  if (icon == &icon_dir || icon == &icon_link_dir) {
    infobox->label("directory");
    return;
  }

  if (icon == &icon_link_any && (resolved = realpath(file, NULL)) == NULL) {
    /* get actual link size */
    lstat(file, &st);
    str = getfsize(st.st_size);
    str += (st.st_size == 0) ? ",  empty" : ",  broken symbolic link";
  } else {
    /* get target filesize */
    stat(file, &st);
    str = getfsize(st.st_size) + ",  " + get_filetype(file);
  }

  if (resolved) {
    free(resolved);
  }

  infobox->copy_label(str.c_str());
}

static void popd_callback(Fl_Widget *)
{
  if (prev_dir.empty()) {
    bt_popd->deactivate();
  } else {
    current_dir.swap(prev_dir);
    br_change_dir();
  }
}

static void up_callback(Fl_Widget *)
{
  if (current_dir != "/") {
    prev_dir = current_dir;

    if (current_dir.back() == '/') {
      current_dir.pop_back();
    }
    current_dir.erase(current_dir.rfind('/'));

    if (current_dir.empty()) {
      current_dir = "/";
    } else {
      current_dir.push_back('/');
    }
    br_change_dir();
  }
}

static void xdg_callback(Fl_Widget *, void *v)
{
  if (v) {
    const char *new_dir = reinterpret_cast<const char *>(v);

    if (current_dir != new_dir) {
      prev_dir = current_dir;
      current_dir = new_dir;
    }
    br_change_dir();
  }
}

static void top_callback(Fl_Widget *)
{
  if (current_dir != "/") {
    prev_dir = current_dir;
    current_dir = "/";
  }
  br_change_dir();
}

static void home_callback(Fl_Widget *)
{
  if (current_dir != home_dir) {
    prev_dir = current_dir;
    current_dir = home_dir;
  }
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

static void close_cb(Fl_Widget *)
{
  for (int i = 0; i <= br->size(); ++i) {
    if (br->data(i)) {
      free(br->data(i));
    }
  }

  win->hide();
}

static void ok_cb(Fl_Widget *)
{
  int line = br->value();

  if (line > 0) {
    selected_file = current_dir;

    if (selected_file.back() != '/') {
      selected_file.push_back('/');
    }

    if (br->data(line)) {
      selected_file += reinterpret_cast<const char *>(br->data(line));
    }

    if (list_files && fl_filename_isdir(selected_file.c_str())) {
      /* don't return path but change directory */
      if (access(selected_file.c_str(), R_OK) == 0) {
        prev_dir = current_dir;
        current_dir = selected_file;
        br_change_dir();
      }
      selected_file.clear();
      return;
    }
    br->data(line, NULL);
  } else {  /* nothing selected */
    if (list_files) {
      return;
    }
    selected_file = current_dir;
  }

  close_cb(NULL);
}

static void br_callback(Fl_Widget *)
{
  auto icon = br->icon(br->value());

  if (!list_files && icon != &icon_dir && icon != &icon_link_dir) {
    Fl::remove_timeout(th);
    selection = 0;
    input->value("");
    infobox->label(NULL);
    br->deselect();
    return;
  }

  std::string name = "";
  std::string path = current_dir;

  if (br->data(br->value())) {
    name = reinterpret_cast<const char *>(br->data(br->value()));
  }

  if (path.back() != '/') {
    path.push_back('/');
  }
  path += name;

  /* some workaround to change directories on double-click */
  if (selection == 0) {
    selection = br->value();
    bt_ok->activate();
    Fl::add_timeout(DOUBLECLICK_TIME, th);
  } else {
    Fl::remove_timeout(th);
    if (br->value() == selection) {
      selection = 0;

      if (name.back() == '/') {
        /* double-clicked on directory */
        if (access(path.c_str(), R_OK) == 0) {
          prev_dir = current_dir;
          current_dir = path.c_str();
          br_change_dir();
        }
      } else {
        /* double-clicked on file */
        ok_cb(NULL);
        return;
      }
    } else {
      selection = br->value();
      Fl::add_timeout(DOUBLECLICK_TIME, th);
    }
  }

  if (br->value() == 0) {
    input->value("");
    infobox->label(NULL);
    if (list_files) {
      bt_ok->deactivate();
    }
  } else {
    if (name.back() == '/') {
      name.pop_back();
    }
    fileInfo(path.c_str());
    input->value(name.c_str());
  }
}

/* sort by basename */
static bool ignorecaseSortXDG(std::string s1, std::string s2) {
  return (strcoll(s1.c_str() + s1.rfind('/') + 1, s2.c_str() + s2.rfind('/') + 1) < 0);
}

static void br_change_dir(void)
{
  struct dirent **list;
  const char *white = "@B255@. ", *yellow = "@B17@. ";

  /* current_dir was deleted in the meanwhile;
   * move up until we are in an existing directory */
  if (!fl_filename_isdir(current_dir.c_str())) {
    for (auto i = std::count(current_dir.begin(), current_dir.end(), '/'); i > 0; --i) {
      size_t pos = current_dir.rfind('/');

      if (pos < 1) {
        current_dir = "/";
        break;
      }
      current_dir.erase(pos);

      if (fl_filename_isdir(current_dir.c_str())) {
        break;
      }
    }
  }

  if (prev_dir.empty() || prev_dir == "" || !fl_filename_isdir(prev_dir.c_str())) {
    prev_dir.clear();
  }

  int n = fl_filename_list(current_dir.c_str(), &list, fl_casenumericsort);

  /* on error switch to home directory */
  if (n < 0) {
    current_dir = home_dir;
    n = fl_filename_list(current_dir.c_str(), &list, fl_casenumericsort);
  }

  /* clear browser */
  for (int i = 0; i <= br->size(); ++i) {
    if (br->data(i)) {
      free(br->data(i));
    }
  }
  br->clear();

  if (list_files) {
    bt_ok->deactivate();
  }
  selection = 0;

  if (n > 0) {
    /* list directories */
    for (int i = 0; i < n; ++i) {
      char *name = sort_reverse ? list[n - 1 - i]->d_name : list[i]->d_name;
      size_t len = strlen(name);

      if (!show_dotfiles && name[0] == '.') {
        /* skip hidden entry */
        continue;
      }

      if (strcmp(name, "./") == 0 || strcmp(name, "../") == 0) {
        /* skip "." and ".." entries */
        continue;
      }

      if (name[len-1] != '/') {
        /* not a directory */
        continue;
      }

      std::string entry = (br->size() % 2 == 0) ? white : yellow;
      entry.append(name);
      entry.pop_back();  /* remove trailing '/' */

      std::string path = current_dir;

      if (path.back() != '/') {
        path.push_back('/');
      }
      path.append(name);
      path.pop_back();  /* remove trailing '/' */

      struct stat st;
      lstat(path.c_str(), &st);

      br->add(entry.c_str(), reinterpret_cast<void *>(strdup(name)));
      br->icon(br->size(), S_ISLNK(st.st_mode) ? &icon_link_dir : &icon_dir);
    }

    /* list files */
    for (int i = 0; i < n; ++i) {
      std::string entry;
      char *name = sort_reverse ? list[n - 1 - i]->d_name : list[i]->d_name;
      size_t len = strlen(name);

      if (!show_dotfiles && name[0] == '.') {
        /* skip hidden entry */
        continue;
      }

      if (name[len-1] == '/') {
        /* skip directories */
        continue;
      }

      if (!list_files) {
        entry = "@i@C8";
      }

      entry += (br->size() % 2 == 0) ? white : yellow;
      entry.append(name);

      std::string path = current_dir;

      if (path.back() != '/') {
        path.push_back('/');
      }
      path.append(name);

      struct stat st;
      lstat(path.c_str(), &st);

      br->add(entry.c_str(), reinterpret_cast<void *>(strdup(name)));
      br->icon(br->size(), S_ISLNK(st.st_mode) ? &icon_link_any : &icon_any);
    }

    fl_filename_free_list(&list, n);
  }

  if (current_dir == "/") {
    addrline->label(" /");
    bt_up->deactivate();
    bt_up->image(go_up_gray);
  } else {
    std::string s = " " + current_dir;

    if (s.back() == '/') {
      s.pop_back();
    }
    addrline->copy_label(s.c_str());
    bt_up->activate();
    bt_up->image(go_up);
  }

  if (prev_dir.empty()) {
    bt_popd->deactivate();
    bt_popd->image(go_back_gray);
  } else {
    bt_popd->activate();
    bt_popd->image(go_back);
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
  char buf[PATH_MAX] = {0};
  const int w = 800, h = 600;
  char *env, *resolved, *dir;

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

    if (home_dir.back() != '/') {
      home_dir.push_back('/');
    }
  }

  /* for file sizes */
  setlocale(LC_NUMERIC, "C");

  /* needed for sorting */
  setlocale(LC_COLLATE, "");

  /* path to magic DB */
  if ((resolved = realpath("/proc/self/exe", buf)) != NULL && (dir = dirname(resolved)) != NULL) {
    std::string s = std::string(dir) + "/../share/file/magic.mgc";
    std::ifstream ifs(s);

    if (ifs.is_open()) {
      ifs.close();
      magicdb = strdup(s.c_str());
    } else {
      s = std::string(dir) + "/../share/misc/magic.mgc";
      ifs.open(s);

      if (ifs.is_open()) {
        ifs.close();
        magicdb = strdup(s.c_str());
      }
    }
  }

  win = new Fl_Double_Window(w, h, title);
  {
    g = new Fl_Group(0, 0, w, h);
    {
      g_top = new Fl_Group(0, 0, w, 40);
      {
        const int bt_w = 36;

        addrline = new Fl_Box(10, 7, w - bt_w*4 - 25, 26, " /");
        addrline->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
        addrline->box(FL_FLAT_BOX);
        addrline->color(fl_lighter(addrline->color()));

        /* cover up the end of addrline */
       { Fl_Box *o = new Fl_Box(w - bt_w*4 - 15, 5, bt_w*4 + 15, 30);
        o->box(FL_FLAT_BOX); }

        bt_popd = new Fl_Button(w - bt_w*4 - 10, 5, bt_w, 30);
        bt_popd->tooltip("Previous Directory");
        bt_popd->image(go_back_gray);
        bt_popd->deactivate();
        bt_popd->callback(popd_callback);
        bt_popd->clear_visible_focus();

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
            for (const char *type : xdg_types) {
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
                      dir = home_dir + "Desktop";
                      if (fl_filename_isdir(dir.c_str())) {
                        vec2.push_back(dir);
                        break;
                      }
                    }
                    continue;
                  }

                  if (dir.back() != '/') {
                    dir.push_back('/');
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
              std::string l = s;

              if (l.back() == '/') {
                l.pop_back();
              }

              Fl_Button *o = new Fl_Button(10, b->y() + 30, 100, 30);
              o->copy_label(l.c_str() + l.rfind('/') + 1);
              o->callback(xdg_callback, reinterpret_cast<void *>(const_cast<char *>(s.c_str())));
              o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
              o->clear_visible_focus();
              b = o;
            }
          }

          dummy = new Fl_Box(10, b->y() + 40, 100, g_main_left->h() - b->y() - 76);
          dummy->box(FL_NO_BOX);

          b = NULL;
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
          if (list_files) {
            bt_ok->deactivate();
          }
          bt_ok->callback(ok_cb);
          bt_cancel = new Fl_Button(bt_ok->x(), bt_ok->y() + bt_h + 5, bt_w, bt_h, fl_cancel);
          bt_cancel->callback(close_cb);

          g_bottom_inside = new Fl_Group(10, g_bottom->y(), w - bt_w - 30, g_bottom->h());
          {
            input = new Fl_Input(10, br->y() + br->h() + 10, bt_ok->x() - 20, bt_h);
            infobox = new Fl_Box(10, input->y() + input->h() + 5, input->w(), bt_h);
            infobox->box(FL_THIN_DOWN_BOX);
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
  win->callback(close_cb);

  current_dir.clear();
  home_callback(NULL);
  run_window(win, g, 320, 360);

  if (magicdb) {
    free(magicdb);
  }

  return selected_file.empty() ? NULL : strdup(selected_file.c_str());
}

