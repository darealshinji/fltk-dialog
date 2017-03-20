# fltk-dialog [![Build Status](https://travis-ci.org/darealshinji/fltk-dialog.svg?branch=master)](https://travis-ci.org/darealshinji/fltk-dialog)

This is a simple [FLTK](http://www.fltk.org/)-based dialog application.

Minimum build dependencies are: `libx11-dev libxcursor-dev libxext-dev libxfixes-dev libxft-dev libxinerama-dev cmake`

To enable native Qt file choosers you must install `libqt4-dev` and/or `qtbase5-dev`.

If you don't want dynamic notification support, install `libnotify-dev` and run `./configure --disable-dynamic-notify`.

You can run `make` without running `./configure` first. This will build fltk-dialog with default build settings.
