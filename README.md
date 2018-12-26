# fltk-dialog [![Build Status](https://travis-ci.org/darealshinji/fltk-dialog.svg?branch=master)](https://travis-ci.org/darealshinji/fltk-dialog)

This is a simple [FLTK](http://www.fltk.org/)-based dialog application.

Minimum build dependencies are: `libx11-dev libxcursor-dev libxext-dev libxfixes-dev libxft-dev libxinerama-dev cmake`

You also need to install `xxd` or `vim-common` if the xxd tool isn't available in its own package.

To enable the native Qt5 file chooser you must install `qtbase5-dev`.

If you don't want dynamic notification support, install `libnotify-dev` and run configure with `--disable-dynamic-notify`.

First initialize the submodules with `git submodule init && git submodule update`.
You can run `make` without running `./configure` first. This will build fltk-dialog with default build settings.
