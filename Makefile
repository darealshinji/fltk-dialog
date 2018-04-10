ifneq ($(wildcard config.mak),)
include config.mak
endif

SYSTEM_JPEG      ?= no
SYSTEM_PNG       ?= no
SYSTEM_ZLIB      ?= yes
HAVE_QT          ?= no
HAVE_QT4         ?= no
HAVE_QT5         ?= no
WITH_RSVG        ?= no
WITH_FRIBIDI     ?= no
DYNAMIC_NOTIFY   ?= yes
EMBEDDED_PLUGINS ?= yes

BIN  = fltk-dialog

OBJS = \
	src/about.o \
	src/calendar.o \
	src/checklist.o \
	src/color.o \
	src/date.o \
	src/dnd.o \
	src/dropdown.o \
	src/FDate.o \
	src/file.o \
	src/Fl_Calendar.o \
	src/font.o \
	src/html.o \
	src/img_to_rgb.o \
	src/l10n.o \
	src/main.o \
	src/message.o \
	src/misc.o \
	src/nanosvg.o \
	src/notify.o \
	src/progress.o \
	src/radiolist.o \
	src/radiolist_browser.o \
	src/textinfo.o \
	src/version.o \
	$(NULL)



# default build flags
CFLAGS   ?= -O3 -Wall
CXXFLAGS ?= -O3 -Wall -std=c++11

# fltk-dialog build flags
main_CXXFLAGS := -Wall -Wextra -Wno-unused-parameter $(CXXFLAGS) $(CPPFLAGS)
main_CXXFLAGS += -I. -Isrc -Isrc/misc -Ifltk/build -Ifltk -DFLTK_STATIC
main_CXXFLAGS += $(shell fltk/build/fltk-config --cxxflags 2>/dev/null | tr ' ' '\n' | grep '^-D.*')

# libfltk build flags
fltk_CFLAGS   := -Wall $(CFLAGS) $(CPPFLAGS) -Wno-unused-parameter -Wno-missing-field-initializers
fltk_CXXFLAGS := -Wall $(CXXFLAGS) $(CPPFLAGS) -Wno-unused-parameter -Wno-missing-field-initializers

ifneq ($(HAVE_QT),no)
main_CXXFLAGS += -DHAVE_QT
ifneq ($(HAVE_QT4),no)
main_CXXFLAGS += -DHAVE_QT4
endif
ifneq ($(HAVE_QT5),no)
main_CXXFLAGS += -DHAVE_QT5
endif
endif # HAVE_QT
ifneq ($(EMBEDDED_PLUGINS),yes)
main_CXXFLAGS += -DUSE_SYSTEM_PLUGINS
main_CXXFLAGS += -DFLTK_DIALOG_MODULE_PATH=\"${libdir}/fltk-dialog\"
endif
ifneq ($(WITH_RSVG),no)
main_CXXFLAGS += -DWITH_RSVG
OBJS          += src/dlopen_rsvg_plugin.o
endif
ifneq ($(DYNAMIC_NOTIFY),no)
main_CXXFLAGS += -DDYNAMIC_NOTIFY
main_LIBS     += -ldl
else
main_CXXFLAGS += $(shell pkg-config --cflags libnotify)
main_LIBS     += $(shell pkg-config --libs libnotify)
endif
ifneq ($(WITH_FRIBIDI),no)
main_CXXFLAGS += -DWITH_FRIBIDI -Ifribidi/lib
libfribidi     = fribidi/lib/.libs/libfribidi.a
main_LIBS     += $(libfribidi)
endif

plugin_CXXFLAGS :=
plugin_CXXFLAGS += -fPIC -DPIC $(main_CXXFLAGS)


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

ifneq ($(WITH_FRIBIDI),no)
OBJS_deps += $(libfribidi)
endif

ifneq ($(WITH_RSVG),no)
librsvg    = librsvg/.libs/librsvg-2.a
OBJS_deps += $(librsvg)
endif

ifeq ($(V),1)
cmake_verbose = VERBOSE=1
make_verbose  = 1
Q =
else
cmake_verbose =
make_verbose  = 0
Q = @
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
  [ ! -f fribidi/Makefile ] || $(MAKE) -C fribidi $@
endef

define NL


endef



all: $(BIN)

clean: mostlyclean
	$(MAKE_CLEAN)

distclean: mostlyclean
	-rm -rf fltk/build autom4te.cache
	-rm -f aclocal.m4 config.mak config.log config.status
	$(MAKE_CLEAN)
	test ! -f fltk/patches_applied_stamp || (cd fltk && patch -p1 -R < ../fltk_patches.diff && rm patches_applied_stamp)

