ifneq ($(wildcard config.mak),)
include config.mak
endif

SYSTEM_JPEG      ?= no
SYSTEM_PNG       ?= no
SYSTEM_ZLIB      ?= yes
HAVE_QT          ?= no
WITH_FRIBIDI     ?= no
SYSTEM_FRIBIDI   ?= no
DYNAMIC_NOTIFY   ?= yes
DYNAMIC_MAGIC    ?= yes
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
	src/file.o \
	src/file_fltk.o \
	src/font.o \
	src/html.o \
	src/img_to_rgb.o \
	src/indicator.o \
	src/indicator_gtk.o \
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
	$(NULL)

libfltk   = fltk/build/lib/libfltk.a
OBJS_deps = $(libfltk)


# default build flags
CFLAGS   ?= -O3 -Wall
CXXFLAGS ?= -O3 -Wall -std=gnu++11

# fltk-dialog build flags
main_CFLAGS   := -Wall -Wextra -Wshadow $(CFLAGS) $(CPPFLAGS) -I. -Isrc
main_CXXFLAGS := -Wall -Wextra -Wshadow $(CXXFLAGS) $(CPPFLAGS)
main_CXXFLAGS += -I. -Isrc -Isrc/misc -Ifltk/build -Ifltk -DFLTK_STATIC
main_CXXFLAGS += $(shell fltk/build/fltk-config --cxxflags 2>/dev/null | tr ' ' '\n' | grep '^-D.*')

# libfltk build flags
fltk_CFLAGS   := -Wall $(CFLAGS) $(CPPFLAGS)
fltk_CXXFLAGS := -Wall $(CXXFLAGS) $(CPPFLAGS)

ifneq ($(HAVE_QT),no)
main_CXXFLAGS += -DHAVE_QT
endif
ifneq ($(EMBEDDED_PLUGINS),yes)
main_CXXFLAGS += -DUSE_SYSTEM_PLUGINS
main_CXXFLAGS += -DFLTK_DIALOG_MODULE_PATH=\"${libdir}/fltk-dialog\"
endif
ifneq ($(DYNAMIC_NOTIFY),no)
main_CXXFLAGS += -DDYNAMIC_NOTIFY
else
main_CXXFLAGS += $(shell pkg-config --cflags libnotify)
main_LIBS     += $(shell pkg-config --libs libnotify)
endif
ifneq ($(DYNAMIC_MAGIC),no)
main_CXXFLAGS += -DDYNAMIC_MAGIC
else
main_LIBS     += -lmagic
endif

ifneq ($(WITH_FRIBIDI),no)
main_CXXFLAGS += -DWITH_FRIBIDI
OBJS          += src/fribidi_parse_line.o
ifneq ($(SYSTEM_FRIBIDI),no)
main_CXXFLAGS += -DSYSTEM_FRIBIDI $(shell pkg-config --cflags fribidi)
main_LIBS     += $(shell pkg-config --libs fribidi)
else
main_CXXFLAGS += -Ifribidi/lib
libfribidi     = fribidi/lib/.libs/libfribidi.a
main_LIBS     += $(libfribidi)
OBJS_deps     += $(libfribidi)
endif
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
main_LIBS += -lpng
else
fltk_cmake_config += -DOPTION_USE_SYSTEM_LIBPNG="OFF"
main_CXXFLAGS += -Ifltk/png
main_LIBS += fltk/build/lib/libfltk_png.a
endif

ifneq ($(SYSTEM_ZLIB),no)
main_LIBS += -lz
else
fltk_cmake_config += -DOPTION_USE_SYSTEM_ZLIB="OFF"
main_LIBS += fltk/build/lib/libfltk_z.a
endif

main_LIBS += $(libfltk)
main_LIBS += $(shell fltk/build/fltk-config --use-images --ldflags | sed -e 's| -lfltk_images||; s| -lpng||; s| -lz||; s| -lfltk||')
main_LIBS += -ldl -lm -lpthread

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
  [ ! -f fribidi/Makefile ] || $(MAKE) -C fribidi $@
endef

define NL


endef



all: $(BIN)

clean: mostlyclean
	$(MAKE_CLEAN)

distclean: mostlyclean
	-rm -rf fltk/build autom4te.cache
	-rm -f aclocal.m4 config.mak config.log config.status fribidi/gen.tab/fribidi-unicode-version.h
	$(MAKE_CLEAN)
	test ! -f fltk/patches_applied_stamp || (cd fltk && patch -p1 -R < ../fltk_patches.diff && rm patches_applied_stamp)

mostlyclean:
	-rm -f $(BIN) *.so *_so.h *_png.h *.o src/*.o

maintainer-clean: distclean
	-rm -f configure

$(OBJS): $(OBJS_deps)

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

$(libfltk): fltk/build/Makefile
	$(MAKE) -C fltk/build fltk_images $(cmake_verbose)

src/about.cpp: fltk_png.h
src/file_fltk.cpp: octicons_png.h
src/indicator.cpp: icon_png.h image_missing_png.h
src/main.cpp: icon_png.h

OCTICONS = src/octicons/arrow-up-gray.png \
	src/octicons/arrow-up.png \
	src/octicons/eye.png \
	src/octicons/file-directory.png \
	src/octicons/file.png \
	src/octicons/file-symlink-directory.png \
	src/octicons/file-symlink-file.png

fltk_png.h: src/fltk.png
	$(msg_GENH)
	$(Q)xxd -i $< > $@

icon_png.h: src/icon.png
	$(msg_GENH)
	$(Q)xxd -i $< > $@ && sed -i 's|^unsigned |static const unsigned |g' $@

image_missing_png.h: src/image-missing.png
	$(msg_GENH)
	$(Q)xxd -i $< > $@

octicons_png.h: $(OCTICONS)
	$(msg_GENH)
	$(Q)rm -f $@ $@_
	$(Q)$(foreach ICON,$^,xxd -i $(ICON) >> $@_; )
	$(Q)mv $@_ $@

ifneq ($(WITH_FRIBIDI),no)
ifeq ($(SYSTEM_FRIBIDI),no)
fribidi/configure:
	autoreconf -if fribidi

fribidi/Makefile: fribidi/configure
	cd fribidi && ./configure --disable-shared --disable-deprecated

$(libfribidi): $(libfltk) fribidi/Makefile
	$(MAKE) -C fribidi V=$(make_verbose) dist_man_MANS='' dist_noinst_MANS=''
endif
endif

ifneq ($(HAVE_QT),no)
include qt.mak
endif


