#!/bin/sh
set -e

if [ ! -e fltk/.git ]; then
  git submodule init
  git submodule update
fi

if [ ! -e fribidi/configure ]; then
  autoreconf -if fribidi
fi

rm -rf autom4te.cache

set -x
autoconf -f

