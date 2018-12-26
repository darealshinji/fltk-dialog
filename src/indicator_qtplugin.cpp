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

#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

#include <iostream>
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

static QApplication *app;
static QSystemTrayIcon *trayIcon;
static const char *command;
static bool listen, auto_close;
static pthread_t thr;

static void close_cb(void) {
  if (listen) {
    pthread_cancel(thr);
  }
  app->quit();
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
      Q_UNUSED(i);
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
        app->quit();
        return nullptr;
      } else if (line.length() > 5 && strcasecmp(line.substr(0,5).c_str(), "icon:") == 0) {
        trayIcon->setIcon(QIcon(line.substr(5).c_str()));
      }
      usleep(300000);  /* 300ms */
    }
  }
  return nullptr;
}

extern "C"
int start_indicator_qt(const char *command_, const char *icon, bool listen_, bool auto_close_)
{
  char fake_argv0[] = "start_indicator_qt()";
  char *fake_argv[] = { fake_argv0 };
  int fake_argc = 1;

  command = command_;
  listen = listen_;
  auto_close = auto_close_;

  app = new QApplication(fake_argc, fake_argv);

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
  trayIcon->setIcon(QIcon(icon));
  trayIcon->setContextMenu(menu);

  QObject::connect(runAction, &QAction::triggered, &callback);
  QObject::connect(quitAction, &QAction::triggered, &close_cb);

  trayIcon->show();

  if (listen) {
    pthread_create(&thr, 0, &getline_qt, NULL);
  }

  return app->exec();
}
