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
#include "icons.h"


class file_chooser_fltk
{
private:
  void create_window(int mode);

public:
  char *get_selection(void);

  file_chooser_fltk(int mode, bool check_devices);
  ~file_chooser_fltk();
};


#define CONF_MAX_SIZE     (512*1024)
#define DOUBLECLICK_TIME  1.0
#define SIDEBAR_EXTRA_W   40
#define STR2VP(x)         reinterpret_cast<void *>( const_cast<char *>(x) )

#define SIDEBAR_LABELLINE(x) \
  sidebar->add("@S8@. "); \
  sidebar->add("@s@b@.   " x); \
  sidebar->add("@S3@. ");

typedef struct {
  char label[256];
  char dev[256];
  char mount[512];
  bool mounted;
  bool rom;
  bool hotplug;
} part_t;

static Fl_Double_Window *win;
static Fl_Group *g;
static Fl_Hold_Browser *br, *sidebar;
static Fl_Box *addrline, *infobox = NULL;
static Fl_Button *bt_popd, *bt_up;
static Fl_Return_Button *bt_ok;
static Fl_Input *input;

//static pthread_t th;
static std::vector<part_t> part_vec;
static std::vector<std::string> xdg_dirs, bookmarks;
static std::string current_dir = "/", prev_dir, home_dir = "/home/", selected_file, desktop;
static char *magicdb = NULL;
static int selection = 0;
static bool show_dotfiles = false, list_files = true, sort_reverse = false;
static int sidebar_first_device = 0;
static int sidebar_last_device = 0;

static void br_change_dir(void);
static void selection_timeout(void);
static Fl_Timeout_Handler htimeout = reinterpret_cast<Fl_Timeout_Handler>(selection_timeout);

static void mount_timeout(void);
static double mount_timeout_limit = 0;
static Fl_Timeout_Handler hmount = reinterpret_cast<Fl_Timeout_Handler>(mount_timeout);

#define PNG(x)  static Fl_PNG_Image x(NULL, icons_##x##_png, icons_##x##_png_len);
PNG(eye)
PNG(eye_closed)
PNG(go_up)
PNG(go_up_gray)
PNG(go_back)
PNG(go_back_gray)
PNG(icon_any)
PNG(icon_hdd)
PNG(icon_rom)
PNG(icon_plugged)
PNG(icon_dir)
PNG(icon_desktop)
PNG(icon_home)
PNG(icon_link_any)
PNG(icon_link_dir)
PNG(list_ordered_1)
PNG(list_ordered_2)


/* sort by basename */
static bool ignorecasesort(std::string s1, std::string s2) {
  return (strcoll(s1.c_str() + s1.rfind('/') + 1, s2.c_str() + s2.rfind('/') + 1) < 0);
}

static void resize_sidebar(int n)
{
  const int max = br->parent()->w() / 3;

  if (n > max) {
    n = max;
  }

  sidebar->resize(sidebar->x(), sidebar->y(), n, sidebar->h());
  br->resize(sidebar->x() + n, br->y(), br->parent()->w() - n, br->h());
  br->parent()->redraw();
}

static std::string getval(std::string val, const char *line)
{
  std::string s;
  size_t pos, pos2;

  if (!line || strlen(line) == 0 || val.empty() || val == "") {
    return "";
  }
  s = line;
  val += "=\"";

  if ((pos = s.find(val)) == std::string::npos) {
    return "";
  }
  pos += val.size();

  if ((pos2 = s.find('"', pos)) == std::string::npos) {
    return "";
  }

  return s.substr(pos, pos2 - pos);
}

