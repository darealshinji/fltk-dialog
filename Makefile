ifneq ($(wildcard config.mak),)
include config.mak
endif

# set to "no" if you want to link
# against system libraries
LOCAL_JPEG    ?= yes
LOCAL_PNG     ?= yes
LOCAL_ZLIB    ?= yes

# yes: FLTK version string will be set statically;
#   don't use this on shared libraries!
# no: version string will be obtained dynamically
#   from the linked in library; works with statically
#   linked FLTK too, but makes more sense on shared libs
# TODO: actually use the system libs or build shared
#   libs if disabled
STATIC_FLTK   ?= yes

# set to "no" if you don't want an embedded FLKT
# icon to appear in taskbar and windows
WITH_DEFAULT_ICON ?= yes

# set to "no" to disable certain features
WITH_CALENDAR ?= yes
WITH_COLOR    ?= yes
WITH_ENTRY    ?= yes
WITH_FILE     ?= yes
WITH_HTML     ?= yes
WITH_PASSWORD ?= yes
WITH_PROGRESS ?= yes
WITH_SCALE    ?= yes
WITH_TEXTINFO ?= yes
WITH_WINDOW_ICON ?= yes

# checkout directory for FLTK
fltk = fltk-1.3

BIN = fltk-dialog
OBJS = $(addprefix src/,about.o choice.o message.o translate.o version.o main.o)

OPT ?= -Os

common_CXXFLAGS := $(OPT) -Wall -Wextra \
 -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 \
 -ffunction-sections -fdata-sections

LDFLAGS += \
 -s -Wl,-O1 -Wl,-z,defs -Wl,-z,relro -Wl,--as-needed -Wl,--gc-sections

CXXFLAGS += $(common_CXXFLAGS) -Isrc -I$(fltk) \
 $(shell $(fltk)/fltk-config --cxxflags | tr ' ' '\n' | grep '^-D.*')

ifneq ($(STATIC_FLTK),no)
CXXFLAGS += \
 -DFLTK_VERSION=\"$(shell cat $(fltk)/VERSION)\" \
 -DREVISION=\"$(shell cat $(fltk)/revision)\"
endif

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
OBJS += src/window_icon.o src/xbm2xpm.o
endif

fltk_CXXFLAGS := $(common_CXXFLAGS) \
 -Wno-unused-parameter -Wno-missing-field-initializers

LIBS = $(shell $(fltk)/fltk-config --ldflags --use-images) -lm

fltk_config = \
 --enable-static \
 --disable-shared \
 --disable-debug \
 --disable-gl \
 --enable-threads
ifneq ($(LOCAL_JPEG),no)
fltk_config += --enable-localjpeg
endif
ifneq ($(LOCAL_PNG),no)
fltk_config += --enable-localpng
endif
ifneq ($(LOCAL_ZLIB),no)
fltk_config += --enable-localzlib
endif

define CLEAN
	-rm -f $(BIN) src/*.o src/Flek/*.o
	[ ! -f $(fltk)/Makefile ] || $(MAKE) -C $(fltk) $@
endef


all: $(BIN)

clean:
	$(CLEAN)

distclean:
	$(CLEAN)
	-rm -rf $(fltk)/autom4te.cache

mostlyclean:
	-rm -f $(BIN) src/*.o src/Flek/*.o

clobber: mostlyclean
	-rm -rf $(fltk)

$(OBJS): $(fltk)/lib/libfltk.a

$(BIN): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

$(fltk):
	svn co --username="" --password="" "http://seriss.com/public/fltk/fltk/branches/branch-1.3" $@; \
  LANG=C svn info $@ | grep '^Revision:' | cut -d' ' -f2 > $@/revision

$(fltk)/fltk-config: $(fltk)
	test -x $@ || (cd $< && NOCONFIGURE=1 ./autogen.sh && \
  CXXFLAGS="$(fltk_CXXFLAGS)" LDFLAGS="$(LDFLAGS)" \
  ./configure $(fltk_config))

$(fltk)/lib/libfltk.a: $(fltk)/fltk-config
	$(MAKE) -C $(fltk)

src/about.o src/html.o src/main.o src/window_icon.o: CXXFLAGS+=-Wno-unused-parameter

