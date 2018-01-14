/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018, djcj <djcj@gmx.de>
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

/* Qt4:
 * CXXFLAGS += -std=c++0x -fPIC $(pkg-config --cflags QtGui QtCore)
 * LDFLAGS += -shared -Wl,--as-needed $(pkg-config --libs QtGui QtCore)
 *
 * Qt5:
 * CXXFLAGS += -std=c++0x -fPIC $(pkg-config --cflags Qt5Widgets Qt5Core)
 * LDFLAGS += -shared -Wl,--as-needed $(pkg-config --libs Qt5Widgets Qt5Core)
 */

#include <iostream>
#include <QApplication>
#include <QFileDialog>
#include <QIcon>

#include <QtGlobal>
#if QT_VERSION >= 0x050000
# include "icon_qrc_qt5.cpp"
#else
# include "icon_qrc_qt4.cpp"
#endif

#include "fltk-dialog.hpp"


extern "C"
int getfilenameqt(int mode, const char *separator, const char *title, int argc, char **argv)
{
  QScopedPointer<QCoreApplication> app(new QApplication(argc, argv));
  QFileDialog *dialog = new QFileDialog();

  dialog->setWindowIcon(QIcon(":/icon.png"));  /* not shown in Qt5? */
  dialog->setWindowTitle(title);

  if (mode == DIR_CHOOSER) {
    dialog->setFileMode(QFileDialog::Directory);
  } else /* if (mode == FILE_CHOOSER) */ {
    dialog->setFileMode(QFileDialog::ExistingFiles);
  }

  if (dialog->exec()) {
    QStringList strList;
    strList << dialog->selectedFiles();
    QString str = strList.join(QString::fromLatin1(separator));
    std::cout << str.toUtf8().constData() << std::endl;
    delete dialog;
    return 0;
  }
  delete dialog;
  return 1;
}