static void get_partitions(void)
{
  FILE *fp;
  char *user, *buf = NULL;
  size_t n = 0;
  unsigned int count = 0;

  if ((user = getenv("USER")) == NULL) {
    return;
  }

  if ((fp = popen("lsblk -p -P -o mountpoint,type,name,label,uuid,size,hotplug 2>/dev/null", "r")) == NULL) {
    return;
  }

  while (getline(&buf, &n, fp) != -1) {
    std::string s;

    part_t part = {0};
    part.mounted = false;
    part.rom = false;
    part.hotplug = false;

    if (n == 0 || !buf) continue;

    if ((s = getval("MOUNTPOINT", buf)) == "/") continue;
    if (s != "") {
      strncpy(part.mount, s.c_str(), sizeof(part.mount) - 1);
      part.mounted = true;
    }

    s = getval("TYPE", buf);
    if (s != "part" && s != "rom") continue;
    if (s == "rom") {
      if (!part.mounted) continue;
      part.rom = true;
    }

    if ((s = getval("NAME", buf)) == "") continue;
    strncpy(part.dev, s.c_str(), sizeof(part.dev) - 1);

    if ((s = getval("LABEL", buf)) != "") {
      strncpy(part.label, s.c_str(), sizeof(part.label) - 1);

      if (!part.mounted) {
        /* replace directory delimiters */
        for (size_t i = 0; i < s.size(); ++i) {
          if (s[i] == '/') {
            s[i] = '_';
          }
        }
        snprintf(part.mount, sizeof(part.mount) - 1, "/media/%s/%s", user, s.c_str());
      }
    } else {
      if ((s = getval("UUID", buf)) == "") continue;

      if (!part.mounted) {
        snprintf(part.mount, sizeof(part.mount) - 1, "/media/%s/%s", user, s.c_str());
      }

      if ((s = getval("SIZE", buf)) == "") {
        strncpy(part.label, "Drive", sizeof(part.label) - 1);
      } else {
        snprintf(part.label, sizeof(part.label) - 1, "Drive %s", s.c_str());
      }
    }

    if (getval("HOTPLUG", buf) == "1") {
      part.hotplug = true;
    }

    part_vec.push_back(part);
    count++;
  }

  if (buf) {
    free(buf);
  }

  pclose(fp);

  if (count == 0) {
    return;
  }

  /* remove duplicates (same mountpoint or device) */
  for (auto it = part_vec.begin(); it != part_vec.end(); ++it) {
    part_t p = *it;
    for (auto it2 = it + 1; it2 != part_vec.end(); ++it2) {
      part_t q = *it2;
      if (strcmp(p.mount, q.mount) == 0 || strcmp(p.dev, q.dev) == 0) {
        part_vec.erase(it2);
        it2--;
      }
    }
  }

  /* add entries to sidebar */

  SIDEBAR_LABELLINE("Devices");

  int sbW = sidebar->w();

  sidebar_first_device = sidebar->size() + 1;

  for (auto &p : part_vec) {
    sidebar->add(p.label, reinterpret_cast<void *>(&p));
    sidebar_last_device = sidebar->size();

    if (p.rom) {
      sidebar->icon(sidebar->size(), &icon_rom);
    } else if (p.hotplug) {
      sidebar->icon(sidebar->size(), &icon_plugged);
    } else {
      sidebar->icon(sidebar->size(), &icon_hdd);
    }
    //tooltip => p.dev ??

    int m = measure_button_width(p.label, SIDEBAR_EXTRA_W);

    if (m > sbW) {
      sbW = m;
    }
  }

  resize_sidebar(sbW);
}

/* look for gtk3 bookmarks;
 * run this after xdg_user_dir_lookup() to avoid duplicated entries
 */
