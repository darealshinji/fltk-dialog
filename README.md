fltk-dialog
===========

This is a simple [FLTK](http://www.fltk.org/)-based dialog application.
It's in an early stage. If you need something more stable and versatile
I recommend using [YAD](https://sourceforge.net/projects/yad-dialog/).

Note: the Makefile will automatically download and compile FLTK.

Minimum build dependencies are `libx11-dev` and `cmake`.
For better looking fonts it's recommended to install `libxft-dev` too.

Configuration:
You can disable certain features, see `Makefile` for details.
You can write your settings in a `config.mak` file.
This file will automatically be included to override the default settings.
