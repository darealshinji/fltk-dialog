fltk-dialog
===========

This is a simple [FLTK](http://www.fltk.org/)-based dialog application.

Minimum build dependencies are: `libx11-dev libxcursor-dev libxext-dev libxfixes-dev libxinerama-dev cmake`

For better looking fonts it's recommended to install `libxft-dev` too.
To enable native Qt file choosers you must install `libqt4-dev` and/or `qtbase5-dev`.

If you don't want dynamic notification support, install `libnotify-dev` and run `./configure --disable-dynamic-notify`.