static void get_gtk3_bookmarks(void)
{
  std::ifstream ifs;
  std::string line;
  std::vector<std::string> vec;

  /* open bookmarks file */

  char *xdg_conf = getenv("XDG_CONFIG_HOME");

  if (xdg_conf && strlen(xdg_conf) > 0) {
    ifs.open(std::string(xdg_conf) + "/gtk-3.0/bookmarks", std::ios::in|std::ios::ate);
    if (!ifs.is_open()) {
      xdg_conf = NULL;
    }
  }

  if (!xdg_conf) {
    ifs.open(home_dir + "/.config/gtk-3.0/bookmarks", std::ios::in|std::ios::ate);
    if (!ifs.is_open()) {
      return;
    }
  }

  if (ifs.tellg() > CONF_MAX_SIZE) {
    ifs.close();
    return;
  }

  ifs.seekg(0, std::ios::beg);

  /* parse bookmarks file */

  while (std::getline(ifs, line)) {
    size_t pos;

    if (line.substr(0,8) != "file:///") {
      continue;
    }

    if ((pos = line.find(' ')) != std::string::npos) {
      line.erase(pos);
    }

    char *p = strdup(line.c_str() + 7);
    fl_decode_uri(p);

    if (!fl_filename_isdir(p)) {
      free(p);
      continue;
    }

    line = p;
    free(p);

    if (line.back() != '/') {
      line.push_back('/');
    }

    if (line != "/" && line != home_dir) {
      vec.push_back(line);
    }
  }

  ifs.close();

  if (vec.size() == 0) {
    return;
  }

  /* only add directories to bookmarks that aren't in xdg_dirs */
  for (auto &s : vec) {
    bool found = false;

    for (auto &xdg_entry : xdg_dirs) {
      if (s == xdg_entry) {
        found = true;
      }
    }

    if (!found) {
      bookmarks.push_back(s);
    }
  }

  if (bookmarks.size() == 0) {
    return;
  }

  vec.clear();
  //std::sort(bookmarks.begin(), bookmarks.end(), ignorecasesort);

  SIDEBAR_LABELLINE("Bookmarks");

  int sbW = sidebar->w();

  for (auto &s : bookmarks) {
    std::string l = s;
    l.pop_back();  /* remove trailing "/" */

    const char *p = l.c_str() + l.rfind('/') + 1;
    int m = measure_button_width(p, SIDEBAR_EXTRA_W);

    sidebar->add(p, STR2VP(s.c_str()));
    sidebar->icon(sidebar->size(), &icon_dir);

    if (m > sbW) {
      sbW = m;
    }
  }

  resize_sidebar(sbW);
}

/* Format is XDG_XXX_DIR="$HOME/yyy", where yyy is a shell-escaped
 * homedir-relative path, or XDG_XXX_DIR="/yyy", where /yyy is an
 * absolute path. No other format is supported.
 */
static void xdg_user_dir_lookup(void)
{
  std::vector<std::string> vec;
  std::ifstream ifs;
  std::string line;

  const char *xdg_types[] = {
    "XDG_DESKTOP_DIR",
    "XDG_DOCUMENTS_DIR",
    "XDG_DOWNLOAD_DIR",
    "XDG_MUSIC_DIR",
    "XDG_PICTURES_DIR",
    "XDG_VIDEOS_DIR"
  };

  /* open "user-dirs.dirs" config file */

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
      return;
    }
  }

  if (ifs.tellg() > CONF_MAX_SIZE) {
    ifs.close();
    return;
  }

  ifs.seekg(0, std::ios::beg);

  /* parse config file */

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

    if (line.substr(4, pos - 4).find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
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
      line.replace(pos, 6, home_dir);
    } else if (line[pos] != '/') {
      continue;
    }

    vec.push_back(line);
  }

  ifs.close();

  if (vec.size() == 0) {
    return;
  }

  /* add XDG directories to sidebar */

  for (const char *type : xdg_types) {
    /* begin at the last vector entry; if there were multiple entries
     * of the same XDG type we pick the last one added to the config file */
    for (auto it = vec.end() - 1; it >= vec.begin(); --it) {
      std::string &s = *it;
      size_t len = strlen(type);
      if (s.substr(0, len) == type) {
        std::string dir = s.substr(len);

        if (dir.back() == '/') {
          dir.pop_back();
        }

        if (!fl_filename_isdir(dir.c_str())) {
          if (strcmp("XDG_DESKTOP_DIR", type) == 0) {
            /* fallback to "$HOME/Desktop" */
            dir = home_dir + "Desktop";
            if (fl_filename_isdir(dir.c_str())) {
              desktop = dir;
            }
          }
          continue;
        }

        if (strcmp("XDG_DESKTOP_DIR", type) == 0) {
          desktop = dir;
        } else {
          xdg_dirs.push_back(dir);
        }
      }
    }
  }

  vec.clear();

  if (!desktop.empty()) {
    desktop.push_back('/');
    sidebar->add("Desktop", STR2VP(desktop.c_str()));
    sidebar->icon(sidebar->size(), &icon_desktop);
  }

  std::sort(xdg_dirs.begin(), xdg_dirs.end(), ignorecasesort);

  int sbW = sidebar->w();

  for (auto &s : xdg_dirs) {
    std::string l = s;
    const char *p = l.c_str() + l.rfind('/') + 1;
    int m = measure_button_width(p, SIDEBAR_EXTRA_W);

    s.push_back('/');
    sidebar->add(p, STR2VP(s.c_str()));
    sidebar->icon(sidebar->size(), &icon_dir);

    if (m > sbW) {
      sbW = m;
    }
  }

  resize_sidebar(sbW);
}

