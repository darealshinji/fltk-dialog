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

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

#include <iostream>
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

/* https://www.qtcentre.org/threads/37629-Detecting-QIcon-failed-load-from-file */
#define QICON_LOADED(x)  (x.pixmap(QSize(1,1)).isNull() == false)

enum {
  FILE_CHOOSER,
  DIR_CHOOSER
};

static QApplication *appTray;
static QSystemTrayIcon *trayIcon;
static const char *command;
static bool listen, auto_close;
static pthread_t t1;


/* QIcon::setFallbackSearchPaths() is only available in Qt 5.11 or newer */
static void set_tray_icon(const char *iconName)
{
  const std::string sizes[] = {
    "scalable",
    "512x512", "256x256", "128x128", "64x64",
    "48x48", "32x32", "24x24", "22x22", "16x16"
  };
  const std::string exthic[] = { "", ".svg", ".svgz", ".png" };  /* hicolor */
  const std::string extpix[] = { "", ".svg", ".png", ".xpm" };   /* pixmaps */
  std::string path, path2;

  QIcon icon(iconName);

  if (QICON_LOADED(icon)) {
    trayIcon->setIcon(icon);
    return;
  }

  /* don't bother looking among the system icons if the icon name
   * was provided as a full or relative path */
  if (strchr(iconName, '/') != NULL) {
    trayIcon->setIcon(icon);  /* avoid "QSystemTrayIcon::setVisible: No Icon set" */
    return;
  }

  for (size_t i = 0; i < sizeof(sizes)/sizeof(*sizes); ++i) {
    path = "/usr/share/icons/hicolor/" + sizes[i] + "/apps/";
    path.append(iconName);

    for (int j = 0; j < 4; ++j) {
      path2 = path + exthic[j];

      QFileInfo file(path2.c_str());

      if (file.exists()) {
        //std::cout << "found: " << path2 << std::endl;
        QIcon iconHicolor(path2.c_str());

        if (QICON_LOADED(iconHicolor)) {
          trayIcon->setIcon(iconHicolor);
          return;
        }
      }
    }
  }

  path = "/usr/share/pixmaps/";
  path.append(iconName);

  for (int i = 0; i < 4; ++i) {
    path2 = path + extpix[i];

    QFileInfo file(path2.c_str());

    if (file.exists()) {
      //std::cout << "found: " << path2 << std::endl;
      QIcon iconPixmap(path2.c_str());

      if (QICON_LOADED(iconPixmap)) {
        trayIcon->setIcon(iconPixmap);
        return;
      }
    }
  }

  /* avoid "QSystemTrayIcon::setVisible: No Icon set" */
  trayIcon->setIcon(icon);
}

static void close_cb(void) {
  if (listen) {
    pthread_cancel(t1);
  }
  appTray->quit();
}

static void callback(void)
{
  if (command && strlen(command) > 0) {
    if (auto_close) {
      close_cb();
      execl("/bin/sh", "sh", "-c", command, NULL);
      _exit(127);
    } else {
      int i = system(command);
      static_cast<void>(i);
    }
  }
}

extern "C"
void *getline_qt(void *)
{
  std::string line;

  while (true) {
    if (std::getline(std::cin, line)) {
      if (strcasecmp(line.c_str(), "quit") == 0) {
        appTray->quit();
        return nullptr;
      } else if (line.length() > 5 && strcasecmp(line.substr(0,5).c_str(), "icon:") == 0) {
        set_tray_icon(line.substr(5).c_str());
      } else if (strcasecmp(line.c_str(), "run") == 0) {
        callback();
      }
      usleep(300000);  /* 300ms */
    }
  }
  return nullptr;
}

extern "C"
int start_indicator_qt(int /**/
,                      const char *command_
,                      const char *iconName
,                      bool listen_
,                      bool auto_close_
)
{
  char fake_argv0[] = "start_indicator_qt()";
  char *fake_argv[] = { fake_argv0 };
  int fake_argc = 1;

  command = command_;
  listen = listen_;
  auto_close = auto_close_;

  appTray = new QApplication(fake_argc, fake_argv);

  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    std::cerr << "error: couldn't detect any system tray" << std::endl;
    return 1;
  }

  trayIcon = new QSystemTrayIcon();
  QAction *runAction = new QAction("Run command", NULL);
  QAction *quitAction = new QAction("Quit", NULL);
  QMenu *menu = new QMenu();

  menu->addAction(runAction);
  menu->addAction(quitAction);
  trayIcon->setContextMenu(menu);
  set_tray_icon(iconName);

  QObject::connect(runAction, &QAction::triggered, &callback);
  QObject::connect(quitAction, &QAction::triggered, &close_cb);

  trayIcon->show();

  if (listen) {
    pthread_create(&t1, 0, &getline_qt, NULL);
  }

  return appTray->exec();
}

extern "C"
int getfilenameqt(int mode
,                 /* char separator
, */              const char *quote
,                 const char *title
,                 bool /**/
,                 bool /**/
)
{
  char fake_argv0[] = "getfilenameqt()";
  char *fake_argv[] = { fake_argv0, NULL };
  int fake_argc = 1, rv = 1;

  QApplication *app = new QApplication(fake_argc, fake_argv);
  QFileDialog *dialog = new QFileDialog(nullptr, title);
  dialog->setOption(QFileDialog::DontUseNativeDialog, true);

  if (mode == DIR_CHOOSER) {
    dialog->setFileMode(QFileDialog::Directory);
  } else {
    dialog->setFileMode(QFileDialog::ExistingFile);
    //dialog->setFileMode(QFileDialog::ExistingFiles);
  }

  if (dialog->exec()) {
    //QStringList strList;
    //QString s(separator);
    if (!quote) {
      /* just in case that quote happens to be NULL */
      quote = "";
    }
    //s = quote + s + quote;
    //strList << dialog->selectedFiles();
    //QString str = strList.join(s);
    //std::cout << quote << str.toUtf8().constData() << quote << std::endl;
    std::cout << quote << dialog->selectedFiles().at(0).toLocal8Bit().constData() << quote << std::endl;
    rv = 0;
  }

  app->quit();
  return rv;
}

