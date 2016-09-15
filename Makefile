ifneq ($(wildcard config.mak),)
include config.mak
endif

# set to "yes" if you want to link
# against system libraries
SYSTEM_JPEG ?= no
SYSTEM_PNG  ?= no
SYSTEM_ZLIB ?= no

# yes: FLTK version string will be set statically;
#   don't use this on shared libraries!
# no: version string will be obtained dynamically
#   from the linked in library; works with statically
#   linked FLTK too, but makes more sense on shared libs
# TODO: actually use the system libs or build shared
#   libs if disabled
#STATIC_FLTK   ?= yes

# set to "no" if you don't want an embedded FLKT
# icon to appear in taskbar and windows
WITH_DEFAULT_ICON ?= yes

# set to "no" to disable certain features
WITH_CALENDAR    ?= yes
WITH_COLOR       ?= yes
WITH_DND         ?= yes
WITH_ENTRY       ?= yes
WITH_FILE        ?= yes
WITH_HTML        ?= yes
WITH_NOTIFY      ?= yes
WITH_PASSWORD    ?= yes
WITH_PROGRESS    ?= yes
WITH_SCALE       ?= yes
WITH_TEXTINFO    ?= yes
WITH_WINDOW_ICON ?= yes

DYNAMIC_NOTIFY ?= yes

# checkout directory for FLTK
fltk = fltk-1.3

libpng_version = 1.6.25
libpng_tarball = libpng-$(libpng_version).tar.xz

BIN = fltk-dialog
OBJS = $(addprefix src/,about.o choice.o message.o translate.o version.o main.o)

OPT ?= -Os

common_CFLAGS := $(OPT) -Wall -Wextra \
 -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 \
 -ffunction-sections -fdata-sections

LDFLAGS += \
 -s -Wl,-O1 -Wl,-z,defs -Wl,-z,relro -Wl,--as-needed -Wl,--gc-sections

CXXFLAGS += $(common_CFLAGS) -Isrc -I$(fltk)/build -I$(fltk) \
 $(shell $(fltk)/build/fltk-config --cxxflags | tr ' ' '\n' | grep '^-D.*')

#ifneq ($(STATIC_FLTK),no)
#CXXFLAGS += \
# -DFLTK_VERSION=\"$(shell cat $(fltk)/VERSION)\" \
# -DREVISION=\"$(shell cat $(fltk)/revision)\"
#endif

ifneq ($(WITH_DEFAULT_ICON),no)
CXXFLAGS += -DWITH_DEFAULT_ICON
endif
ifneq ($(WITH_CALENDAR),no)
CXXFLAGS += -DWITH_CALENDAR
OBJS += src/calendar.o src/Flek/FDate.o src/Flek/Fl_Calendar.o
endif
ifneq ($(WITH_COLOR),no)
CXXFLAGS += -DWITH_COLOR
OBJS += src/color.o
endif
ifneq ($(WITH_DND),no)
CXXFLAGS += -DWITH_DND
OBJS += src/dnd.o
endif
ifneq ($(WITH_ENTRY),no)
CXXFLAGS += -DWITH_ENTRY
OBJS += src/input.o
endif
ifneq ($(WITH_FILE),no)
CXXFLAGS += -DWITH_FILE
OBJS += src/file.o
endif
ifneq ($(WITH_HTML),no)
CXXFLAGS += -DWITH_HTML
OBJS += src/html.o
endif
ifneq ($(WITH_NOTIFY),no)
CXXFLAGS += -DWITH_NOTIFY
CXXFLAGS += $(shell pkg-config --cflags libnotify)
OBJS += src/notify.o
endif
ifneq ($(WITH_PASSWORD),no)
CXXFLAGS += -DWITH_PASSWORD
OBJS += src/password.o
endif
ifneq ($(WITH_PROGRESS),no)
CXXFLAGS += -DWITH_PROGRESS
OBJS += src/progress.o
endif
ifneq ($(WITH_SCALE),no)
CXXFLAGS += -DWITH_SCALE
OBJS += src/slider.o
endif
ifneq ($(WITH_TEXTINFO),no)
CXXFLAGS += -DWITH_TEXTINFO
OBJS += src/textinfo.o
endif
ifneq ($(WITH_WINDOW_ICON),no)
CXXFLAGS += -DWITH_WINDOW_ICON
OBJS += src/window_icon.o
endif

ifneq ($(WITH_NOTIFY),no)
ifneq ($(DYNAMIC_NOTIFY),no)
CXXFLAGS += -DDYNAMIC_NOTIFY
LIBS += -ldl
else
LIBS += $(shell pkg-config --libs libnotify)
endif
endif


fltk_CFLAGS := $(common_CFLAGS) \
  -Wno-unused-parameter -Wno-missing-field-initializers