static std::string get_filesize(long bytes)
{
  std::ostringstream out;
  const char *unit;
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
    return std::to_string(bytes) + " Bytes";
  }

  out.precision(1);
  out << std::fixed << size;

  return out.str() + unit;
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
  std::string info = "", type = "";
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
    type = (st.st_size == 0) ? "empty" : "broken symbolic link";
  } else {
    /* get target filesize */
    stat(file, &st);
    type = get_filetype(file);
  }

  info = get_filesize(st.st_size);

  if (type != "") {
    info += ",  " + type;
  }

  if (resolved) {
    free(resolved);
  }

  infobox->copy_label(info.c_str());
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
  if (current_dir == "/") {
    return;
  }

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

static void mount_timeout(void)
{
  if (access(current_dir.c_str(), R_OK) == 0 || mount_timeout_limit <= 0) {
    Fl::remove_timeout(htimeout);
    br_change_dir();
    sidebar->deselect();
  } else {
    mount_timeout_limit -= 0.01;
    Fl::add_timeout(0.01, hmount);
  }
}

static void sidebar_callback(Fl_Widget *o)
{
  std::string new_dir;
  part_t *p = NULL;
  int val = sidebar->value();

  if (val == 0 || sidebar->data(val) == NULL) {
    sidebar->deselect();
    return;
  }

  if (val >= sidebar_first_device && val <= sidebar_last_device) {
    p = reinterpret_cast<part_t *>(sidebar->data(val));
    new_dir = p->mount;
  } else {
    new_dir = reinterpret_cast<const char *>(sidebar->data(val));;
  }

  if (current_dir != new_dir) {
    prev_dir = current_dir;
    current_dir = new_dir;
  }

  if (p && !p->mounted) {
    std::string cmd = "gio mount -d ";
    cmd += p->dev;
    cmd += " >/dev/null 2>/dev/null";

    if (system(cmd.c_str()) == 0) {
      p->mounted = true;
    }

    mount_timeout_limit = 2.0;
    Fl::add_timeout(0.01, hmount);
  } else {
    br_change_dir();
    sidebar->deselect();
  }
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
    b->image(list_ordered_1);
  } else {
    sort_reverse = true;
    b->image(list_ordered_2);
  }

  br_change_dir();
}

static void selection_timeout(void) {
  selection = 0;
}

static void cancel_cb(Fl_Widget *o) {
  //pthread_cancel(th);
  o->window()->hide();
}

