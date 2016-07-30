fltk-dialog
===========

This is a simple [FLTK](http://www.fltk.org/)-based dialog application.
It's in a very early stage. If you need something more stable and versatile
I recommend using [YAD](https://sourceforge.net/projects/yad-dialog/).

Note: the Makefile will automatically download and compile FLTK.

Minimum build dependencies are `libx11-dev patch`.
For better looking fonts it's recommended to install `libxft-dev` too.
Install `libjpeg-dev libpng-dev zlib1g-dev` to link against the system's jpeg, png and zlib libraries.
For more portability you can force the usage of local embedded libraries by running `make`
with `LOCAL_JPEG=1`, `LOCAL_PNG=1` and/or `LOCAL_ZLIB=1`.