cmake_config = \
  -DCMAKE_BUILD_TYPE="None" \
  -DCMAKE_CXX_FLAGS="$(fltk_CFLAGS)" \
  -DCMAKE_C_FLAGS="$(fltk_CFLAGS)" \
  -DCMAKE_EXE_LINKER_FLAGS="$(LDFLAGS)" \
  -DOPTION_USE_GL="OFF" \
  -DOPTION_OPTIM="$(OPT)"

LIBS += $(fltk)/build/lib/libfltk_images.a

ifneq ($(SYSTEM_JPEG),no)
LIBS += -ljpeg
else
cmake_config += -DOPTION_USE_SYSTEM_LIBJPEG="OFF"
LIBS += $(fltk)/build/lib/libfltk_jpeg.a
endif

ifneq ($(SYSTEM_PNG),no)
LIBS += -lpng
else
libpng_a = libpng16/build/libpng.a
cmake_config += -DOPTION_USE_SYSTEM_LIBPNG="ON" \
 -DHAVE_LIBPNG_PNG_H="$(CURDIR)/libpng16/build/png.h" \
 -DLIB_png="$(CURDIR)/$(libpng_a)"
LIBS += $(libpng_a)
endif

ifneq ($(SYSTEM_ZLIB),no)
LIBS += -lz
else
cmake_config += -DOPTION_USE_SYSTEM_ZLIB="OFF"
LIBS += $(fltk)/build/lib/libfltk_z.a
endif

LIBS += $(fltk)/build/lib/libfltk.a \
 $(shell $(fltk)/build/fltk-config --use-images --ldflags)

ifeq ($(V),1)
cmake_vebose = -DCMAKE_VERBOSE_MAKEFILE="ON"
else
silent = @
endif



all: $(BIN)

configure: checkout-sources
	autoconf

checkout-sources: libpng16 $(fltk)

clean: mostlyclean
	[ ! -f $(fltk)/build/Makefile ] || $(MAKE) -C $(fltk)/build clean
	[ ! -f libpng16/build/Makefile ] || $(MAKE) -C libpng16/build clean

distclean: mostlyclean
	-rm -rf $(fltk)/build libpng16/build autom4te.cache
	-rm -f config.mak config.log config.status
	-rm -f src/fltk.xpm

mostlyclean:
	-rm -f $(BIN) src/*.o src/Flek/*.o

clobber: mostlyclean
	-rm -rf $(fltk) libpng16 autom4te.cache
	-rm -f configure config.mak config.log config.status
	-rm -f $(libpng_tarball)

$(OBJS): $(libpng_a) $(fltk)/build/lib/libfltk.a

$(BIN): $(OBJS)
	@echo "Linking CXX executable $@"
	$(silent)$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

.cpp.o:
	@echo "Building CXX object $@"
	$(silent)$(CXX) $(CXXFLAGS) -c -o $@ $<

libpng16: libpng16/checkout_stamp
libpng16/checkout_stamp:
	rm -rf `dirname $@` $(libpng_tarball) && \
  wget "https://sourceforge.net/projects/libpng/files/libpng16/$(libpng_version)/$(libpng_tarball)" && \
  tar xf $(libpng_tarball) && \
  mv libpng-$(libpng_version) `dirname $@` && \
  rm -f $(libpng_tarball) && \
  touch $@

$(fltk):
	svn co --username="" --password="" "http://seriss.com/public/fltk/fltk/branches/branch-1.3" $@; \
  LANG=C svn info $@ | grep '^Revision:' | cut -d' ' -f2 > $@/revision

$(fltk)/build/Makefile: $(fltk)
	mkdir -p $(fltk)/build
	cd $(fltk)/build && cmake .. $(cmake_config) $(cmake_vebose)

libpng16/build/Makefile: libpng16
	mkdir -p libpng16/build
	cd libpng16/build && cmake .. $(cmake_vebose) \
  -DCMAKE_BUILD_TYPE="None" \
  -DCMAKE_CXX_FLAGS="$(common_CFLAGS)" \
  -DCMAKE_C_FLAGS="$(common_CFLAGS)" \
  -DCMAKE_EXE_LINKER_FLAGS="$(LDFLAGS)" \
  -DPNG_DEBUG="OFF" \
  -DPNG_SHARED="OFF" \
  -DPNG_STATIC="ON"

$(fltk)/build/fltk-config: $(fltk)/build/lib/libfltk.a

libpng16/build/libpng.a: libpng16/build/Makefile
	$(MAKE) -C libpng16/build

$(fltk)/build/lib/libfltk.a: $(libpng16_a) $(fltk)/build/Makefile
	$(MAKE) -C $(fltk)/build

$(fltk)/fltk.xpm: $(fltk)

src/about.cpp: src/fltk.xpm

src/fltk.xpm: $(fltk)/fltk.xpm
	sed 's/static char/static const char/' $< > $@

src/about.o src/html.o src/main.o src/window_icon.o: CXXFLAGS+=-Wno-unused-parameter

