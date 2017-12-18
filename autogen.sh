#!/bin/sh
set -e

if [ ! -e fltk/.git ]; then
  git submodule init
  git submodule update
fi

if [ ! -e librsvg/configure ]; then
  echo "downloading librsvg"
  rm -rf librsvg
  wget -c -q http://http.debian.net/debian/pool/main/libr/librsvg/librsvg_2.40.18.orig.tar.xz
  tar xf librsvg_2.40.18.orig.tar.xz
  rm -f librsvg_2.40.18.orig.tar.xz
  mv librsvg-2.40.18 librsvg
fi

rm -rf autom4te.cache
echo "running autoconf"
autoconf

