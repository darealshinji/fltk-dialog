ifneq ($(wildcard config.mak),)
include config.mak
endif

SYSTEM_JPEG      ?= no
SYSTEM_PNG       ?= no
SYSTEM_ZLIB      ?= yes
HAVE_QT          ?= no
HAVE_QT4         ?= no
HAVE_QT5         ?= no
WITH_NATIVE_FILE ?= no
WITH_NOTIFY      ?= no
WITH_RSVG        ?= no
DYNAMIC_NOTIFY   ?= yes
EMBEDDED_PLUGINS ?= yes

FLTK_VERSION = 1.3.4

BIN  = fltk-dialog
OBJS = $(addprefix src/,about.o calendar.o checklist.o color.o date.o dnd.o \
  dropdown.o FDate.o file.o Fl_Calendar.o Fl_Color_Chooser2.o Fl_Select_Browser2.o \
  font.o html.o l10n.o main.o message.o misc.o progress.o radiolist.o textinfo.o \
  version.o window_icon.o)



# default build flags
common_CFLAGS := -O2 -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 -ffunction-sections -fdata-sections
CFLAGS        ?= $(common_CFLAGS)
CXXFLAGS      ?= -std=c++11 $(common_CFLAGS)
LDFLAGS       ?= -s -Wl,-O1 -Wl,-z,defs -Wl,-z,relro -Wl,--as-needed -Wl,--gc-sections

# fltk-dialog build flags
main_CXXFLAGS := -Wall -Wextra -Wno-unused-parameter $(CXXFLAGS) $(CPPFLAGS)
main_CXXFLAGS += -I. -Isrc -Isrc/misc -Ifltk/build -Ifltk -DFLTK_VERSION=\"$(FLTK_VERSION)\"
main_CXXFLAGS += $(shell fltk/build/fltk-config --cxxflags 2>/dev/null | tr ' ' '\n' | grep '^-D.*')

# libfltk build flags
fltk_CFLAGS   := -Wall $(CFLAGS) $(CPPFLAGS) -Wno-unused-parameter -Wno-missing-field-initializers
fltk_CXXFLAGS := -Wall $(CXXFLAGS) $(CPPFLAGS) -Wno-unused-parameter -Wno-missing-field-initializers

ifneq ($(WITH_NATIVE_FILE),no)
main_CXXFLAGS += -DWITH_NATIVE_FILE_CHOOSER
endif
ifneq ($(HAVE_QT),no)
main_CXXFLAGS += -DHAVE_QT
OBJS          += src/file_dlopen_qtplugin.o
ifneq ($(HAVE_QT4),no)
main_CXXFLAGS += -DHAVE_QT4
endif
ifneq ($(HAVE_QT5),no)
main_CXXFLAGS += -DHAVE_QT5
endif
ifneq ($(EMBEDDED_PLUGINS),yes)
main_CXXFLAGS += -DUSE_SYSTEM_PLUGINS
main_CXXFLAGS += -DFLTK_DIALOG_MODULE_PATH=\"${libdir}/fltk-dialog\"
endif
endif
ifneq ($(WITH_NOTIFY),no)
main_CXXFLAGS += -DWITH_NOTIFY
OBJS          += src/notify.o
endif
ifneq ($(WITH_RSVG),no)
main_CXXFLAGS += -DWITH_WINDOW_ICON -DWITH_RSVG
OBJS          += src/window_icon_dlopen_rsvg_plugin.o
endif
ifneq ($(WITH_NOTIFY),no)
ifneq ($(DYNAMIC_NOTIFY),no)
main_CXXFLAGS += -DDYNAMIC_NOTIFY
main_LIBS     += -ldl
else
main_CXXFLAGS += $(shell pkg-config --cflags libnotify)
main_LIBS     += $(shell pkg-config --libs libnotify)
endif
endif

# Qt plugin CXXFLAGS
plugin_CXXFLAGS :=
plugin_CXXFLAGS += -fPIC -DPIC $(main_CXXFLAGS)

plugin_CFLAGS :=
plugin_CFLAGS += -fPIC -DPIC $(CFLAGS)