static void ok_cb(Fl_Widget *o)
{
  if (br->value() > 0) {
    selected_file = current_dir;

    if (selected_file.back() != '/') {
      selected_file.push_back('/');
    }

    /* skip text-formatting chars */
    const char *p = strstr(br->text(br->value()), "@.");
    selected_file += p ? p + 2 : br->text(br->value());

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
  } else {  /* nothing selected */
    if (list_files) {
      return;
    }
    selected_file = current_dir;
  }

  //pthread_cancel(th);
  o->window()->hide();
}

static void br_callback(Fl_Widget *o)
{
  auto icon = br->icon(br->value());

  if (br->value() == 0 || (!list_files && icon != &icon_dir && icon != &icon_link_dir)) {
    Fl::remove_timeout(htimeout);
    selection = 0;
    input->value("");
    br->deselect();

    if (infobox) {
      infobox->label(NULL);
    }

    if (list_files) {
      bt_ok->deactivate();
    }
    return;
  }

  /* skip text-formatting chars */
  const char *p = strstr(br->text(br->value()), "@.");
  std::string name = p ? p + 2 : br->text(br->value());

  std::string path = current_dir;

  if (path.back() != '/') {
    path.push_back('/');
  }
  path += name;

  /* some workaround to change directories on double-click */
  if (selection == 0) {
    selection = br->value();
    bt_ok->activate();
    Fl::add_timeout(DOUBLECLICK_TIME, htimeout);
  } else {
    Fl::remove_timeout(htimeout);
    if (br->value() == selection) {
      selection = 0;

      if (icon == &icon_dir || icon == &icon_link_dir) {
        /* double-clicked on directory */
        if (access(path.c_str(), R_OK) == 0) {
          prev_dir = current_dir;
          current_dir = path.c_str();
          br_change_dir();
        }
      } else {
        /* double-clicked on file */
        ok_cb(o);
        return;
      }
    } else {
      selection = br->value();
      Fl::add_timeout(DOUBLECLICK_TIME, htimeout);
    }
  }

  if (selection != 0) {
    if (name.back() == '/') {
      name.pop_back();
    }

    if (infobox) {
      fileInfo(path.c_str());
    }

    input->value(name.c_str());
  }
}