mostlyclean:
	-rm -f $(BIN) *.so *_so.h *_png.h *_qrc.h *.o src/*.o src/Flek/*.o src/misc/*.o

maintainer-clean: distclean
	-rm -f configure
	-rm -rf librsvg

$(OBJS): $(OBJS_deps)

$(libfribidi):
	$(MAKE) -C fribidi V=$(make_verbose) dist_man_MANS='' dist_noinst_MANS=''

$(BIN): $(OBJS)
	$(msg_CXXLD)
	$(Q)$(CXX) -o $@ $^ $(LDFLAGS) $(main_LIBS) $(LIBS)

.c.o:
	$(msg_CC)
	$(Q)$(CC) $(main_CFLAGS) -c -o $@ $<

.cpp.o:
	$(msg_CXX)
	$(Q)$(CXX) $(main_CXXFLAGS) -c -o $@ $<


fltk/build/fltk-config: $(libfltk)

fltk/patches_applied_stamp: fltk_patches.diff
	cd fltk && patch -p1 < ../$< && touch ../$@

fltk/build/Makefile: fltk/patches_applied_stamp
	mkdir -p fltk/build
	cd fltk/build && $(CMAKE) .. $(fltk_cmake_config) -DCMAKE_VERBOSE_MAKEFILE="OFF"

$(libfltk): $(libpng_a) fltk/build/Makefile
	$(MAKE) -C fltk/build $(cmake_verbose)

src/about.cpp: fltk_png.h
src/main.cpp: icon_png.h

fltk_png.h: src/fltk.png
	$(msg_GENH)
	$(Q)xxd -i $< > $@

icon_png.h: src/icon.png
	$(msg_GENH)
	$(Q)xxd -i $< > $@

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
	$(Q)rm -f $@
ifneq ($(HAVE_QT4),no)
	$(Q)xxd -i qt4gui.so >> $@
endif
ifneq ($(HAVE_QT5),no)
	$(Q)xxd -i qt5gui.so >> $@
endif

qt4gui.so: src/file_qtplugin_qt4.o
	$(msg_CXXLDSO)
	$(Q)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs QtGui QtCore)

qt5gui.so: src/file_qtplugin_qt5.o
	$(msg_CXXLDSO)
	$(Q)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs Qt5Widgets Qt5Core)

src/file_qtplugin_qt4.o: src/file_qtplugin.cpp icon_qrc.h
	$(msg_CXX)
	$(Q)$(CXX) $(plugin_CXXFLAGS) -Wno-unused-variable $(shell pkg-config --cflags QtGui QtCore) -c -o $@ $<

src/file_qtplugin_qt5.o: src/file_qtplugin.cpp icon_qrc.h
	$(msg_CXX)
	$(Q)$(CXX) $(plugin_CXXFLAGS) $(shell pkg-config --cflags Qt5Widgets Qt5Core) -c -o $@ $<

icon_qrc.h: src/icon.png
	$(msg_GENH)
	$(Q)printf "static const unsigned char qt_resource_data[] = {\n " > $@
	$(Q)printf "%08x" $$(wc -c < $<) | sed 's|.\{2\}| 0x&,|g' >> $@
	$(Q)printf "\n" >> $@
	$(Q)xxd -i < $< >> $@
	$(Q)printf "};\n" >> $@

ifneq ($(EMBEDDED_PLUGINS),no)
src/file_dlopen_qtplugin.o: qtgui_so.h
else
src/file_dlopen_qtplugin.o: $(qtplugins)
endif

src/file.cpp: qtgui_so.h
src/file_qtplugin.cpp: $(libfltk)
endif # HAVE_QT

ifneq ($(WITH_RSVG),no)
rsvg_convert_so.h: rsvg_convert.so
	$(msg_GENH)
	$(Q)xxd -i $< > $@

rsvg_modules = glib-2.0 gio-2.0 gdk-pixbuf-2.0 cairo pangocairo libxml-2.0 libcroco-0.6

rsvg_convert.so: src/rsvg_convert.o $(librsvg)
	$(msg_CCLDSO)
	$(Q)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs $(rsvg_modules)) -lm

src/rsvg_convert.cpp: $(librsvg)
src/rsvg_convert.o: src/rsvg_convert.cpp
	$(msg_CC)
	$(Q)$(CXX) -I librsvg $(plugin_CXXFLAGS) -Wno-deprecated-declarations $(shell pkg-config --cflags $(rsvg_modules)) -c -o $@ $<

$(librsvg): librsvg/Makefile
	$(MAKE) -C librsvg V=$(make_verbose)

librsvg/Makefile:
	cd librsvg && ./configure --disable-shared --disable-introspection --disable-pixbuf-loader --with-pic

ifneq ($(EMBEDDED_PLUGINS),no)
src/dlopen_rsvg_plugin.o: rsvg_convert_so.h
else
src/dlopen_rsvg_plugin.o: rsvg_convert.so
endif

endif # WITH_RSVG


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