extra_include :=
extra_libdirs :=
fltk_cmake_config = \
  -DCMAKE_BUILD_TYPE="None" \
  -DCMAKE_CXX_COMPILER="$(CXX)" \
  -DCMAKE_C_COMPILER="$(CC)" \
  -DCMAKE_CXX_FLAGS="$(fltk_CXXFLAGS) $(extra_include)" \
  -DCMAKE_C_FLAGS="$(fltk_CFLAGS) $(extra_include)" \
  -DCMAKE_EXE_LINKER_FLAGS="$(LDFLAGS) $(extra_libdirs)" \
  -DOPTION_USE_GL="OFF" \
  -DOPTION_OPTIM="" \
  -DOPTION_BUILD_EXAMPLES="OFF"


main_LIBS += fltk/build/lib/libfltk_images.a

ifneq ($(SYSTEM_JPEG),no)
main_LIBS += -ljpeg
else
fltk_cmake_config += -DOPTION_USE_SYSTEM_LIBJPEG="OFF"
main_LIBS += fltk/build/lib/libfltk_jpeg.a
endif

ifneq ($(SYSTEM_PNG),no)
main_LIBS         += -lpng
else
fltk_cmake_config += -DOPTION_USE_SYSTEM_LIBPNG="OFF"
main_LIBS += fltk/build/lib/libfltk_png.a
endif

ifneq ($(SYSTEM_ZLIB),no)
main_LIBS += -lz
else
fltk_cmake_config += -DOPTION_USE_SYSTEM_ZLIB="OFF"
main_LIBS += fltk/build/lib/libfltk_z.a
endif

libfltk    = fltk/build/lib/libfltk.a
main_LIBS += $(libfltk) $(shell fltk/build/fltk-config --use-images --ldflags) -lm -lpthread

librsvg = librsvg/.libs/librsvg-2.a

ifeq ($(V),1)
cmake_verbose = VERBOSE=1
make_verbose  = 1
else
make_verbose  = 0
silent        = @
endif

msg_GENH    = @echo "Generating header file $@"
msg_CC      = @echo "Building CC object $@"
msg_CCLDSO  = @echo "Linking CC shared object $@"
msg_CXX     = @echo "Building CXX object $@"
msg_CXXLD   = @echo "Linking CXX executable $@"
msg_CXXLDSO = @echo "Linking CXX shared object $@"

CMAKE ?= cmake

define MAKE_CLEAN
  [ ! -f fltk/makeinclude ] || $(MAKE) -C fltk $@
  [ ! -f fltk/build/Makefile ] || $(MAKE) -C fltk/build clean
  [ ! -f librsvg/Makefile ] || $(MAKE) -C librsvg $@
endef



all: $(BIN)

clean: mostlyclean
	$(MAKE_CLEAN)

distclean: mostlyclean
	-rm -rf fltk/build autom4te.cache
	-rm -f aclocal.m4 config.mak config.log config.status
	$(MAKE_CLEAN)
	[ ! -f fltk/src/Fl_Choice.cxx.orig ] || mv -f fltk/src/Fl_Choice.cxx.orig fltk/src/Fl_Choice.cxx
	[ ! -f fltk/FL/Fl_Help_Dialog.H.orig ] || mv -f fltk/FL/Fl_Help_Dialog.H.orig fltk/FL/Fl_Help_Dialog.H
	[ ! -f fltk/src/Fl_Help_Dialog.cxx.orig ] || mv -f fltk/src/Fl_Help_Dialog.cxx.orig fltk/src/Fl_Help_Dialog.cxx