static void br_change_dir(void)
{
  struct dirent **list;
  const char *white = "@.";  // "@B255@."
  const char *yellow = "@B17@.";

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

      /* check for hidden directories AND files first */
      if (!show_dotfiles && name[0] == '.') {
        name[0] = 0;
        continue;
      }

      if (strcmp(name, "./") == 0 || strcmp(name, "../") == 0) {
        /* skip "." and ".." entries */
        name[0] = 0;
        continue;
      }

      size_t len = strlen(name);

      if (name[len-1] != '/') {
        /* not a directory */
        continue;
      }

      /* remove trailing '/' */
      name[len-1] = 0;

      std::string entry = (br->size() % 2 == 0) ? white : yellow;
      entry.append(name);

      std::string path = current_dir;

      if (path.back() != '/') {
        path.push_back('/');
      }
      path.append(name);

      struct stat st;
      lstat(path.c_str(), &st);

      br->add(entry.c_str());
      br->icon(br->size(), S_ISLNK(st.st_mode) ? &icon_link_dir : &icon_dir);

      name[0] = 0;
    }

    /* list files */
    for (int i = 0; i < n; ++i) {
      std::string entry;
      char *name = sort_reverse ? list[n - 1 - i]->d_name : list[i]->d_name;

      if (name[0] == 0) {
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

      br->add(entry.c_str());
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

  if (infobox) {
    infobox->label(NULL);
  }
  input->value("");
}

void file_chooser_fltk::create_window(int mode)
{
  Fl_Button *bt_cancel;
  Fl_Group *g_top, *g_main, *g_bottom, *g_bottom_inside;
  Fl_Tile *tile;
  const int w = 800, h = 600;
  char buf[PATH_MAX] = {0};
  char *env, *resolved, *dir;

  list_files = (mode == FILE_CHOOSER);

  if ((env = getenv("HOME")) && strlen(env) > 0) {
    home_dir = std::string(env);

    /* make sure that it always ends on '/' */
    if (home_dir.back() != '/') {
      home_dir.push_back('/');
    }
  }

  current_dir = home_dir;

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
        o->image(list_ordered_1);
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
        tile = new Fl_Tile(10, 40, w - 20, h - g_top->h() - 76);
        const int d = 60;  /* resize distance */
        Fl_Box *r = new Fl_Box(tile->x() + d, tile->y() + d, tile->w() - 2*d, tile->h() - 2*d);
        {
          int sbW = 100;

          sidebar = new Fl_Hold_Browser(10, g_top->h(), sbW, h - g_top->h() - 76);
          sidebar->color(17);  /* yellow */
          sidebar->callback(sidebar_callback);

          SIDEBAR_LABELLINE("Places");
          sidebar->add("/", STR2VP("/"));
          sidebar->icon(sidebar->size(), &icon_hdd);
          sidebar->add("Home", STR2VP(home_dir.c_str()));
          sidebar->icon(sidebar->size(), &icon_home);

          /* file browser */
          br = new Fl_Hold_Browser(10 + sidebar->w(), g_top->h(), tile->w() - sidebar->w(), h - g_top->h() - 76);
          br->callback(br_callback);
        }
        tile->end();
        tile->resizable(r);

        g_bottom = new Fl_Group(0, br->y() + br->h(), w, h - br->y() - br->h());
        {
          const int bt_h = 26,
            bt_w1 = measure_button_width(fl_ok, 40),
            bt_w2 = measure_button_width(fl_cancel, 15),
            bt_w = (bt_w1 > bt_w2) ? bt_w1 : bt_w2;

          bt_ok = new Fl_Return_Button(w - bt_w - 10, br->y() + br->h() + 10, bt_w, bt_h, fl_ok);
          if (list_files) {
            bt_ok->deactivate();
          }
          bt_ok->callback(ok_cb);
          bt_cancel = new Fl_Button(bt_ok->x(), bt_ok->y() + bt_h + 5, bt_w, bt_h, fl_cancel);
          bt_cancel->callback(cancel_cb);

          g_bottom_inside = new Fl_Group(10, g_bottom->y(), w - bt_w - 30, g_bottom->h());
          {
            input = new Fl_Input(10, br->y() + br->h() + 10, bt_ok->x() - 20, bt_h);

            if (list_files) {
              infobox = new Fl_Box(10, input->y() + input->h() + 5, input->w(), bt_h);
              infobox->box(FL_THIN_DOWN_BOX);
              infobox->labelsize(12);
              infobox->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
            } else {
              new Fl_Box(FL_THIN_DOWN_BOX, 10, input->y() + input->h() + 5, input->w(), bt_h, NULL);
            }
          }
          g_bottom_inside->end();
        }
        g_bottom->resizable(g_bottom_inside);
        g_bottom->end();
      }
      g_main->resizable(tile);
      g_main->end();
    }
    g->resizable(g_main);
    g->end();
  }
  win->end();
}

char *file_chooser_fltk::get_selection(void) {
  return selected_file.empty() ? NULL : strdup(selected_file.c_str());
}

file_chooser_fltk::file_chooser_fltk(int mode, bool check_devices)
{
  create_window(mode);

  set_size(win, g);
  set_size_range(win, 360, 320);
  set_position(win);
  set_taskbar(win);

  br_change_dir();
  xdg_user_dir_lookup();
  get_gtk3_bookmarks();

  if (check_devices) {
    get_partitions();
  }

  win->show();
  set_undecorated(win);
  set_always_on_top(win);
}

file_chooser_fltk::~file_chooser_fltk()
{
  if (magicdb) {
    free(magicdb);
  }
}

char *file_chooser(int mode, bool check_devices)
{
  file_chooser_fltk *fc = new file_chooser_fltk(mode, check_devices);

  Fl::run();

  return fc->get_selection();
}

