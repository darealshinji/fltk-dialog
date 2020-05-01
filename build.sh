#!/bin/sh
set -e
set -x

JOBS=4
DEF_CFLAGS="-Wall -O3 -ffunction-sections -fdata-sections -I/usr/include/harfbuzz"
DEF_CXXFLAGS="$DEF_CFLAGS -std=c++11"
DEF_LDFLAGS="-Wl,-O1 -Wl,--gc-sections -Wl,-z,defs -Wl,--as-needed"

external_plugins=""
use_dlopen="yes"
syslibs="ON"
if [ "x$1" = "x--external-plugins" ]; then
  external_plugins="yes"
elif [ "x$1" = "x--disable-dlopen" ]; then
  use_dlopen=""
  syslibs="OFF"
fi

mkdir -p build


if [ -d .git ]; then
  git submodule init
  git submodule update
fi


### patch fltk ###
if [ ! -e fltk/patches_applied_stamp ]; then
  cd fltk
  patch -p1 < ../fltk_patches.diff
  touch patches_applied_stamp
  cd -
fi


### build fltk ###
if [ ! -e build/fltk/lib/libfltk_images.a ]; then
  mkdir -p build/fltk
  cd build/fltk
  git rev-parse --short HEAD > fltk_git_hash

  cmake ../../fltk -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_CXX_FLAGS="$DEF_CXXFLAGS" \
    -DCMAKE_C_FLAGS="$DEF_CFLAGS" \
    -DOPTION_USE_GL="OFF" \
    -DOPTION_BUILD_EXAMPLES="OFF" \
    -DOPTION_PRINT_SUPPORT="OFF" \
    -DOPTION_USE_NANOSVG="ON" \
    -DOPTION_USE_PANGO="ON" \
    -DOPTION_CAIRO="OFF" \
    -DOPTION_USE_SYSTEM_LIBJPEG="$syslibs" \
    -DOPTION_USE_SYSTEM_LIBPNG="$syslibs" \
    -DOPTION_USE_SYSTEM_ZLIB="$syslibs"
  make -j$JOBS fltk_images
  cd -
fi


### build fltk-dialog ###

cd build
rm -rf fltk_dialog AppDir
mkdir -p fltk_dialog

if [ -f fltk/fltk_git_hash ]; then
  define_git_hash="-DFLTK_GIT_HASH=$(cat fltk/fltk_git_hash)"
fi

V=1 \
USE_EXTERNAL_PLUGINS="$external_plugins" \
USE_DLOPEN="$use_dlopen" \
CXXFLAGS="$DEF_CXXFLAGS -I$PWD/fltk -I$PWD/../fltk $(./fltk/bin/fltk-config --use-images --cxxflags) $define_git_hash" \
LDFLAGS="$DEF_LDFLAGS -L$PWD/fltk/lib $(./fltk/bin/fltk-config --use-images --ldflags) -lmagic" \
QT_CXXFLAGS="$DEF_CXXFLAGS $(pkg-config --cflags Qt5Widgets Qt5Core)" \
QT_LDFLAGS="$DEF_LDFLAGS $(pkg-config --libs Qt5Widgets Qt5Core)" \
BUILDDIR="$PWD/fltk_dialog" \
SOURCEDIR="$PWD/../src" \
  make -j$JOBS -f ../src/Makefile

strip --strip-all fltk_dialog/fltk-dialog
readelf -d fltk_dialog/fltk-dialog | grep '(NEEDED)'
cd -


### undo patching ###
cd fltk
patch -p1 -R < ../fltk_patches.diff
rm patches_applied_stamp


### bundling ###
exit 0
if [ "x$1" = "x--disable-dlopen" ]; then
  cd ../build

  cp -f ../src/icon.png fltk-dialog.png
  mkdir -p AppDir/usr/share/file
  cp -f /usr/share/file/magic.mgc AppDir/usr/share/file

  wget -c https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
  chmod a+x *.AppImage
  ./linuxdeploy-x86_64.AppImage -oappimage --appdir=AppDir --create-desktop-file -ifltk-dialog.png -efltk_dialog/fltk-dialog
fi