mostlyclean:
	-rm -f $(BIN) *.so *_so.h *_png.h *.o src/*.o src/Flek/*.o src/misc/*.o

maintainer-clean: distclean
	-rm -f configure
	-rm -rf librsvg

ifneq ($(WITH_RSVG),no)
$(OBJS): $(libfltk) $(librsvg)
else
$(OBJS): $(libfltk)
endif

$(BIN): $(OBJS)
	$(msg_CXXLD)
	$(silent)$(CXX) -o $@ $^ $(LDFLAGS) $(main_LIBS) $(LIBS)

.cpp.o:
	$(msg_CXX)
	$(silent)$(CXX) $(main_CXXFLAGS) -c -o $@ $<


fltk/build/fltk-config: $(libfltk)

fltk/src/Fl_Choice.cxx.orig:
	patch -p1 --backup < patches/Fl_Choice-pulldown.patch

fltk/src/Fl_Help_Dialog.cxx.orig:
	patch -p1 --backup < patches/Fl_Help_Dialog-close+nodeco.patch

fltk/build/Makefile: fltk/src/Fl_Choice.cxx.orig fltk/src/Fl_Help_Dialog.cxx.orig
	mkdir -p fltk/build
	cd fltk/build && $(CMAKE) .. $(fltk_cmake_config) -DCMAKE_VERBOSE_MAKEFILE="OFF"

$(libfltk): $(libpng_a) fltk/build/Makefile
	$(MAKE) -C fltk/build $(cmake_verbose)

src/about.cpp: fltk_png.h
src/main.cpp: icon_png.h

fltk_png.h: src/fltk.png
	$(msg_GENH)
	$(silent)xxd -i $< > $@

icon_png.h: src/icon.png
	$(msg_GENH)
	$(silent)xxd -i $< > $@

ifneq ($(HAVE_QT),no)
qtplugins =
ifneq ($(HAVE_QT4),no)
qtplugins += qt4gui.so
endif
ifneq ($(HAVE_QT5),no)
qtplugins += qt5gui.so
endif

qtgui_so.h: $(qtplugins)
	$(msg_GENH)
	$(silent)rm -f $@
ifneq ($(HAVE_QT4),no)
	$(silent)xxd -i qt4gui.so >> $@
endif
ifneq ($(HAVE_QT5),no)
	$(silent)xxd -i qt5gui.so >> $@
endif

qt4gui.so: src/file_qtplugin_qt4.o
	$(msg_CXXLDSO)
	$(silent)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs QtGui QtCore)

qt5gui.so: src/file_qtplugin_qt5.o
	$(msg_CXXLDSO)
	$(silent)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs Qt5Widgets Qt5Core)

src/file_qtplugin_qt4.o: src/file_qtplugin.cpp
	$(msg_CXX)
	$(silent)$(CXX) $(plugin_CXXFLAGS) -Wno-unused-variable $(shell pkg-config --cflags QtGui QtCore) -c -o $@ $<

src/file_qtplugin_qt5.o: src/file_qtplugin.cpp
	$(msg_CXX)
	$(silent)$(CXX) $(plugin_CXXFLAGS) $(shell pkg-config --cflags Qt5Widgets Qt5Core) -c -o $@ $<

rsvg_convert_so.h: rsvg_convert.so
	$(msg_GENH)
	$(silent)xxd -i $< > $@

rsvg_modules = glib-2.0 gio-2.0 gdk-pixbuf-2.0 cairo pangocairo libxml-2.0 libcroco-0.6

rsvg_convert.so: src/rsvg_convert.o rsvg-size-callback.o $(librsvg)
	$(msg_CCLDSO)
	$(silent)$(CC) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs $(rsvg_modules)) -lm

src/rsvg_convert.c: $(librsvg)
src/rsvg_convert.o: src/rsvg_convert.c
	$(msg_CC)
	$(silent)$(CC) -I librsvg $(plugin_CFLAGS) -Wno-deprecated-declarations $(shell pkg-config --cflags $(rsvg_modules)) -c -o $@ $<

rsvg-size-callback.o: librsvg/rsvg-size-callback.c
	$(msg_CC)
	$(silent)$(CC) -I librsvg $(plugin_CFLAGS) $(shell pkg-config --cflags $(rsvg_modules)) -c -o $@ $<

$(librsvg): librsvg/Makefile
	$(MAKE) -C librsvg V=$(make_verbose)

librsvg/Makefile:
	cd librsvg && ./configure --disable-shared --disable-introspection --disable-pixbuf-loader --with-pic

ifneq ($(EMBEDDED_PLUGINS),no)
src/file_dlopen_qtplugin.o: qtgui_so.h
src/window_icon_dlopen_rsvg_plugin.o: rsvg_convert_so.h
else
src/file_dlopen_qtplugin.o: $(qtplugins)
src/window_icon_dlopen_rsvg_plugin.o: rsvg_convert.so
endif

src/file_qtplugin.cpp: $(libfltk)
endif


DISTFILES = fltk/ librsvg/ patches/ src/ \
	ax_check_compile_flag.m4 \
	ax_cxx_compile_stdcxx.m4 \
	config.mak.in \
	configure \
	configure.ac \
	COPYING.LGPL-2 \
	LICENSE \
	Makefile \
	README.md

DISTDIR = fltk-dialog-src

dist: distclean
	-rm -rf $(DISTDIR)
	-rm -f $(DISTDIR).tar.xz
	mkdir $(DISTDIR)
	cp -r $(DISTFILES) $(DISTDIR)
	cd $(DISTDIR) && autoconf
	-rm -rf $(DISTDIR)/autom4te.cache
	tar cfJ $(DISTDIR).tar.xz $(DISTDIR)
	-rm -rf $(DISTDIR)

